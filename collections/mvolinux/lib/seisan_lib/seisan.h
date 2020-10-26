/* Include file for use with read_seisan.c and write_seisan.c */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define STATION_LEN      5   /* max string-length of station code */
#define CHAN_LEN         5   /* max string-length of component code   */
#define NETWORK_LEN      8   /* Max string-length of network code       */
#define LINELEN        256   /* max length of file names, system commands etc */
#define EVENTHEADLEN    80   /* length of records making up event header lines */
#define CHANHEADLEN   1040   /* length of records making up channel header lines */
#define BUFFLEN         20   /* length of ring buffer files in minutes */

/* rbuffers now in two places - they get written to RBUFFER_PATH by earthworm
 * but are moved to OLD_RBUFFER_PATH after being backed up to tape.                  
 * NB - this assumes that whole months are in either one place or another.     */
#define RBUFFER_PATH      getenv("RBUFFER_PATH")
#define OLD_RBUFFER_PATH  getenv("OLD_RBUFFER_PATH")

/* Structure for information extracted from SEISAN wavefile event header */
typedef struct EventHead_struct {

	int num_chan;                         /* Number of channels in file */
	int year;
	int doy;                              /* day of year  0-364 */
	int month;                            /* 0-11 */
	int day;
	int hour;
	int min;
	double sec;
	double win;                          /* Length of file in seconds */
	double StartTime;                    /* time as seconds since epoch */

} EventHead;

/* Structure for information extracted from SEISAN wavefile channel header */
typedef struct ChanHead_struct {

	char sta[STATION_LEN];                /* Station name */
	char chan[CHAN_LEN];                  /* Channel code */
	double samprate;                      /* Number of samples per second */
	int num_samp;                         /* Number of samples present for channel */
	double gain_factor;                   /* Seismometer gain constant (m/s)/count */
	int dataSize;                         /* 2 or 4 for 2 or 4 byte integer */
	char wholeHeader[CHANHEADLEN+1];      /* Everything */
	
} ChanHead;

/* Structure for data for each SCN
 *********************************/
typedef struct SCNdata_struct {

	char       sta[STATION_LEN];   /* Station name                            */
	char       chan[CHAN_LEN];     /* Channel code                            */
	char       net[NETWORK_LEN];   /* Network name                            */
	double     gainFactor;         /* Seismometer gain constant (m/s)/count   */
	long       *data;              /* Store time samples until have a minute  */
	int        numSamp;            /* Number of samples in data               */

} SCNdata;

/* Debug - value set at top of main or in config file, kept here so library functions see it */
int debug;

/* Functions included in read_seisan.c and write_seisan.c */
int read_seisan_event_header ( FILE *fp, EventHead *head, int swap_flag);
int read_seisan_chan_header  ( FILE *fp, EventHead *event_head, ChanHead *chan_head, int swap_flag);
char *read_seisan_record (FILE *fp, int swap_flag);
int write_seisan_event_header (FILE *fp, EventHead *event_head, ChanHead chan_head[]);
int write_seisan_chan_header ( FILE *fp, EventHead *event_head, ChanHead *chan_head);
int write_seisan_record (FILE *fp, void *buffer, int record_length);
int get_gain_factor (EventHead *event_head, ChanHead *chan_head);
int get_data_time(int sampPerSec, time_t searchStart, time_t searchEnd, int totalSamp, int nPlot, SCNdata *plotSCN);
int is_swap_needed (FILE *fp);
int swap_long(long *array, int length);
