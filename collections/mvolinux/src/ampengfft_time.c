/*---------------------------------------------------------------------*
 *
 * Name:           ampengfft_time
 * Arguments:      start_time stop_time  (times inclusive and given to the minute)
 *                 e.g.  rbuffer2trig 200505012305 200505012325
 * Calls:          functions from read_seisan.c ampengfft_utils.c and matrix.c
 * Returns:        0 on success 1 on failure
 *
 *  Version of ampengfft that works for a given time interval rather than an event.
 *  Calculates the fft and finds energy for each of several frequency 'slices'.
 *  Notes the frequency with maximum amplitude in the FFT.
 *  Calculates the total energy in the event.
 *
 *----------------------------------------------------------------------*/

/* Functions to read seisan wavefiles and associated structures */
#include "../seisan_lib/seisan.h"

/* Just used to parse the earthworm style configuration file */
#include "../earthworm_lib/kom.h" 

#include "matrix.h"
#include "ampengfft.h"

/* Array of frequencies for numSlice - first slice between 0.1 and 1.0 etc */
double slice_freqs[] = {0.1,1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0,30.0};
int numSlice  = sizeof(slice_freqs) / sizeof(double);

/* Configuration file */
char *configfile = "/live/ampengfft_time.d";
#define NUM_COMMANDS 6                   /* Number of required commands in config file */

/* Parameters to be read from a configuration file */
/* debug in seisan.h so can apply to seisan library functions */
int           sampPerSec;                /* Only accept data with this sampling rate  */
int           ampAvgWin;                 /* Number of seconds for moving average window */
char          outFilename[LINELEN];      /* Name of output file */
double        lowCut;                    /* Ignore frequencies below this for max peak only */
SCNdata 	  *plotSCN = NULL;           /* Which SCNs to get and how to plot them    */
int     	  nPlot = 0;                 /* Number of channels to plot                */

int ReadConfig( char* configfile);

int main (int argc,char **argv)
{
	FILE *fp_out;				        /* Filepointer to s-file */
	FILE *fp_wav;				        /* Filepointer to data trace file */

	struct tm tp1;                      /* Time structure used to get time_t values */
	struct tm tp2;                      /* Time structure used to get time_t values */

	time_t searchStart;
	time_t searchEnd;

	char startString[LINELEN];
	char stopString[LINELEN];
	char **c_line;                      /* lines to be written to output file. */

	ChanHead chan_head;                 /* structure for parameters from channel header */

	double *d_signal;                   /* time series for fft */
	double *d_fft;                      /* fft real-imaginary pairs */
	double *d_spec;                     /* amplitude spectrum */
	double *d_filter;                   /* frequency filter */
	double slice_frac[numSlice];        /* fraction of total energy in each slice */
	double avgAmpMax;                   /* maximum amplitude overall */
	double freqInterval;                /* interval between frequency samples */
	double totalEnergy;                 /* total spectral energy overall */
	double maxFreq;                     /* frequency with maximum amplitude */

	int yyyy1,mm1,dd1,hh1,mi1,ss1;
	int yyyy2,mm2,dd2,hh2,mi2,ss2;
	int sliceSamp[numSlice+1];          /* Array of samples in fft bounding each slice */
	int totalSamp;                      /* number of samples in triggered event */
	int nSampFFT;                       /* number of samples in FFT after pad */
	int chan;                           /* channel number */
	int ret;                            /* return value from function */
	int skip_chan;                      /* flag to mark channel for skipping */
	int num_lines;                      /* number of lines so far in c_line array */
	int i,j;                            /* loop variables */

	char usage[] = "USAGE: ampengfft_time yyyymmdd[hhmi] yyyymmdd[hhmi]\n";

	/* Read command line. */
	if (argc == 3){
		strcpy(startString,argv[1]);
		strcpy(stopString,argv[2]);
	}
	else {
		fprintf (stderr,usage);
		exit (1);
	}

	/* Read the configuration file */
	if (ReadConfig( configfile))
		exit(1);
	if (debug)
		fprintf(stderr,"Read %s\n",configfile);

	/* Parse dates input from command line */
	if ((ret = sscanf(startString,"%4d%2d%2d%2d%2d%2d",&yyyy1,&mm1,&dd1,&hh1,&mi1,&ss1)) <3){
		fprintf (stderr,usage);
		fprintf (stderr,"Can't parse first argument\n");
		exit (1);
	}
	if (ret < 4) hh1 = 0;
	if (ret < 5) mi1 = 0;
	if (ret < 6) ss1 = 0;

	if ((ret = sscanf(stopString,"%4d%2d%2d%2d%2d%2d",&yyyy2,&mm2,&dd2,&hh2,&mi2,&ss2)) <3){
		fprintf (stderr,usage);
		fprintf (stderr,"Can't parse second argument\n");
		exit (1);
	}
	if (ret < 4) hh2 = 0;
	if (ret < 5) mi2 = 0;
	if (ret < 6) ss2 = 0;

	if (yyyy1 != yyyy2 || mm1 != mm2){
		fprintf (stderr,"ERROR: ampengfft_time: Can't proces more than one month at a time.\n");
		exit (1);
	}
	/* Set start and end  */
	tp1.tm_year = yyyy1 - 1900;
	tp1.tm_mon  = mm1 - 1;
	tp1.tm_mday = dd1;
	tp1.tm_hour = hh1;
	tp1.tm_min  = mi1;
	tp1.tm_sec  = ss1;
	searchStart = mktime(&tp1);

	tp2.tm_year = yyyy2 - 1900;
	tp2.tm_mon  = mm2 - 1;
	tp2.tm_mday = dd2;
	tp2.tm_hour = hh2;
	tp2.tm_min  = mi2;
	tp2.tm_sec  = ss2;
	searchEnd = mktime(&tp2);

	if (searchEnd < searchStart){
		fprintf (stderr,"ERROR: ampengfft_time: %s after %s\n",startString,stopString);
		exit (1);
	}

	/* Number of samples needed to give time range requested */
	totalSamp = (searchEnd - searchStart) * sampPerSec;
	if (debug)
		printf("Requested %d samples\n",totalSamp);

	/* Allocate memory for data in channel structures. */
	for (i=0; i<nPlot; i++){
		if ((plotSCN[i].data = (long *) calloc ((size_t)totalSamp, sizeof(long))) ==  NULL) {
			fprintf(stderr, "ERROR: ampengfft_time: %s: Can't allocate memory\n");
			exit(1);
		}
	}

	/* Get data from rbuffers into SCN structure */
	if (get_data_time(sampPerSec, searchStart, searchEnd, totalSamp, nPlot, plotSCN)){
		fprintf (stderr,"ERROR: ampengfft_time: Problem in get_data\n");
		exit (1);
	}

	/* Check that got enough data */
	for (i=0; i<nPlot; i++)
		if (plotSCN[i].numSamp != totalSamp)
			fprintf (stderr,"WARNING: ampengfft_time: Only %d samples for %s %s\n",plotSCN[i].numSamp,plotSCN[i].sta,plotSCN[i].chan);

	if (debug)
		printf("Read in %d samples\n",totalSamp);

	/* Allocate memory for array of lines of text */
	c_line = cmatrix(0,nPlot,0,82);
	num_lines = 0;

	/* Open output file */
	if ((fp_out = fopen(outFilename,"w")) == NULL) {
		fprintf (stderr,"ERROR: ampengfft_time: Failed to open %s\n",outFilename);
		exit (1);
	}

	/* Write header for ampengfft bit.*/
	fprintf(fp_out," Analysis between %s and %s\n",startString,stopString);
    fprintf(fp_out,"      STAT CMP   MAX AVG  TOTAL ENG             ");
    fprintf(fp_out,"FREQUENCY BINS (Hz)       MAX\n");
	fprintf(fp_out,"                AMP (m/s)   (J/kg)   ");
	for (i=0; i<numSlice; i++)
		fprintf(fp_out,"%-3.0f",slice_freqs[i]);
	fprintf(fp_out," (Hz)\n");


	/* Calculte number of samples suitable for FFT */
	nSampFFT = num_samp_fft(totalSamp);

	/* Replace FFT slice boundaries with frequency sample numbers */
	freqInterval = (double)sampPerSec/(double)nSampFFT;
	for (i=0; i<numSlice; i++)
		sliceSamp[i] = ((int)(slice_freqs[i]/freqInterval)) + 1;
	lowCut /= freqInterval;

	/* Loop over channels */
	for (chan=0; chan<nPlot; chan++){

		/* Check that actually any data for this requested channel */
		if (plotSCN[chan].gainFactor == 0)
			continue;

		/* Allocate memory for vectors */
		d_signal  = dvector(1,totalSamp);
		d_fft     = dvector(1,2*nSampFFT);
		d_spec    = dvector(1,nSampFFT);
		d_filter  = dvector(1,nSampFFT);
	
		/* Select data window, apply gains and remove offset */
		for (i=1; i<=totalSamp; i++)
			d_signal[i]=plotSCN[chan].data[i-1]*plotSCN[chan].gainFactor;
		remoff(d_signal, totalSamp);

		/* Pad with 0 up to next power of 2 and set every 2nd value to 0. */
		prepare_for_fft(d_signal, d_fft, totalSamp);

		/* Take FFT. The result is 2n values (n complex) */
		/* ranging from 0 to Nyq then from -Nyq back to 0 */
		four1(d_fft,nSampFFT,1);

		/* Get amplitude spectrum from FFT */
		amplitude_spectrum(d_fft, d_spec, nSampFFT);

		/* Calculate energy slices using amplitude spectrum. */
		slice_energy(sliceSamp,numSlice,d_spec,slice_frac,lowCut,&maxFreq);
		maxFreq *= freqInterval;

		if (debug)
			printf("maxfreq=%.2f\n",maxFreq);
	
		/* Filter in frequency domain to crudely remove effect of transfer function -  then carry out inverse fft. */
		create_filter(d_filter, freqInterval, nSampFFT);
		filter_fft_and_inversefft(d_fft,d_filter,nSampFFT,d_signal,totalSamp);

		/* Calculate total energy in time series. */
		totalEnergy = get_energy(d_signal,totalSamp);

		/* Make all negative samples positive. */
		abs_signal(d_signal, totalSamp);

		/* Find maximum average amplitude */
		avgAmpMax=max_average_amplitude(d_signal,totalSamp,sampPerSec,ampAvgWin);

		/* Write to string in memory */
		strcpy(chan_head.sta,plotSCN[chan].sta);
		strcpy(chan_head.chan,plotSCN[chan].chan);
		write_sfile(c_line[num_lines++],&chan_head,avgAmpMax,totalEnergy,slice_frac,numSlice,maxFreq);

		if (debug)
			printf("%s",c_line[num_lines-1]);

		free_dvector(d_signal,1,totalSamp);
		free_dvector(d_fft,1,2*nSampFFT);
		free_dvector(d_spec,1,nSampFFT);
		free_dvector(d_filter,1,nSampFFT);

	}      /* ENDFOR:loop for each channel */

	/* Sort data lines into alphabetical order */
	/* Use qsort_strcmp to pass right pointers to strcmp */
	qsort((void*)c_line,num_lines,sizeof(c_line[0]),qsort_strcmp);

	/* Write lines to ouput file, do it backwards so get Z comp first */
	for (i=num_lines-1; i>=0; i--)
		fprintf(fp_out,"%s",c_line[i]);

	fclose(fp_out);
	free_cmatrix(c_line,0,nPlot,0,82);

	for (i=0; i<nPlot; i++)
		free(plotSCN[i].data);

	exit (0);
}

/**************************************************************************
 * Name:    ReadConfig
 * Input:   configfile - filename of configuration file
 *
 * Calls:   k_open() ,k_rd, k_str(), k_its(), k_int(), k_val(), k_err from kom.c
 * Returns: Either 0 or 1.
 *
 * Uses earthworm functions to parse a configuration file.
 *
 **************************************************************************/
int ReadConfig( char* configfile)
{
	char init[NUM_COMMANDS]; /* init flags, one for each required command */
	int nmiss;               /* number of required commands that were missed */
	char *com;
	char *str;
	int  nfiles;
	int  success;
	int  i;

	/* Index of plotSCN array */
	nPlot = 0;

	/* Set to zero one init flag for each required command
	*****************************************************/
	for (i = 0; i < NUM_COMMANDS; i++)
		init[i] = 0;

	/* Open the main configuration file
	**********************************/
	nfiles = k_open (configfile);
	if (nfiles == 0)
	{
		printf("ampengfft_time: Can't open <%s>; exitting!\n",configfile);
		return 1;
	}

	/* Process all command files
	***************************/
	while (nfiles > 0)   /* While there are command files open */
	{
		while (k_rd ())        /* Read next line from active file  */
		{
			com = k_str();         /* Get the first token from line */

			/* Ignore blank lines & comments
			*******************************/
			if (!com) continue;
			if (com[0] == '#') continue;

			/* Open a nested configuration file
			**********************************/
			if (com[0] == '@')
			{
				success = nfiles + 1;
				nfiles  = k_open (&com[1]);
				if (nfiles != success)
				{
					printf("ampengfft_time: Can't open <%s>\n",&com[1]);
					return 1;
				}
				continue;
			}

			/* Process anything else as a command
			************************************/
	/*0*/	else if (k_its ("debug")){
				debug = k_int();
				init[0] = 1;
			}
	/*1*/	else if (k_its ("ampAvgWin")){
				ampAvgWin = k_val();
				init[1] = 1;
			}
	/*2*/	else if (k_its ("sampPerSec")){
				sampPerSec = k_int();
				init[2] = 1;
			}
	/*3*/	else if (k_its ("outFilename")){
				if ((str = k_str()) != NULL)
					strcpy( outFilename , str );
				init[3] = 1;
			}
	/*4*/	else if (k_its ("lowCut")){
				lowCut = k_val();
				init[4] = 1;
			}
			/* Enter SCNs and plot params
			*******************************/
	/*5*/	else if (k_its("Plot")){

				/* Allocate another slot in array */
				if ((plotSCN = (SCNdata *)realloc(plotSCN,((nPlot+1)*sizeof(SCNdata)))) == NULL){
					printf("ampengfft_time: plotSCN realloc failed; exiting!\n");
					return 1;
				}

				/* S */
				if ((str = k_str()) != NULL)
					strcpy( plotSCN[nPlot].sta , str );
				
				/* C */
				if ((str = k_str()) != NULL)
					strcpy( plotSCN[nPlot].chan , str );

				/* N */
				if ((str = k_str()) != NULL)
					strcpy( plotSCN[nPlot].net , str );

				plotSCN[nPlot].numSamp    = 0;
				plotSCN[nPlot].gainFactor = 0;

				nPlot++;
				init[5] = 1;
			}
			/* Unknown command
			*****************/
			else {
				printf("ampengfft_time: <%s> Unknown command in <%s>.\n",com,configfile);
				continue;
			}

			/* See if there were any errors processing the command
			*****************************************************/
			if (k_err ()){
				printf("ampengfft_time: Bad <%s> command in <%s>; exitting!\n",com,configfile);
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
		printf( "ampengfft_time: ERROR, no ");
		if (!init[0])  printf( "<debug> " );
		if (!init[1])  printf( "<ampAvgWin> ");
		if (!init[2])  printf( "<sampsPerSec> ");
		if (!init[3])  printf( "<outFilename> ");
		if (!init[4])  printf( "<lowCut> ");
		if (!init[5])  printf( "<Plot> ");

		printf("command(s) in <%s>; exitting!\n", configfile);
		return 1;
	}
	return 0;
}
