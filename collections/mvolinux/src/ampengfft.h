/*
*   header file for compilation with ampengfft.c
*
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* Prototypes for functions in ampengfft_utils.c */
int get_wavefilename_from_sfile(char* sfile, char** c_wavfilename, int* i_num);
int num_samp_fft (int i_num_samp);
int isDeadChannel(double d_samprate, int *i_signal, int i_num_samp);
int write_zeroes_for_dead_channel (char* c_line, ChanHead* head, int i_num_slice);
int prepare_for_fft(double* d_time, double* d_fft, int i_nb);
int amplitude_spectrum(double* d_fft, double* d_spectrum_io, int i_nb_samp_fft);
int slice_energy(int* i_slice, int i_num_slice, double* d_spec, double* d_slice_frac, double lowCut, double* d_maxfreq);
int create_filter(double* d_filter, double d_freq_interv, int i_num_samp_fft);
int filter_fft_and_inversefft(double *d_fft, double* d_filt, int i_num_samp_fft, 
                              double *d_signal, int i_num_samp_event);
double get_energy(double* d_signal, int n);
int	abs_signal(double* d_signal, int n);
double max_average_amplitude(double* d_signal, int i_num_samp_event, 
                             double d_samprate,int i_amp_avg_window);
int write_sfile(char* c_line, ChanHead* head, double d_amp_max, 
                double d_total_energy,double* d_slice_frac, 
                int i_num_slice,double d_maxfreq);
int qsort_strcmp( const void *sp1, const void *sp2 );
