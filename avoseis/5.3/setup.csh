unsetenv AVOSEIS

source /opt/antelope/5.3/setup.csh

setenv AVOSEIS /opt/avoseis/5.3 

set a="$AVOSEIS/bin"

foreach i ( $path ) 
    if ( "$i" !~ /opt/avoseis/* ) then
        set a=($a $i)
    endif
end
set path=($a)

if ( $?MANPATH ) then
    set a=$AVOSEIS/man
    foreach i ( `echo $MANPATH | sed 's/:/ /g'` )
        if ( "$i" !~ /opt/avoseis/* ) then
            set a=($a $i)
        endif
    end
    setenv MANPATH `echo $a | sed 's/ /:/g' ` 
else
    setenv MANPATH $AVOSEIS/man:$ANTELOPE/man:$ANTELOPE/local/man:$ANTELOPE:h/../tcltk8.4.19/man:$ANTELOPE:h/../perl5.14.2/man
endif

if ( $?PFPATH ) then
    if ( $PFPATH !~ *$AVOSEIS* ) then
	set p=""
	set m="$ANTELOPE/data/pf"
	foreach i ( `echo $PFPATH | sed 's/:/ /g'` )
	    if ( "$i" !~ "$m" ) then
		set p=($p $i)
	    else 
		set p=($p $m "$AVOSEIS/data/pf")
	    endif
	end
	set p=($p)
	setenv PFPATH `echo $p | sed 's/ /:/g' `
    endif
else
    setenv PFPATH $ANTELOPE/data/pf:$ANTELOPE/local/data/pf:$AVOSEIS/data/pf:./pf:.
endif

if ( $?DATAPATH ) then
    if ( $DATAPATH !~ *$AVOSEIS* ) then
	set p=""
	set m="$ANTELOPE/data"
	foreach i ( `echo $DATAPATH | sed 's/:/ /g'` )
	    if ( "$i" !~ "$m" ) then
		set p=($p $i)
	    else
                set p=($p $m "$AVOSEIS/data")
	    endif
	end
	set p=($p)
	setenv DATAPATH `echo $p | sed 's/ /:/g' `
    endif
else
    setenv DATAPATH $ANTELOPE/data:$ANTELOPE/local/data:$AVOSEIS/data
endif

if ( $?SCHEMA_DIR ) then
    set a=$AVOSEIS/data/schemas
    foreach i ( `echo $SCHEMA_DIR | sed 's/:/ /g'` )
        if ( "$i" !~ /opt/avoseis/* ) then
            set a=($a $i)
        endif
    end
    setenv SCHEMA_DIR `echo $a | sed 's/ /:/g' ` 
else
    setenv SCHEMA_DIR $AVOSEIS/data/schemas:$ANTELOPE/data/schemas:$ANTELOPE/local/data/schemas
endif

unset a
unset m
unset p

setenv AVOSEISMAKE     $AVOSEIS/include/avoseismake

set avoseis=$AVOSEIS
set antelopemake=$ANTELOPEMAKE
