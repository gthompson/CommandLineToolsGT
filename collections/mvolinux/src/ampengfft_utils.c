/*---------------------------------------------------------------------*
 *
 *  description: functions used in ampengfft.c
 *
 *----------------------------------------------------------------------*/


#include <dirent.h>
#include <time.h>

/* Functions to read seisan wavefiles and associated structures */
#include "../lib/seisan_lib/seisan.h"

#include "ampengfft.h"

/***************************************************************
 * name:         get_wavefilename_from_sfile
 *
 * arguments:    pointer to char with name of sfile to read
 *               pointer to array of strings holding resulting wavfile names
 *               number of wavfiles listed in s-file
 * return:       0 on success 1 on failure
 * description:  reads line type 6 in an s-file to get name of wavfile
 *
 ***************************************************************/

/* Two formats of wavefile name to cope with: yyyy-mm-dd-hhmi-ssS.MVO___ccc
                                              yymm-dd-hhmi-ssS.MVO_cc_1      */

int get_wavefilename_from_sfile(char* c_sfile, char** c_wavfilename, int* i_num)
{
	FILE *fp_sfile;
	int yy, yyyy ,mm;
	char c_line[LINELEN];
	char c_filename_bit[LINELEN];

	/* Open s-file */
	if ((fp_sfile = fopen(c_sfile,"r")) == NULL) {
		printf ("ERROR: ampengfft: get_wavefilename_from_sfile: ");
		printf ("Failed to open %s\n",c_sfile);
		return 1;
	}

	/* Get wavefile names from s-file */
	*i_num = 0;
	while ( fgets(c_line,LINELEN,fp_sfile) != NULL){
		/* Skip all lines but type 6 lines. */
		if ((strlen(c_line) < 80) || (c_line[79] != '6'))
			continue;

		/* old wavefile name */
		if (c_line[16] == 'S'){
			if (sscanf(c_line," %2d%2d-%s ",&yy,&mm,c_filename_bit) != 3){
				fprintf (stderr,"ERROR: ampengfft: get_wavefilename_from_sfile: ");
				fprintf (stderr,"Failed to parse type 6 line in s-file %s\n",c_sfile);
				return 1;
			}
			if (yy>50)
				yyyy=1900+yy;
			else
				yyyy=2000+yy;

			sprintf(c_wavfilename[*i_num],"%s/%d/%02d/%02d%02d-%s",
		                               getenv("EVENT_WAV_PATH"),yyyy,mm,yy,mm,c_filename_bit);
		}
		/* new wavefile name */
		else{
			if (sscanf(c_line," %4d-%2d-%s ",&yyyy,&mm,c_filename_bit) != 3){
				fprintf (stderr,"ERROR: ampengfft: get_wavefilename_from_sfile: ");
				fprintf (stderr,"Failed to parse type 6 line in s-file %s\n",c_sfile);
				return 1;
			}
			sprintf(c_wavfilename[*i_num],"%s/%d/%02d/%d-%02d-%s",
		                               getenv("EVENT_WAV_PATH"),yyyy,mm,yyyy,mm,c_filename_bit);
		}
		(*i_num)++;
	}

	if (*i_num == 0){
		printf ("ERROR: ampengfft: get_wavefilename_from_sfile: ");
		printf ("Failed to find type 6 line in s-file %s\n",c_sfile);
		return 1;
	}

	close(fp_sfile);

	return 0;
}


/***************************************************************
 * name:        num_samp_fft
 * arguments:   number of samples in time series
 * return:      next power of 2 for FFT calculation
 * description: calculates number of samples required to do FFT over time series.
 *
 ***************************************************************/

int num_samp_fft (int i_num_samp) 
{
	int i_num_samp_fft=0;
	int i=0;
	while ((i_num_samp_fft=pow((double)2,(double)i)) < i_num_samp) {
		i++;
	}
	return i_num_samp_fft;
}

/***************************************************************
 * name:        isDeadChannel
 * arguments:   number of samples in event and sampling rate
 * return:      0 if channel is alive 1 if channel is dead
 * description: guesses that channel is dead if not enough samples.
 *
 ***************************************************************/

int isDeadChannel(double d_samprate, int *i_signal, int i_num_samp) 
{
	int i;
	int first_val;

	if (d_samprate < 75)
		return 1;

	if (i_num_samp < 1000)
		return 1;

	/* Check that all values are not the same */
	first_val =i_signal[1];
	for (i=1; i<i_num_samp; i++)
		if (i_signal[i] != first_val)
			break;
	if (i==i_num_samp)
		return 1;

	return 0;
}

/***************************************************************
 * name:        write_zeroes_for_dead_channel
 * arguments:   string to write to
 *              ID (station, component) of dead channel
 *              number of frequency slices
 * return:      0 on success 1 on failure
 * description: write a single s-file line for a dead channel.
 *
 ***************************************************************/

int write_zeroes_for_dead_channel (char* c_line, ChanHead *head, int i_num_slice)
{
	int j;
	char c_linebit[20];
	char chan3[4];

	/* Only room for 3 chars in chan for listing */
	chan3[0] = head->chan[0];
	if (head->chan[1] == 'B')
		chan3[1] = head->chan[2];
	else
		chan3[1] = head->chan[1];
	chan3[2] = head->chan[3];
	chan3[3] = '\0';

	sprintf(c_line," VOLC %4s %3s A%8f E%8f F ",head->sta,chan3,0.0,0.0);

	for (j=0; j<i_num_slice; j++){
		sprintf(c_linebit,"%-3.0f",0.0);
		strcat(c_line,c_linebit);
	}

	sprintf(c_linebit,"%5.2f 3\n",0.0);
	strcat(c_line,c_linebit);
	return 0;
}

/***************************************************************
 * name:        prepare_for_fft
 * arguments:   pointer to array containing time series
 *              pointer to array containing prepared data for FFT
 *              number of values in first array
 * return:      0
 * description: creates a time series for four1 to work on, 
 *              with every second value 0 and padded to next power of two.
 *
 ***************************************************************/

int prepare_for_fft(double* d_time_i, double* d_fft, int n) 
{
	int j, first;
	int i_nb_fft;

	/* Every other value made zero (Imaginary part.) */
	for(j=1; j<=n; j++){
		d_fft[j*2-1] = d_time_i[j];
		d_fft[j*2] = 0.0;
	}
	
	i_nb_fft = num_samp_fft(n);

	/* Pad to power of two */
	first=n+1;
	for (j=first;j<=i_nb_fft;j++){
		d_fft[j*2-1] = d_fft[j*2] = 0.0;
	}

	return 0;
}

/***************************************************************
 * name:         amplitude_spectrum
 * arguments:    pointer to array containing FFT
 *               pointer to array for amplitude spectrum
 *               number of samples in spectrum (half those in FFT)
 * return:       0
 * description:  for each complex number in FFT calculates the amplitude, ie the velocity.
 *
 ***************************************************************/

int amplitude_spectrum(double* d_fft, double* d_spectrum, int n)
{
	int j;

	for (j=1; j<n; j++){
		d_spectrum[j] = sqrt( d_fft[j*2-1]*d_fft[j*2-1] + d_fft[j*2]*d_fft[j*2] );
	}
	return 0;
}

/***************************************************************
 * name:        slice_energy
 * arguments:   array of frequencies for each slice in number of samples along spectrum
 *              number of frequency slices
 *              array with amplitude spectrum
 *              array for fraction of total energy in each fft slice
 *              lowCut only used to stop maxFreq being microseism peak
 *              pointer for frequency with maximum energy
 * return:      0 or 1
 * description: Calculate energy for each frequency slice, 
 *              also notice highest peak in spectrum and total energy.
 *
 ***************************************************************/

int slice_energy (int* sliceSamp, int numSlice, double* d_spec, double* slice_frac, double lowCut, double* maxFreq ) 
{
	double maxAmp = 0;
	double totalEnergy = 0;
	double sliceEng[numSlice];
	int    j, k;

	/* Loop over frequency slices */
	for (j=0; j<numSlice; j++){

		sliceEng[j]=0;

		/* Loop over samples in slice */
		for (k=sliceSamp[j]; k<=sliceSamp[j+1]; k++){		
			sliceEng[j] += d_spec[k]*d_spec[k];
			if (maxAmp < d_spec[k] && k>lowCut){
				maxAmp = d_spec[k];
				*maxFreq = k;
			}
		}
		totalEnergy += sliceEng[j];
	}

	if (totalEnergy == 0){
		fprintf (stderr,"ERROR: slice_energy: totalEnergy=0\n");
		return 1;
	}

	for (j=0; j<numSlice; j++)
		slice_frac[j] = 100 * sliceEng[j]/totalEnergy;

	return 0;
}

/***************************************************************
 * name:        create_filter
 * arguments:   array for frequency domain filter
 *              interval between samples in Hz
 *              number of samples in data FFT
 * return:      0
 * description: creates a filter removing a generic transfer function in frequency domain.
 *
 ***************************************************************/

int create_filter(double* d_filter, double d_freq_interv, int i_num_samp_fft) 
{
	/*  High pass filter to remove the effect of transfer function */
	const double d_lowcut=0.5;
	const double d_hicut=30;	
	const double d_slope=10;
	double d_ramp, d_rampstart, d_rampstop; 
	double d_lowcuti, d_hicuti;
	double d_pi;
	int j;
	
	d_pi = 2.0 * asin(1.0);

	/* Convert filter frequency to number of frequency samples */
	d_lowcuti = (int)(d_lowcut/d_freq_interv);
	d_hicuti = (int)(d_hicut/d_freq_interv);

	d_ramp= d_lowcuti/d_slope;
	d_rampstart = d_lowcuti-d_ramp;

	for (j=1;j<d_rampstart;j++) { 
		d_filter[j] = 0.0;
	}

	for (j=d_rampstart;j<=d_lowcuti;j++) {
		d_filter[j] = sin(d_pi*((j-d_rampstart)/d_ramp)-(d_pi/2.0)); 
		d_filter[j] = (d_filter[j]+1.0)/2.0;
	}

	for (j=d_lowcuti;j<=d_hicuti;j++) { 
		d_filter[j]=1.0;
	}

	d_ramp= d_hicuti/d_slope;
	d_rampstop = d_hicuti+d_ramp;

	for (j=d_hicuti;j<=d_rampstop;j++){
		  d_filter[j] = sin(d_pi*((j-d_hicuti)/d_ramp)+(d_pi/2.0));
		  d_filter[j] = (d_filter[j]+1.0)/2.0;
	}

	for (j=d_rampstop;j<=i_num_samp_fft/2;j++) {
		 d_filter[j] = 0.0;
	}

	return 0;
}

/***************************************************************
 * name:        filter_fft_and_inversefft
 * arguments:   array with fft of event
 *              array with frequency domain filter
 *              number of samples in data FFT
 *              array for resulting time-series
 *              number of samples in event (before padding)
 * return:      0
 * description: filters spectrum to remove transfer function, then does inverse FFT.
 *
 ***************************************************************/

int filter_fft_and_inversefft(double *d_fft, double* d_filter, 
                              int i_num_samp_fft, double *d_signal, int i_num_samp_event) 
{
	int j,k;

	/* Filter dc and positive frequencies */
	for (j=1;j<=i_num_samp_fft/2;j++){
		d_fft[j*2-1] *= d_filter[j];
		d_fft[j*2] *= d_filter[j];
	}

	/* Filter negative frequencies */
	k=0;
	for (j=i_num_samp_fft;j>i_num_samp_fft/2+1;j--){
		k++;
		d_fft[j*2-1] *= d_filter[k];
		d_fft[j*2] *= d_filter[k];
	}
	
	/* Inverse fft to get filtered time series. */
	four1(d_fft,i_num_samp_fft,-1);
	for(j=1; j<=i_num_samp_event; j++){
		d_signal[j]= d_fft[j*2-1]/i_num_samp_fft;
	}

	return 0;
}

/***************************************************************
 * name:        get_energy
 * arguments:   array with time series (velocity)
 *              number of samples
 * return:      total energy
 * description: squares each velocity sample and sums them to give energy.
 *
 ***************************************************************/

double get_energy(double* d_signal, int n) 
{
	double d_energy;
	int j;

	d_energy=0;
	for (j=1; j<=n; j++) {
		d_energy += d_signal[j]*d_signal[j];
	}
	
	return d_energy;
}

/***************************************************************
 * name:        abs_signal
 * arguments:   array with time series (velocity)
 *              number of samples
 * return:      0
 * description: make negative samples positive in time series.
 *
 ***************************************************************/

int abs_signal(double* d_signal, int n)
{
	int j;

	/* Make amplitudes positive */
	for (j=1; j<=n; j++) {
		while(d_signal[j]<0.0)
			d_signal[j] *= -1;
	}
	return;
}

/***************************************************************
 * name:        max_average_amplitude
 * arguments:   array with time series (velocity)
 *              number of samples
 *              sampling rate
 *              number of seconds to use as moving average window
 * return:      maximum average amplitude
 * description: takes a running average over the time series 
 *              and returns the maximum value.
 *
 ***************************************************************/

double max_average_amplitude(double* d_signal, int i_num_samp_event, 
                             double d_samprate,int i_amp_avg_window) 
{

	double d_max_average_amp;	
	double d_average;				/* average over each i_amp_avg_window seconds */
	int i_num_samp_avg;			/* number of samples for average */
	int j, k, last, lastk;

	d_max_average_amp=0;

	i_num_samp_avg =i_amp_avg_window*d_samprate;
	last = i_num_samp_event - i_num_samp_avg;
	
	for (j=1; j<=last; j+=i_num_samp_avg) {
		d_average=0;
		lastk=j+i_num_samp_avg;
		for (k=j; k<lastk; k++) {
			d_average += d_signal[k];
		}
		if (d_average>d_max_average_amp) {
			d_max_average_amp=d_average;
		}
	}

	d_max_average_amp /= i_num_samp_avg;

	return d_max_average_amp;
}

/***************************************************************
 * name:        write_sfile
 * arguments:   string to write to
 *              values for writing to s-file
 * return:      0 on success 1 on failure
 * description: formats and writes numbers to an s-file line.
 *
 ***************************************************************/

int write_sfile(char *line, ChanHead *head, double amp_max, 
                double total_energy,double* slice_frac, int num_slice,
                double maxfreq)
{
	int j;
	char linebit[20];
	char chan3[4];

	/* Only room for 3 chars in chan for listing */
	/* SB at start means B */ 
	if (head->chan[0] == 'S' && head->chan[1] == 'B'){
		chan3[0] = head->chan[1];
		chan3[1] = head->chan[2];
		chan3[2] = head->chan[3];
	}
	else if (head->chan[3] == ' '){
		chan3[0] = head->chan[0];
		chan3[1] = head->chan[1];
		chan3[2] = head->chan[2];
	}
	else{
		chan3[0] = head->chan[0];
		chan3[1] = head->chan[1];
		chan3[2] = head->chan[3];
	}
	chan3[3] = '\0';

	sprintf(line," VOLC %s %s A%8.2e E%8.2e F ",head->sta,chan3,amp_max,total_energy);

	for (j=0; j<num_slice; j++){
		sprintf(linebit,"%-3.0f",slice_frac[j]);
		strcat(line,linebit);
	}
	sprintf(linebit,"%5.2f 3\n",maxfreq);
	strcat(line,linebit);

	return 0;
}


/**************************************************************************
 * name:        qsort_strcmp
 * arguments:   pointers to two srtings
 * return:      1,0.-1 as strcmp
 * description: comparison function for qsort - needed to get right pointers into strcmp
 *
 **************************************************************************/
int qsort_strcmp( const void *sp1, const void *sp2 )
{
    return( strcmp(*(char **)sp1, *(char **)sp2) );
}
