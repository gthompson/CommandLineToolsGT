
/* Functions from Numerical Recipies in C second edition. */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <dirent.h>

#include "matrix.h"

#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
#define PI 3.141592
#define frac 0.1

char **cmatrix(int nrl,int nrh,int ncl,int nch)
{
	int i;
	char **m;

	m=(char **) malloc((unsigned) (nrh-nrl+1)*sizeof(char*));
	m -= nrl;

	for(i=nrl;i<=nrh;i++) {
		m[i]=(char *) malloc((unsigned) (nch-ncl+1)*sizeof(char));
		m[i] -= ncl;
	}
	return m;
}
/*			date2samp.c					*/
/* Function to convert date to number of samples since 1990		 */


double date2samp(int samp,int year,int month,int day,int hour,int min,double sec)
{

	int monlen[] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };

	double nsamp;
	int i;

	/* year 2000 fudge */
	if (year < 90) year += 100;
	
	for(i=90; i<year; i++){
		if ((double)i/4 == (int)(i/4)) day += 366;
		else day += 365;
	}

	for (i=1;i<month;i++) day += monlen[i];

	/* leap year check */
	if ((double)year/4 == (int)(year/4) && month > 2) day++;


	hour +=  day*24;
	min  += hour*60;
	sec  +=  min*60;
	nsamp =  sec*samp;
	
	return nsamp;
}

double **dmatrix(int nrl,int nrh,int ncl,int nch)
{
	int i;
	double **m;

	m=(double **) malloc((unsigned) (nrh-nrl+1)*sizeof(double*));
	if (!m) return 0;
	m -= nrl;

	for(i=nrl;i<=nrh;i++) {
		m[i]=(double *) malloc((unsigned) (nch-ncl+1)*sizeof(double));
		if (!m[i]) return 0;
		m[i] -= ncl;
	}
	return m;
}
double *dvector(int nl, int nh)
{
	double *v;

	v=(double *)malloc((unsigned) (nh-nl+1)*sizeof(double));
	if (!v) return 0;
	return v-nl;
}
/*			fft.c							*/
/* Function that uses Numerical Recipes four1 to calculate the amplitude
   spectrum of a[1,n]. n must be multiple of 2. 				*/

void fft(double *a,double *f,int n)
{

	int i;
	double *c;

	c = dvector(1,2*n);

	/* Every other vaue made zero (Imaginary part.)*/
	for(i=1; i<=n; i++){
		c[i*2-1] = a[i];
		c[i*2] = 0;
	}

	/* Take FFT. The result is 2n values (n real,imaginary pairs) ranging
	   from 0 to Nyquist then from -Nyquist and back to zero.	*/
	four1(c,n,1);

	/* Calculate amplitude. */
	for(i=1; i<=n; i++) f[i] = sqrt(c[i*2-1]*c[i*2-1]+c[i*2]*c[i*2]);

	free_dvector(c,1,2*n);

}

void four1(double *data,int nn,int isign)
{
	int n,mmax,m,j,istep,i;
	double wtemp,wr,wpr,wpi,wi,theta;
	double tempr,tempi;

	n=nn << 1;
	j=1;
	for (i=1;i<n;i+=2) {
		if (j > i) {
			SWAP(data[j],data[i]);
			SWAP(data[j+1],data[i+1]);
		}
		m=n >> 1;
		while (m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	mmax=2;
	while (n > mmax) {
		istep=2*mmax;
		theta=6.28318530717959/(isign*mmax);
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=n;i+=istep) {
				j=i+mmax;
				tempr=wr*data[j]-wi*data[j+1];
				tempi=wr*data[j+1]+wi*data[j];
				data[j]=data[i]-tempr;
				data[j+1]=data[i+1]-tempi;
				data[i] += tempr;
				data[i+1] += tempi;
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}
}

void free_cmatrix(char **m,int nrl,int nrh,int ncl,int nch)
{
	int i;

	for(i=nrh;i>=nrl;i--) free((char*) (m[i]+ncl));
	free((char*) (m+nrl));
}
void free_dmatrix(double **m,int nrl,int nrh,int ncl,int nch)
{
	int i;

	for(i=nrh;i>=nrl;i--) free((char*) (m[i]+ncl));
	free((char*) (m+nrl));
}
void free_dvector(double *v,int nl,int nh)
{
	free((char*) (v+nl));
}
void free_imatrix(int **m,int nrl,int nrh,int ncl,int nch)
{
	int i;

	for(i=nrh;i>=nrl;i--) free((char*) (m[i]+ncl));
	free((char*) (m+nrl));
}
void free_ivector(int *v,int nl,int nh)
{
	free((char*) (v+nl));
}
void free_matrix(float **m,int nrl,int nrh,int ncl,int nch)
{
	int i;

	for(i=nrh;i>=nrl;i--) free((char*) (m[i]+ncl));
	free((char*) (m+nrl));
}
void free_vector(float *v,int nl,int nh)
{
	free((char*) (v+nl));
}

int **imatrix(int nrl,int nrh,int ncl,int nch)
{
	int i;
	int **m;

	m=(int **) malloc((unsigned) (nrh-nrl+1)*sizeof(int*));
	if (!m) return 0;
	m -= nrl;

	for(i=nrl;i<=nrh;i++) {
		m[i]=(int *) malloc((unsigned) (nch-ncl+1)*sizeof(int));
		if (!m[i]) return 0;
		m[i] -= ncl;
	}
	return m;
}
int *ivector(int nl,int nh)
{
	int *v;

	v=(int *)malloc((unsigned) (nh-nl+1)*sizeof(int));
	if (!v) return 0;
	return v-nl;
}

float **matrix(int nrl,int nrh,int ncl,int nch)
{
	int i;
	float **m;

	m=(float **) malloc((unsigned) (nrh-nrl+1)*sizeof(float*));
	m -= nrl;

	for(i=nrl;i<=nrh;i++) {
		m[i]=(float *) malloc((unsigned) (nch-ncl+1)*sizeof(float));
		m[i] -= ncl;
	}
	return m;
}


int remoff(double *a,int n)
{
	double sum;
	int i;

	sum = 0;
	for (i=1;i<=n;i++) sum += a[i];
	sum /= n;

	for (i=1;i<=n;i++){
		a[i] -= sum;
	}

	return 0;
}

/* sort.c */

void sort(double *a,int bot, int top)
{
	int i, last;

	swap(&a[bot],&a[(top+bot)/2]);

	last=bot;
	for (i=bot+1;i<=top;i++){
		if(a[i]<a[bot]){
			swap(&a[last+1],&a[i]);
			last++;
		}
	}
	swap(&a[bot],&a[last]);
	sort(a,bot,last-1);
	sort(a,last+1,top);
}
/* swap.c */

void swap(double *a, double *b){

	double temp;

	temp=*a;
	*a=*b;
	*b=temp;
}

/* c_sort - sort stings */

void c_sort(char *a[],int bot, int top)
{
	int i, last;
printf("in sort\n");
	c_swap(a,bot,(top+bot)/2);
printf("first swap\n");
	last=bot;
	for (i=bot+1; i<=top; i++){
		if(strcmp(a[i],a[bot])<0)
			c_swap(a,++last,i);
	}
	c_swap(a,bot,last);
printf("last swap\n");
	c_sort(a,bot,last-1);
	c_sort(a,last+1,top);
}
/* c_swap */

void c_swap(char *a[], int i, int j){

	char *temp;
	
	temp=a[i];
	a[i]=a[j];
	a[j]=temp;
}

/* Function to taper traces held in rows of rectangular matrix. */
void taper(float **a,int n,int m)
{
	int i,j;

	for(i=1; i<=n; i++){
		for(j=1; j<=m; j++){
			a[i][j] *= 0.5*(1.0 - cos(2*PI*j/m));

		}
	}
}
/* Function to taper one trace */


void taper1(double *a,int n)
{
	int i;

	for(i=1; i<=n*frac; i++){
		a[i] *= 0.5*(1.0 - cos(2*PI*i/(n*frac)));

	}

	for(i=n-n*frac; i<=n; i++){
		a[i] *= 0.5*(1.0 - cos(2*PI*i/(n*frac)));

	}

}
float *vector(int nl,int nh)
{
	float *v;

	v=(float *)malloc((unsigned) (nh-nl+1)*sizeof(float));
	return v-nl;
}
char *itoa(int n){
   int i, sign;
   static char s[100];

   if((sign = n)<0)
      n=-n;
   i=0;
   do{
      s[i++]=n%10+'0';
   }while((n/=10)>0);
   if(sign<0)
      s[i++] = '-';
   s[i]='\0';
   reverse(s);
   return s;
}
void reverse(char s[])
{
   int c,i,j;

   for(i=0,j=strlen(s)-1;i<j;i++,j--){
      c=s[i];
      s[i]=s[j];
      s[j]=c;
   }
}
#undef SWAP

