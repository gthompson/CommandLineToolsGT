#  description:
#	makefile for ampengfft and associated perl scripts
#
#  synopsis:
#		make test	     # make executable in {TEST_HOST}  [DEFAULT]
#		make install 	 # make executable in /live
#

#PROD_HOST = /live
MVOLINUX  = /raid/apps/mvolinux
PROD_HOST = ${MVOLINUX}/bin
TEST_HOST = ${MVOLINUX}/testbin

SEILIB   = ${MVOLINUX}/lib/seisan_lib
EARTHLIB = ${MVOLINUX}/lib/earthworm_lib

COMPILER = gcc
CCFLAGS = -g -o

BINARIES = ampengfft_utils.o matrix.o ${SEILIB}/read_seisan.o ${EARTHLIB}/kom.o
INCLUDES = ampengfft.h matrix.h ${SEILIB}/seisan.h 

test:	${TEST_HOST}/ampengfft        \
	${TEST_HOST}/ampengfft_time   \
	plot_sfile               \
	countrock                \
	compile_slice            \
	plot_slice                \
	remove_slice             \
	plot_cumul               \
	${TEST_HOST}/sfile2wavfile \
	${TEST_HOST}/wavfile2eventmetrics

install: ${PROD_HOST}/ampengfft       \
	 ${PROD_HOST}/ampengfft_time  \
	 ${PROD_HOST}/plot_sfile      \
  	 ${PROD_HOST}/compile_slice   \
	 ${PROD_HOST}/countrock       \
 	 ${PROD_HOST}/plot_slice      \
 	 ${PROD_HOST}/remove_slice    \
 	 ${PROD_HOST}/plot_cumul      \
 	 ${PROD_HOST}/sfile2wavfile   \	
	 ${PROD_HOST}/wavfile2eventmetrics

${TEST_HOST}/ampengfft: ampengfft.o ${BINARIES} ${INCLUDES}
	${COMPILER} ${CCFLAGS} $@ ampengfft.o ${BINARIES} -lm

${PROD_HOST}/ampengfft: ampengfft.o ${BINARIES} ${INCLUDES}
	${COMPILER} ${CCFLAGS} $@ ampengfft.o ${BINARIES} -lm

${TEST_HOST}/sfile2wavfile: sfile2wavfile.o ${BINARIES} ${INCLUDES}
	${COMPILER} ${CCFLAGS} $@ sfile2wavfile.o ${BINARIES} -lm

${PROD_HOST}/sfile2wavfile: sfile2wavfile.o ${BINARIES} ${INCLUDES}
	${COMPILER} ${CCFLAGS} $@ sfile2wavfile.o ${BINARIES} -lm

${TEST_HOST}/wavfile2eventmetrics: wavfile2eventmetrics.o ${BINARIES} ${INCLUDES}
	${COMPILER} ${CCFLAGS} $@ wavfile2eventmetrics.o ${BINARIES} -lm

${PROD_HOST}/wavfile2eventmetrics: wavfile2eventmetrics.o ${BINARIES} ${INCLUDES}
	${COMPILER} ${CCFLAGS} $@ wavfile2eventmetrics.o ${BINARIES} -lm

${TEST_HOST}/ampengfft_time: ampengfft_time.o ${BINARIES} ${INCLUDES}
	${COMPILER} ${CCFLAGS} $@ ampengfft_time.o ${BINARIES} -lm

${PROD_HOST}/ampengfft_time: ampengfft_time.o ${BINARIES} ${INCLUDES}
	${COMPILER} ${CCFLAGS} $@ ampengfft_time.o ${BINARIES} -lm

plot_sfile: plot_sfile.pl
	/usr/bin/perl -c  plot_sfile.pl

${PROD_HOST}/plot_sfile: plot_sfile.pl
	cp plot_sfile.pl ${PROD_HOST}/plot_sfile; chmod 775 ${PROD_HOST}/plot_sfile

countrock: countrock.pl
	/usr/bin/perl -c  countrock.pl

${PROD_HOST}/countrock: countrock.pl
	cp countrock.pl ${PROD_HOST}/countrock; chmod 775 ${PROD_HOST}/countrock

compile_slice: compile_slice.pl
	/usr/bin/perl -c  compile_slice.pl

${PROD_HOST}/compile_slice: compile_slice.pl
	cp compile_slice.pl ${PROD_HOST}/compile_slice; chmod 775 ${PROD_HOST}/compile_slice

plot_slice: plot_slice.pl
	/usr/bin/perl -c  plot_slice.pl

${PROD_HOST}/plot_slice: plot_slice.pl
	cp plot_slice.pl ${PROD_HOST}/plot_slice; chmod 775 ${PROD_HOST}/plot_slice

remove_slice: remove_slice.pl
	/usr/bin/perl -c  remove_slice.pl

${PROD_HOST}/remove_slice: remove_slice.pl
	cp remove_slice.pl ${PROD_HOST}/remove_slice; chmod 775 ${PROD_HOST}/remove_slice

plot_cumul: plot_cumul.pl
	/usr/bin/perl -c  plot_cumul.pl

${PROD_HOST}/plot_cumul: plot_cumul.pl
	cp plot_cumul.pl ${PROD_HOST}/plot_cumul; chmod 775 ${PROD_HOST}/plot_cumul


