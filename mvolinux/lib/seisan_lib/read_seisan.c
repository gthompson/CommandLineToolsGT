/***************************************************************
 *  read_seisan.c
 *
 *  Functions to read seisan wavefiles
 *
 *  Used by: rbuffer2rsam, rbuffer2trig, rbuffer2heli
 *
 * compiled by makefiles for any of above
 *
 ***************************************************************/

#include "seisan.h"
#include <dirent.h>
#include <time.h>

/***************************************************************
 * name:        read_seisan_event_header
 * arguments:   file pointer to wavfile
 *              pointer to event header structure
 * return:      0 or 1 on error
 * description: reads next record from wavfile and parses that it is an event header.
 *
 ***************************************************************/

int read_seisan_event_header ( FILE *fp, EventHead *head, int swap_flag)
{
	struct tm tp;            /* Time structure used to get time_t value */

	char *buffer = 0;
	char temp_str[10];
	double msec;
	int century;

	/* Read in next record */
	if ((buffer=read_seisan_record(fp,swap_flag)) == NULL) {
		fprintf(stderr, "ERROR: read_seisan_event_header: Can't read next record\n");
		return 1;
	}

	/* Read number of channels */
	strncpy (temp_str, buffer+30,3);
	temp_str[3] = '\0';
	head->num_chan =  atoi (temp_str);

	/* Read bits of date/time */
	strncpy (temp_str, buffer+33,1);
	temp_str[1] = '\0';
	century =  atoi (temp_str);
	if (century==1)
		century=2000;
	else
		century=1900;

	strncpy (temp_str, buffer+34,2);
	temp_str[2] = '\0';
	head->year  = atoi (temp_str) + century;

	strncpy (temp_str, buffer+37,3);
	temp_str[3] = '\0';
	head->doy   = atoi (temp_str) - 1;

	strncpy (temp_str, buffer+41,2);
	temp_str[2] = '\0';
	head->month = atoi (temp_str) - 1;

	strncpy (temp_str, buffer+44,2);
	temp_str[2] = '\0';
	head->day   = atoi (temp_str);
	
	strncpy (temp_str, buffer+47,2);
	temp_str[2] = '\0';
	head->hour  = atoi (temp_str);

	strncpy (temp_str, buffer+50,2);
	temp_str[2] = '\0';
	head->min   = atoi (temp_str);

	strncpy (temp_str, buffer+53,6);
	temp_str[6] = '\0';
	head->sec   = (double)(atol (temp_str));
	msec = head->sec - (int)head->sec;

	strncpy (temp_str, buffer+60,9);
	temp_str[9] = '\0';
	head->win   = (double)(atol (temp_str));
	
	free(buffer);

	/* Get event time in seconds since epoch */
	tp.tm_year = head->year - 1900;
	tp.tm_mon  = head->month;
	tp.tm_mday = head->day;
	tp.tm_hour = head->hour;
	tp.tm_min  = head->min;
	tp.tm_sec  = (int)head->sec;
	head->StartTime = mktime(&tp) + msec;
	
	return 0;
}

/***************************************************************
 * name:        read_seisan_chan_header
 * arguments:   file pointer to wavfile and headers for event
 *              pointer to event header structure
 * return:      0 or 1 on error or 2 for station without calibration file
 * description: reads next record from wavfile and parses it as a channel header.
 *
 ***************************************************************/

int read_seisan_chan_header ( FILE *fp, EventHead *event_head, ChanHead *chan_head, int swap_flag)
{
	char *buffer = 0;
	char temp_str[10];
	double sensitivity = 0;
	double mediagain = 0;
	int i;

	/* Read in next record */
	if ((buffer=read_seisan_record(fp,swap_flag)) == NULL) {
		fprintf(stderr, "ERROR: read_seisan_chan_header: Can't read next record\n");
		return 1;
	}

	/* Store whole header line */
	/* - easier to amend it when writing new header than to store every value */
	strncpy (chan_head->wholeHeader,buffer,CHANHEADLEN);

	/* Get station code 5 chars long but only want 4 non spaces */
	strncpy (chan_head->sta, buffer,5);
	chan_head->sta[5] = '\0';
	for (i=0; i<5; i++)
		if (chan_head->sta[i] == ' '){
			chan_head->sta[i]=chan_head->sta[i+1];
			chan_head->sta[i+1] = ' ';
		}
		
	/* Get channel code 4 chars long */
	strncpy (chan_head->chan, buffer+5,4);
	chan_head->chan[4] = '\0';

	/* Get sample rate */
	strncpy (temp_str, buffer+36,7);
	temp_str[7] = '\0';
	chan_head->samprate = (double)(atol (temp_str));

	/* Get number of samples */
	strncpy (temp_str, buffer+44,6);
	temp_str[6] = '\0';
	chan_head->num_samp = atoi(temp_str);

	/* Get size of each sample */
	strncpy (temp_str, buffer+76,1);
	temp_str[1] = '\0';
	chan_head->dataSize = atoi(temp_str);

	/* Character 78 is blank if seismometer period/gain used for response */
	/* (could also be P or T in theory). */
	if (*(buffer + 77) == ' ') {
		/* Read required numbers. */
		strncpy (temp_str, buffer+176,8);
		temp_str[9] = '\0';
		sensitivity = (double)(atol(temp_str));

		strncpy (temp_str, buffer+192,8);
		temp_str[9] = '\0';
		mediagain = (double)(atol(temp_str));
	}
	free(buffer);

	/* If gains in wavefile then calculate factor. */
	if ( sensitivity && mediagain ) {
		chan_head->gain_factor = 1/(sensitivity*mediagain);
	}
	/* Otherwise get factor from calibration file. */
	else{
		if (i = get_gain_factor(event_head,chan_head))
			return i;
	}

	return 0;
}

/***************************************************************
 * name:        read_seisan_record
 * arguments:   file pointer to wavfile
 * return:      pointer to next record or NULL on error
 * description: reads next record from wavfile.
 *
 ***************************************************************/
char *read_seisan_record (FILE *fp, int swap_flag)
{
	long record_length;
	char *buffer;

	/* Read the length from the file */
	if (fread (&record_length, sizeof(long), 1, fp) != 1) {
		fprintf(stderr, "ERROR: read_seisan_record: Can't read length\n");
		return NULL;
	}
	if (swap_flag)
		swap_long(&record_length,1);

	/* Allocate memory to hold record */
	if ((buffer = (char *) malloc ((size_t) record_length)) ==  NULL) {
		fprintf(stderr, "ERROR: read_seisan_record: Can't allocate memory (len=%d)\n",record_length);
		return NULL;
	}

	/* Read the record */
	if (fread (buffer, sizeof(char), record_length, fp) != record_length) {
		fprintf (stderr,"ERROR: read_seisan_record: Can't read buffer: %s\n",strerror(errno));
		return NULL;
	}

	/* (dummy) read the length from the file again */
	if (fread (&record_length, sizeof(long), 1, fp) != 1) {
		fprintf(stderr, "ERROR: read_seisan_record: Can't read length (dummy)\n");
		return NULL;
	}
	return buffer;
}

/**************************************************************************
 * name:        get_gain_factor
 * arguments:   header structures for an event file
 * return:      0 on success or 1 on failure or 2 if station without calibration file
 * description: get gain for the given channel from response file 
 *
 **************************************************************************/

int get_gain_factor (EventHead *event_head, ChanHead *chan_head)
{
	FILE *fp;
	DIR *dir_handle;
	struct dirent *dir_details;
	struct tm tp; 
	time_t CalTime;
	time_t PrevCalTime = (time_t)0;
	char cal_data [1500];
	char base_name [LINELEN];
	char filename [LINELEN];
	char cal_dir_name [LINELEN];
	int year,month;
	int count, length, byte, remainder, end;
	double sensitivity = 0;
	double mediagain = 0;
	int i;

	/* Generate the base file name for the response file */
	strncpy(base_name,chan_head->sta,4); 	/* 4 char station code */
	base_name[4] = '_';
	strncpy(base_name+5,chan_head->chan,4); /* 4 char chan code */	
	for (i=5; i<9; i++)
		if  (base_name[i] == ' ')
			base_name[i] = '_';           /* Replace spaces with underscore */

	/* Special case of 3 char chan - still need 4 chars in CAL filename */
	/* eg BHZ -> BH_Z (mulplt etc do this too) */
	if (base_name[8] == '_'){
		base_name[8] = base_name[7];
		base_name[7] = '_';
	}
	base_name[9] = '.';
	base_name[10] = '\0';

	/* Find calibration file directory */
	sprintf (cal_dir_name, "%s/CAL", getenv("SEISAN_TOP"));
	if (!(dir_handle = opendir (cal_dir_name))) {
		fprintf (stderr,"ERROR: get_gain_factor: Can't open directory %s\n",cal_dir_name);
	    return 1;
	}

	/* Loop over files in directory */
	filename[0]='\0';
	while (dir_details = readdir(dir_handle))
	{
		/* Do the base filenames match? */
		if (strncmp (base_name, dir_details->d_name, strlen(base_name)) != 0)
			continue;
\
		/* Yes - extract the 'valid from' time for this file */
		if (sscanf (dir_details->d_name+10,"%4d-%2d-%2d-%2d%2d",
			                                &year,&month,&tp.tm_mday,&tp.tm_hour,&tp.tm_min) != 5)
		{
			fprintf (stderr,"ERROR: get_gain_factor: parsing filename %s (base=%s)\n",dir_details->d_name,base_name);
	    	return 1;
		}
		/* Get calibration time in seconds since epoch */
		tp.tm_year = year - 1900;
		tp.tm_mon  = month - 1;
		CalTime = mktime(&tp);

		/* Is the cal file before the event time? */
		if (event_head->StartTime > CalTime)
		{
			/* Yes - is this the latest such cal file? */
			if (CalTime > PrevCalTime)
			{
				/* Yes - store the time */
				PrevCalTime = CalTime;
                
				/* Remember the full path to the file */
				sprintf (filename, "%s/%s",cal_dir_name,dir_details->d_name);
			}
		} 
	}                           /* while read dir */
	closedir (dir_handle);
	if (filename[0]=='\0'){
		/* Happens for pressure sensors and other unwanted channels */
		if (debug)
			printf (" get_gain_factor:  Can't find file for %s\n",base_name);
	    return 2;
	}
	/* Open the response data file */
	if (!(fp = fopen(filename, "r"))){
		fprintf (stderr,"ERROR: get_gain_factor: Can't open %s\n",filename);
	    return 1;
	}

	/* Read the data */
	count = 0;
	while ((count<1040)) {

		/* get the next byte */
		byte = fgetc (fp);

		/* have we found end of file ?? */
		if (byte == EOF){
			if (count < 1000) {
				fprintf (stderr,"ERROR: get_gain_factor: reading %s\n",filename);
	    		return 1;
			}
			while (count<1040) cal_data [count ++] = ' ';
		}
		/* have we found newline */
		else if (byte < ' ') {
			remainder = count % 80;
			if (remainder) {
				end = (count + 80) - remainder;
				while (count < end) cal_data [count ++] = ' ';
			}
		}

		/* store this data byte */
		else cal_data [count ++] = byte;
	}
	if (fclose (fp)){
		fprintf (stderr,"ERROR: get_gain_factor: Can't close %s\n",filename);
	    return 1;
	}

	/* Calculate gain factor */
	sensitivity = atof (cal_data + 176);
	mediagain = atof (cal_data + 192);
	chan_head->gain_factor = 1/(sensitivity*mediagain);

	return 0;  
}

/**************************************************************************
 *	Name: get_data
 *	Input:	sampPerSec - sampling rate expected of all data
 *          *tp1       - C time structure holding start of requested interval
 *          *tp1       - C time structure holding end of requested interval
 *          totalSamp  - number of samples expected - size allocated to arrays.
 *          nPlot      - number of channels requested
 *          *plot      - array of channel strutures to put data in
 *			
 *    Calls:          read_seisan_event_header(), read_seisan_chan_header(), 
 *                    read_seisan_record() from read_seisan.c
 *	Returns: Either 0 or 1.
 *
 *	Read as many SEISAN rbuffer files as needed to fill time interval given.
 *
 **************************************************************************/

int get_data_time (int sampPerSec, time_t searchStart, time_t searchEnd, int totalSamp, int nPlot, SCNdata *plotSCN)
{
	DIR  *dp_bufferdir;
	FILE *fp_buffer;                    /* Filepointer to ring buffer file */

	struct dirent *nextFile;

	struct tm tp1;                      /* Time structure used with time_t values */
	struct tm tp2;                      /* Time structure used with time_t values */

	char *record;                       /* Temporary buffer - memory allocated in read_seisan_record */
	char buffDir[LINELEN];
	char buffFile[LINELEN];             /* Name of buffer file */
	char system_command[LINELEN];       /* string to pass to system command */

	EventHead event_head;               /* Structure for parameters from event header */
	ChanHead chan_head;                 /* Structure for parameters from channel header */

	int dd,hh,mi;
	int str_length;                     /* number of characters in string */
	int num_header_lines;               /* number of header lines in wavfile */
	int record_length;		            /* length of one record in binary file */
	long *signal;				        /* time series from single WAV-file */
	int skip_chan;                      /* Flag marked if chan has no calibration file */
	int ret;                            /* Return value from function */
	int index;                          /* Index of channel in file */
	int numSamp;                        /* Number of samples read in so far */
	int start,end;
	int swap_flag;
	int i,j;                            /* Loop variables */

	/* Expected number of smaples in each buffer - doesn't have to be true */
	int sampPerFile = BUFFLEN*60*sampPerSec;

	/* Populate time structures for comparison with filenames */
	tp1 = *(localtime(&searchStart));
	tp2 = *(localtime(&searchEnd));

	/* Open WAV directory */
	sprintf(buffDir,"%s/%d/%02d/",RBUFFER_PATH,1900+tp1.tm_year,tp1.tm_mon+1);
	if ((dp_bufferdir = opendir(buffDir)) == NULL){
		fprintf (stderr, "ERROR: get_data: can't open %s\n",buffDir);
		return 1;
	}

	/* Loop over rbuffer files */
	while ( (nextFile = readdir(dp_bufferdir)) != NULL){
		if (nextFile->d_name[0] == '.')
			continue;
		/* Only want to look at rbuffer type files within time range */
		if (sscanf(nextFile->d_name,"%*d-%*d-%2d-%2d%2d",&dd,&hh,&mi) != 3)
			continue;
		if ( tp1.tm_mday > dd ||
		    (tp1.tm_mday == dd && tp1.tm_hour > hh) || 
		    (tp1.tm_mday == dd && tp1.tm_hour == hh && tp1.tm_min >= mi+BUFFLEN))
			continue;
		if ( tp2.tm_mday < dd ||
		    (tp2.tm_mday == dd && tp2.tm_hour < hh) || 
		    (tp2.tm_mday == dd && tp2.tm_hour == hh && tp2.tm_min < mi))
			break;

		strcpy(buffFile,buffDir);
		strcat(buffFile,nextFile->d_name);

		if (debug)
			printf("Reading %s\n",buffFile);

		/* Unzip wavfile if necessary */
		str_length = strlen(buffFile);
		if (buffFile[str_length-3] == 'b' &&
	    buffFile[str_length-2] == 'z' &&
		    buffFile[str_length-1] == '2'    )
		{
			sprintf(system_command,"bunzip2 %s",buffFile);
			if (debug) printf("%s\n",system_command);
			if (system (system_command)){
				fprintf (stderr, "ERROR: get_data: Can't unzip %s\n",buffFile);
				exit (1);
			}		
			buffFile[str_length-4] = '\0';
		}

		/* Open rbuffer WAV-file */
		if ((fp_buffer = fopen(buffFile,"r")) == NULL) {
			fprintf (stderr, "ERROR: get_data: can't open %s\n",buffFile);
			return 1;
		}
		swap_flag = is_swap_needed (fp_buffer);

		/* Read the first line of the seisan file - the event header */
		if (read_seisan_event_header(fp_buffer,&event_head,swap_flag)){
			fprintf (stderr,"ERROR: get_data: can't read %s header.\n",buffFile);
			return 1;
		}

		if (debug){
			printf("Event header %d channels %.0f seconds long\n",event_head.num_chan,event_head.win);
			printf("\tstarting at %d/%d ",event_head.year,event_head.doy);
			printf("%d:%d:%.2f (%.2f)\n",event_head.hour,event_head.min,event_head.sec,event_head.StartTime);
		}

		/* Skip station headers in wavfile */	
		num_header_lines = event_head.num_chan/3;
		if (num_header_lines<11)
			num_header_lines = 11;

		/* Skip line 2 - free */
		/* record has memory allocated in read_seisan_record */
		if ((record = read_seisan_record(fp_buffer,swap_flag)) == NULL){
			fprintf (stderr,"ERROR: get_data: skipping headers\n");
			return 1;
		}
		free(record);

		/* Skip station headers in wavfile - at least 10 lines but could be more */	
		num_header_lines = event_head.num_chan/3;
		if (event_head.num_chan%3)
			num_header_lines++;
		if (num_header_lines<10)
			num_header_lines = 10;

		for (i=0; i<num_header_lines; i++){
			if ((record = read_seisan_record(fp_buffer,swap_flag)) == NULL){
				fprintf (stderr,"ERROR: get_data: skipping headers\n");
				return 1;
			}
			free(record);
		}

		/* Loop over channels */
		for (index=1; index<=event_head.num_chan; index++) {

			/* Want to skip channels without calibration files but still
			    need to read in the data so are lined up for next channel. */
			skip_chan=0;

			/* Read channel header for next channel */
			/* read_seisan_chan_header has 3 possible return values 0, 1, or 2 */
			ret = read_seisan_chan_header(fp_buffer,&event_head,&chan_head,swap_flag);
			if ( ret == 1){
				fprintf (stderr,"get_data: %s: can't read chan %d header\n",buffFile,index);
				return 1;
			}
			if ( ret == 2){
				if (debug)
					printf ("get_data: %s: chan %d has no calibration file\n",buffFile,index);
				skip_chan=1;
			}

			/* Allocate memory for array of integer samples. */
			if ((signal = (long *) calloc ((size_t)chan_head.num_samp, sizeof(long))) ==  NULL) {
				fprintf(stderr, "get_data: %s: Can't allocate memory\n",buffFile);
				return 1;
			}

			if (debug){
				printf("Channel %d %s_%s: ",index,chan_head.sta,chan_head.chan);
				printf("%d samps at %.1f\n",chan_head.num_samp,chan_head.samprate);
			}

			/* Read data into signal */
			fread((void*) &record_length, sizeof(int), 1, fp_buffer);
			fread((void*) signal, sizeof(int), chan_head.num_samp, fp_buffer); 
			fread((void*) &record_length, sizeof(int), 1, fp_buffer);
			if (swap_flag)
				swap_long(signal,chan_head.num_samp);

			/* Skip channels without calibration information */
			if (skip_chan){
				free(signal);
				continue;
			}

			/* If sample rate odd then channel is dead - skip it */
			if (chan_head.samprate != sampPerSec ){
				if (debug)
					printf("\tBad sample rate - skip channel %s %s.\n",chan_head.sta,chan_head.chan);
				free(signal);
				continue;
			}
		
			/* Find channel in configuration file */
			for (i=0; i<nPlot; i++){
				if (strcmp(chan_head.sta,plotSCN[i].sta)==0 && strcmp(chan_head.chan,plotSCN[i].chan)==0 ){
					break;
				}
			}
			if (i==nPlot){
				if (debug)
					printf ("Skipping chan %s %s.\n",chan_head.sta,chan_head.chan);
				free(signal);
				continue;
			}
			plotSCN[i].gainFactor = chan_head.gain_factor;

			/* Get start and end points required from rbuffer  */
			if (event_head.StartTime < searchStart){
				start = (searchStart - event_head.StartTime) * sampPerSec;
			}
			else{
				start = 0;
			}
			if (event_head.StartTime + BUFFLEN*60 > searchEnd){
				end = (searchEnd - event_head.StartTime) * sampPerSec;
			}
			else{
				end = sampPerFile;
			}
			if (plotSCN[i].numSamp + end - start > totalSamp){
				fprintf (stderr,"get_data: too many samples (%d) chan %d\n",plotSCN[i].numSamp,i);
				return 1;
			}

			/* If extra samples in file (120001) ignore them */
			/* If missing samples then pad.                  */
			for (j=start; j<end; j++){
				if (j<chan_head.num_samp)
					plotSCN[i].data[plotSCN[i].numSamp++] = signal[j];
				else
					plotSCN[i].data[plotSCN[i].numSamp++] = 0;
			}
			if (debug)
				printf("now %d samples in chan %d\n",plotSCN[i].numSamp,i);

			free(signal);
		} /* channel loop */
		fclose(fp_buffer); 

	} /* read directory loop */
	closedir(dp_bufferdir);

	return 0;
}

/****************************************************************
 * name:        is_swap_needed
 * arguments:   fp - the file to check
 * return:      1 if swap is needed, 0 otherwise
 * description: work out if the given seisan file needs to have binary data byte swapped
 *
 ****************************************************************/
int is_swap_needed (FILE *fp)
{
	int ret_val = 0;
	int length;
 
	rewind (fp);

	if (fread (&length, 1, sizeof (long), fp) == sizeof (long))
		if (length != 80)
			ret_val = 1;

	rewind (fp);
	return ret_val;
}

/**************************************************************************
 * name:        swap_long
 * arguments:   array of integers
 *              number of values in array
 * return:      0
 * description: swap the bytes in an integer array
 *
 **************************************************************************/

int swap_long (long *array, int length)
{
  int count;
  union {char bytes[5]; long number; } swap;

  for (count=0; count<length; count++)
  {
    swap.number = *(array + count);
    swap.bytes [4] = swap.bytes [0];
    swap.bytes [0] = swap.bytes [3];
    swap.bytes [3] = swap.bytes [4];
    swap.bytes [4] = swap.bytes [1];
    swap.bytes [1] = swap.bytes [2];
    swap.bytes [2] = swap.bytes [4];
    *(array + count) = swap.number;
  }
  return 0;
}
