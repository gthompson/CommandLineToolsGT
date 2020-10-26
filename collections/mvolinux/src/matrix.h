
/* Prototypes for functions in matrix.c */

char **cmatrix(int nrl,int nrh,int ncl,int nch);
double date2samp(int samp,int year,int month,int day,int hour,int min,double sec);
double **dmatrix(int nrl,int nrh,int ncl,int nch);
double *dvector(int nl, int nh);
void fft(double *a,double *f,int n);
void four1(double *data,int nn,int isign);
void free_cmatrix(char **m,int nrl,int nrh,int ncl,int nch);
void free_dmatrix(double **m,int nrl,int nrh,int ncl,int nch);
void free_dvector(double *v,int nl,int nh);
void free_imatrix(int **m,int nrl,int nrh,int ncl,int nch);
void free_ivector(int *v,int nl,int nh);
void free_matrix(float **m,int nrl,int nrh,int ncl,int nch);
void free_vector(float *v,int nl,int nh);
int getdata(int nchan,int samp,double start,int n,int **a,char *bufdir);
int getnchan(char *bufdir,int *nchan,int *samp);
int **imatrix(int nrl,int nrh,int ncl,int nch);
int *ivector(int nl,int nh);
int lcount(char *filename);
float **matrix(int nrl,int nrh,int ncl,int nch);
int rdos9(char *infile,int skip,int m,int **a);
int rdsei(char *infile,int n_traces,int n_dat,int **data,double *gain);
int rdseiall(char *filename,int **data,int *n_traces,int *n_dat,double *samprate);
int rdseihead(char *infile,int *n_traces,int *n,double *samprate);
int remoff(double *a,int n);
void taper(float **a,int n,int m);
void taper1(double *a,int n);
float *vector(int nl,int nh);
int demux(char *infile,int **data,int *year,int *mnth,int *day,int *hour,int *min,double *sec);
void swap(double *a, double *b);
void sort(double *a,int bot,int top);
void c_sort(char *a[],int bot, int top);
void c_swap(char *a[],int i,int j);
char *itoa(int n);
void reverse(char s[]);

