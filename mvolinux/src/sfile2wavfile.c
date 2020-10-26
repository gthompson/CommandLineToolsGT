/*---------------------------------------------------------------------*
 *
 * History:        copied from ampengfft on 2014/01/05 and then modified
 * Arguments:      s-file path/name
 * Calls:          functions from ampengfft_utils.c, matrix.c and read_seisan.c
 * Returns:        0 on success 1 on failure
 *
 *  Calculates a running average across an event and gets maximum average amplitude.
 *  Calculates the fft and finds energy for each of several frequency 'slices'.
 *  Notes the frequency with maximum amplitude in the FFT.
 *  Calculates the total energy in the event.
 *  Writes these pieces of data to the s-file corresponding to the wavfile.
 *
 *  Has been called wav2aef in the past.
 *
 *----------------------------------------------------------------------*/

/* Functions to read seisan wavefiles and associated structures */
#include "../seisan_lib/seisan.h"
#include "matrix.h"
#include "ampengfft.h"

#define NUM_COMMANDS    4      /* Number of required commands in config file */
#define MAXLINES 200
#define SFILELINE 82

/* Configuration file */
char *configfile   = "/live/ampengfft.d";

/* Parameters to be read from a configuration file */
/* debug in seisan.h so can apply to seisan library functions */
int i_amp_avg_window;  /*=2 Number of seconds for moving average window */
int i_pretrig; /*= 20;        /* pre-trigger interval in seconds */
int i_postrig; /*= 2;         /* post-trigger interval in seconds */


/* Local functions */
int ReadConfig (char *configfile);

int main (int argc,char **argv)
{
	FILE *fp_sfile;				        /* Filepointer to s-file */
	FILE *fp_wav;				        /* Filepointer to data trace file */

	char *in_buffer;			        /* input buffer */
	char c_sfilename[LINELEN];		    /* name s-file */
	char c_syscom[LINELEN];             /* string to pass to system command */
	char **c_wavfilename;	            /* array of WAV filenames */

	/* Keep output lines in array as they have to go into middle of s-file (before picks). */
	/* Keep lines for a single wav file in seperate array because they are sorted. */
	char in_line[SFILELINE];                  /* line read from sfile */
	char dummy[SFILELINE];                    /* string to do pattern matching using sscanf */
	char **chan_line;                  /* lines to be written to s-file for channels from a single wav file. */

	EventHead event_head;               /* structure for parameters from event header */
	ChanHead chan_head;                 /* structure for parameters from channel header */

	long record_length;		            /* length of one record in binary file */
	int *i_signal;				        /* whole time series from the WAV-file */
	int swap_flag;                      /* flag - 1 if byte swap of data needed */


	int i_num_wavfiles;				    /* number of wavfiles for event */
	int i_chan;			                /* channel number */
	int i_num_header_lines;             /* number of header lines in wavfile */
	int ret;                            /* return value from function */
	int skip_chan;                      /* flag to mark channel for skipping */
	int num_chan;
	int i,j,k;		                /* loop variables */

	/* Read s-file name and possibly path from command line. */
	if (argc == 2){
		strcpy(c_sfilename,argv[1]);
		if (debug)
			printf("sfile=%s\n",c_sfilename);
	}
	else{
		fprintf (stderr,"USAGE: %s s-file\n", argv[0]);
		exit (1);
	}

	/* Read the configuration file(s) */
	if (ReadConfig( configfile))
		exit(1);
	if (debug)
		printf("Read %s\n",configfile);

	/* Assign memory for wavefile name array - either 1 or 2 */
    c_wavfilename = cmatrix(0,2,0,LINELEN);

	/* Read s-file to get name and directory for wavefile(s). */
	if (get_wavefilename_from_sfile(c_sfilename,c_wavfilename,&i_num_wavfiles)){
		exit(1);
	}
	if (i_num_wavfiles>2){
		fprintf (stderr,"Too many wavefiles listed in %s\n",c_sfilename);
		exit(1);
	}
	if (debug)
		printf("i_num_wavfiles=%d\n",i_num_wavfiles);

	/* Open sfile for reading */
	if ((fp_sfile = fopen(c_sfilename,"r")) == NULL) {
		fprintf (stderr,"ERROR: %s: Failed to open for reading %s\n",argv[0], c_sfilename);
		exit (1);
	}

	/* Read in up to the header for the phase lines and copy to buffer */
	while (fgets(in_line,82,fp_sfile)){
		if (sscanf(in_line,"     STAT CMP   %s",dummy))
			fprintf (stderr,"ERROR: %s this event has ampengfft lines in the Sfile: %s\n",argv[0], c_sfilename);

		if (sscanf(in_line," STAT  SP %s",dummy))
			break;
	}
	/* in_line now has phase header in it */

	/* Loop over wavefiles listed in s-file - maximum 2 in practice */
	for (i=0; i<i_num_wavfiles; i++){

		if (debug)
			printf("wavefile=%s\n",c_wavfilename[i]);

		/* Open WAV-file from WAVDIR */
		if ((fp_wav = fopen(c_wavfilename[i],"rw")) == NULL) {

			/* Try unzipping wavfile. */
			sprintf(c_syscom,"bunzip2 %s.bz2",c_wavfilename[i]);
			if (debug) printf("%s\n",c_syscom);
			system (c_syscom);

			/* Try oppening it again */
			if ((fp_wav = fopen(c_wavfilename[i],"rw")) == NULL) {
				fprintf (stderr, "ERROR: %s: open %s\n",argv[0],c_wavfilename[i]);
				exit (1);
			}
		}
		swap_flag = is_swap_needed (fp_wav);

		/* Read the first line of the seisan file - the event header */
		if (read_seisan_event_header(fp_wav,&event_head,swap_flag)){
			fprintf (stderr,"ERROR: %s: can't read %s header.\n",argv[0],c_wavfilename[i]);
			exit(1);
		}

		if (debug){
			printf("%d chans starting %d/%d ",event_head.num_chan,event_head.year,event_head.doy);
			printf("%d:%d:%.2f\n",event_head.hour,event_head.min,event_head.sec);
		}
		
		/* Skip line 2 - free */
		if ((in_buffer = read_seisan_record(fp_wav,swap_flag)) == NULL){
			fprintf (stderr,"%s skipping headers\n", argv[0]);
			exit(1);
		}
		free(in_buffer);
	
		/* Skip station headers in wavfile - at least 10 lines but could be more */	
		i_num_header_lines = event_head.num_chan/3;
		if (event_head.num_chan%3)
			i_num_header_lines++;
		if (i_num_header_lines<10)
			i_num_header_lines = 10;
	
		for (j=0; j<i_num_header_lines; j++){
			if ((in_buffer = read_seisan_record(fp_wav,swap_flag)) == NULL){
				fprintf (stderr,"%s skipping headers\n", argv[0]);
				exit(1);
			}
			free(in_buffer);
		}

		/* Allocate memory for array of lines of text */
		chan_line = cmatrix(0,event_head.num_chan,0,82);
		num_chan = 0;
 
		/* Loop over channels */
		for (i_chan=1; i_chan<=event_head.num_chan; i_chan++) {
			skip_chan=0;

			/* Read channel header for next channel */
			/* read_seisan_chan_header has 3 possible return values 0, 1, or 2 */
			ret = read_seisan_chan_header(fp_wav,&event_head,&chan_head,swap_flag);
			if ( ret == 1){
				fprintf (stderr,"%s can't read chan %d header\n",argv[0],i_chan);
				exit(1);
			}
			if ( ret == 2){
				if (debug)
					printf("Chan %d has no calibration file\n",i_chan);
				skip_chan=1;
			}

			/* Allocate memory for array of integer samples. */
			i_signal = ivector(1,chan_head.num_samp);

			/* Skip channels for whatever reason. */
			if (skip_chan==1 || chan_head.sta[0] != 'M' || 
			   (chan_head.sta[1] != 'B' && chan_head.sta[1] != 'S'))
			{
				fread(&record_length, sizeof(long), 1, fp_wav);
				fread(&i_signal[1], sizeof(long), chan_head.num_samp, fp_wav); 
				fread(&record_length, sizeof(long), 1, fp_wav);
				free_ivector(i_signal,1,chan_head.num_samp);

				if (debug)
					printf("Skipping chan %s %s\n",chan_head.sta,chan_head.chan);

				continue;
			}

			if (debug){
				printf("Channel %d/%d %s %s ",i_chan,event_head.num_chan,chan_head.sta,chan_head.chan);
				printf("%d samples at %.1f sps ",chan_head.num_samp,chan_head.samprate);
				printf("gain factor = %g\n",chan_head.gain_factor);
			}

			/* Read the length from the file */
			if (fread (&record_length, sizeof(long), 1, fp_wav) != 1) {
				fprintf(stderr, "ERROR: %s: Can't read length\n", argv[0]);
				exit(1);
			}

			if (record_length != chan_head.num_samp*sizeof(long)){
				fprintf(stderr, "ERROR: %s: record has wrong length (%d not %d)\n",
				                 argv[0], record_length,chan_head.num_samp);
				exit(1);
			}
			if (fread(&i_signal[1], sizeof(long), chan_head.num_samp, fp_wav) != chan_head.num_samp){ 
				fprintf(stderr, "ERROR: %s: Can't read data\n", argv[0]);
				exit(1);
			}
			if (fread (&record_length, sizeof(long), 1, fp_wav) != 1) {
				fprintf(stderr, "ERROR: %s: Can't read length (second time)\n", argv[0]);
				exit(1);
			}

			free_ivector(i_signal,1,chan_head.num_samp);

		}      /* ENDFOR:loop for each channel */
		fclose(fp_wav); 

		free_cmatrix(chan_line,0,event_head.num_chan,0,84);

	}		/* ENDFOR:loop for each wavfile */
	free_cmatrix(c_wavfilename,0,2,0,LINELEN);


	fclose(fp_sfile);

	exit(0);
}



/***************************************************************
 * name:        ReadConfig
 * arguments:   configfile - filename of configuration file
 * return:      0 or 1 if error
 * description: Use earthworm functions to read in values from a configuration file.
 *
 ***************************************************************/
int ReadConfig( char* configfile)
{
	char   init[NUM_COMMANDS];  /* init flags, one for each required command */
	int    nmiss;               /* number of required commands that were missed */
	char   *com;
	char   *str;
	double val;
	int    nfiles;
	int    success;
	int    i,j;

	/* Set to zero one init flag for each required command */
	for (i = 0; i < NUM_COMMANDS; i++)
		init[i] = 0;

	/* Open the main configuration file */
	nfiles = k_open (configfile);
	if (nfiles == 0)
	{
		printf ("Can't open <%s>; exitting!\n",configfile);
		return 1;
	}

	/* Process all command files */
	while (nfiles > 0)   /* While there are command files open */
	{
		while (k_rd ())        /* Read next line from active file  */
		{
			com = k_str();         /* Get the first token from line */

			/* Ignore blank lines & comments */
			if (!com) continue;
			if (com[0] == '#') continue;

			/* Open a nested configuRation file */
			if (com[0] == '@')
			{
				success = nfiles + 1;
				nfiles  = k_open (&com[1]);
				if (nfiles != success)
				{
					printf ("Can't open <%s>\n",&com[1]);
					return 1;
				}
				continue;
			}

			/* Process anything else as a command */
	/*0*/	else if (k_its ("debug")){
				debug = k_int();
				init[0] = 1;
			}
	/*1*/	else if (k_its ("amp_avg_window")){
				i_amp_avg_window = k_int();
				init[1] = 1;
			}
	/*2*/	else if (k_its ("pretrig")){
				i_pretrig = k_int();
				init[2] = 1;
			}
	/*3*/	else if (k_its ("postrig")){
				i_postrig = k_val();
				init[3] = 1;
			}
			/* Unknown command
			*****************/
			else {
				printf ("<%s> Unknown command in <%s>.\n",com,configfile);
				continue;
			}

			/* See if there were any errors processing the command
			*****************************************************/
			if (k_err ()){
				printf ("Bad <%s> command in <%s>; exitting!\n",com,configfile);
				return 1;
			}

		} /** while k_rd() **/

		nfiles = k_close ();

	} /** while nfiles **/

	/* After all files are closed, check init flags for missed commands
	******************************************************************/
	nmiss = 0;
	for (i = 0; i < NUM_COMMANDS; i++)
		if (!init[i])
			nmiss++;

	if (nmiss){
		printf ("ERROR, no ");
		if (!init[0]) printf ("<debug> " );
		if (!init[1]) printf ("<amp_avg_window> " );
		if (!init[2]) printf ("<pretrig> " );
		if (!init[3]) printf ("<postrig> " );


		printf ("command(s) in <%s>; exitting!\n", configfile);
		return 1;
	}

	return 0;
}
