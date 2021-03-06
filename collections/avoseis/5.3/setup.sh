unset AVOSEIS

. /opt/antelope/5.3/setup.sh

AVOSEIS=/opt/avoseis/5.3; export AVOSEIS

a="$AVOSEIS/bin"

# Set up the PATH and MANPATH environment variables.
# If some version of the local trees is already in the path, remove it.
for i in `echo $PATH | sed 's/:/ /g'`; do
  case $i in
    /opt/avoseis/*)        ;;
    *)                 a="$a:$i" ;;
  esac
done
PATH=$a ; export PATH
unset a

if [ "x$MANPATH" != x ] ; then
  a=$AVOSEIS/man
  for i in `echo $MANPATH | sed 's/:/ /g'`; do
    case $i in
      /opt/avoseis/*)  ;;
      *)                a="$a:$i"
    esac
  done
  MANPATH=$a ; export MANPATH
  unset a
else
  MANPATH="$AVOSEIS/man:$ANTELOPE/man:$ANTELOPE/local/man:$ANTELOPE/../tcltk8.4.19/man:$ANTELOPE/../perl5.14.2/man" ; export MANPATH
fi

if [ "x$PFPATH" != x ] ; then
  case $PFPATH in
    *\$AVOSEIS*)      ;;
    *)            
    p=""
    m="$ANTELOPE/data/pf"
    for i in `echo $PFPATH | sed 's/:/ /g'`; do
      if [ "$i" != "$m" ]; then
        p="$p:$i"
      else
        p="$p:$m:$AVOSEIS/data/pf"
      fi
    done
    PFPATH=$p; export PFPATH
    ;;
  esac
else
  PFPATH="$ANTELOPE/data/pf:$ANTELOPE/local/data/pf:$AVOSEIS/data/pf:./pf:." ; export PFPATH
fi

if [ "x$DATAPATH" != x ] ; then
  case $DATAPATH in
    *\$AVOSEIS*)      ;;
    *)            
    p=""
    m="$ANTELOPE/data"
    for i in `echo $DATAPATH | sed 's/:/ /g'`; do
      if [ "$i" != "$m" ] ; then
        p="$p:$i"
      else
        p="$p:$m:$AVOSEIS/data"
      fi
    done
    DATAPATH=$p; export DATAPATH
    ;;
  esac
else
  DATAPATH="$ANTELOPE/data:$ANTELOPE/local/data:$AVOSEIS/data" ; export DATAPATH
fi

if [ "x$SCHEMA_DIR" != x ] ; then
  a=$AVOSEIS/data/schemas
  for i in `echo $SCHEMA_DIR | sed 's/:/ /g'`; do
    case $i in
      /opt/avoseis/*)  ;;
      *)                a="$a:$i"
    esac
  done
  SCHEMA_DIR=$a ; export SCHEMA_DIR
  unset a
else
  SCHEMA_DIR="$AVOSEIS/data/schemas:$ANTELOPE/data/schemas:$ANTELOPE/local/data/schemas" ; export SCHEMA_DIR
fi

unset a
unset m
unset p

AVOSEISMAKE="$AVOSEIS/include/avoseismake" ; export AVOSEISMAKE
