#!/bin/bash
for dir in AEF REA WAV
do
	for yyyy in 1996 1997 1998 1999 2000 2001 2002 2003 2004 2005 2006 2007
	do
		for mm in 01 02 03 04 05 06 07 08 09 10 11 12
		do
			source="/media/data/Montserrat/MontserratSeismicMastering/seisan/$dir/MVOE_/$yyyy/$mm" 
			if [[  -e $source ]]; then
				target="/media/sdd1/seismo/$dir/MVOE_/$yyyy/"
				if [[ ! -e $target ]]; then
				    mkdir -p $dir
				fi
				echo "rsync -av $source $target"
				rsync -av $source $target
			fi
		done
	done
done

