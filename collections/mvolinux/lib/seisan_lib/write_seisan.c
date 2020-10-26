/***************************************************************
 *  write_seisan.c
 *
 *  Functions to write seisan wavefiles
 *
 *  Used by: rbuffer2trig
 *
 * compiled by makefile in ../rbuffer2trig
 *
 ***************************************************************/

#include "seisan.h"

/***************************************************************
 * name:        write_seisan_event_header
 * arguments:   file pointer to wavfile
 *              pointer to event header structure
 * return:      0 or 1 on error
 * description: reads next record from wavfile and parses that it is an event header.
 *
 ***************************************************************/

int write_seisan_event_header (FILE *fp, EventHead *event_head, ChanHead chan_head[])
{
	char headline[EVENTHEADLEN];
	int century,year;
	int num_header_lines;
	int i;

	/* Top line */
	if (event_head->year >= 2000){
		century = 1;
		year = event_head->year - 2000;
	}
	else { 
		century = 0;
		year = event_head->year - 1900;
	}		
	sprintf(headline," %-29s%3d%1d%02d %03d %02d %02d %02d %02d %6.3f %9.3f %10s",
	                  "MVO",event_head->num_chan,century,year,event_head->doy+1,
	                  event_head->month+1,event_head->day,event_head->hour,
					  event_head->min,event_head->sec,event_head->win," ");

	if (write_seisan_record (fp, headline, EVENTHEADLEN))
		return 1;
		
	/* Second line is blank */
	sprintf(headline,"%80s","");

	if (write_seisan_record (fp, headline, EVENTHEADLEN))
		return 1;
		
	/* Channel info */
	for (i=0; i< event_head->num_chan; i+=3){
		if ( i+2 < event_head->num_chan){
			sprintf(headline," %4s%-4s %7.2f %8.2f %4s%-4s %7.2f %8.2f %4s%-4s %7.2f %8.2f  ",
			                 chan_head[i].sta,chan_head[i].chan,0.0,event_head->win,
							 chan_head[i+1].sta,chan_head[i+1].chan,0.0,event_head->win,
			                 chan_head[i+2].sta,chan_head[i+2].chan,0.0,event_head->win);
		}
		else if ( i+1 < event_head->num_chan){
			sprintf(headline," %4s%-4s %7.2f %8.2f %4s%-4s %7.2f %8.2f %27s",
			                 chan_head[i].sta,chan_head[i].chan,0.0,event_head->win,
							 chan_head[i+1].sta,chan_head[i+1].chan,0.0,event_head->win," ");
		}
		else {
			sprintf(headline," %4s%-4s %7.2f %8.2f %53s",
			                 chan_head[i].sta,chan_head[i].chan,0.0,event_head->win," ");
		}
		if (write_seisan_record (fp, headline, EVENTHEADLEN))
			return 1;
	}

	/* Must be at least 10 lines for channel info */
	num_header_lines = event_head->num_chan/3;
	if (event_head->num_chan%3)
		num_header_lines++;
	for (i=num_header_lines; i<10; i++){
		sprintf(headline,"%80s","");
		if (write_seisan_record (fp, headline, EVENTHEADLEN))
			return 1;
	}

	return 0;
}

/***************************************************************
 * name:        write_seisan_chan_header
 * arguments:   file pointer to wavfile
 *              pointer to event header structure
 * return:      0 or 1 on error
 * description: writes channel header.
 *
 ***************************************************************/

int write_seisan_chan_header ( FILE *fp, EventHead *event_head, ChanHead *chan_head)
{
	char headline[CHANHEADLEN];
	int century,year;
	int i;

	if (event_head->year >= 2000){
		century = 1;
		year = event_head->year - 2000;
	}
	else { 
		century = 0;
		year = event_head->year - 1900;
	}

	sprintf(headline,"%-5s%-4s%1d%02d %03d %02d %02d %02d %02d %6.3f %7.2f %6d%26s%1d",
	                  chan_head->sta,chan_head->chan,century,year,
	                  event_head->doy+1,event_head->month+1,event_head->day,
					  event_head->hour,event_head->min,event_head->sec,
	                  chan_head->samprate,chan_head->num_samp,"",chan_head->dataSize);

	/* Take rest of header straight from input file*/
	for (i=78; i<CHANHEADLEN; i++)
		headline[i]=chan_head->wholeHeader[i];

	if (write_seisan_record (fp, headline, CHANHEADLEN)){
		fprintf(stderr, "ERROR: write_seisan_chan_header: %s %s\n",
		        chan_head->sta,chan_head->chan);
		return 1;	
	}
	return 0;
}

/***************************************************************
 * name:        write_seisan_record
 * arguments:   file pointer to wavfile
 * return:      0 or 1 on error
 * description: writes record to wavfile.
 *
 ***************************************************************/
int write_seisan_record (FILE *fp, void *buffer, int record_length)
{

	/* Write record length to file */
	if (fwrite (&record_length, 1, sizeof (long), fp) != sizeof (long)) {
		fprintf(stderr, "ERROR: write_seisan_record: Can't write length\n");
		return 1;
	}

	/* Write record */
	if (fwrite (buffer, 1, record_length, fp) != record_length) {
		fprintf(stderr, "ERROR: write_seisan_record: Can't write buffer len=%d\n",record_length);
		return 1;
	}

	/* Write record length to file again */
	if (fwrite (&record_length, 1, sizeof(long), fp) != sizeof (long)) {
		fprintf(stderr, "ERROR: write_seisan_record: Can't write length (second)\n");
		return 1;
	}

	return 0;
}
