#
# cross compile FUPPES for Netgear ReadyNas
# Copyright (C) 2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
#

# get the compiler from:
# http://www.readynas.com/download/development/readynas-cross-3.3.5.tar.gz


# settings

# the build host
HOST="sparc-linux"

# target directory
PREFIX=`pwd`"/readynas"

# make command (e.g. "make -j 2")
MAKE="make -j 2"
MAKE_INSTALL="make install"

# you should not need to change anything below

# exports
export CFLAGS="-I$PREFIX/include $CFLAGS"
export CPPFLAGS="-I$PREFIX/include $CPPFLAGS"
export LDFLAGS="-L$PREFIX/lib"
export PKG_CONFIG_LIBDIR="$PREFIX/lib/pkgconfig"

FUPPES_DIR=`pwd`


# create target directory
if ! test -d $PREFIX; then
  #echo "enter root password for target directory creation"
  #su -c "mkdir $PREFIX && chmod 777 $PREFIX"
  mkdir $PREFIX
fi

# create source directory
if ! test -d $PREFIX"/src"; then
  mkdir $PREFIX"/src"
fi
cd $PREFIX"/src"/


# $1 = paket name, $2 = file ext, $3 = url
function loadpkt {

  if ! test -d ../downloads; then
    mkdir ../downloads
  fi

  SRC_PKT=$1$2
  if ! test -e ../downloads/$SRC_PKT; then
    wget --directory-prefix=../downloads/ $3$SRC_PKT
  fi

  case $2 in
    ".tar.gz")
      tar -xvzf ../downloads/$1$2
    ;;
    ".tar.bz2")
      tar -xvjf ../downloads/$1$2
    ;;
    ".zip")
      unzip -o ../downloads/$1$2
    ;;
  esac

  cd $1
}



HAVE_ZLIB="yes"
HAVE_UUID="no"
HAVE_ICONV="yes"
HAVE_TAGLIB="yes"
HAVE_EXIV2="no"

HAVE_JPEG="yes"
HAVE_PNG="yes"

HAVE_SIMAGE="no"
HAVE_IMAGEMAGICK="yes"

HAVE_FFMPEG="yes"
HAVE_FFMPEGTHUMBNAILER="yes"



# zlib 1.2.3 (for taglib, exiv2, imagemagick & co.) 
# strongly recommended

if test "$HAVE_ZLIB" == "yes"; then

echo "start building zlib"

loadpkt "zlib-1.2.3" ".tar.gz" \
          "http://prdownloads.sourceforge.net/libpng/"

echo "AC_INIT([zlib], [1.2.3], [fuppes@ulrich-voelkel.de])
AM_INIT_AUTOMAKE([foreign])

AC_PROG_CC
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AM_PROG_INSTALL_STRIP

AC_OUTPUT([Makefile])" \
> configure.ac

echo "include_HEADERS = zlib.h zconf.h  

lib_LTLIBRARIES = libz.la

libz_la_SOURCES = \
  adler32.h adler32.c \
  compress.h compress.c \
  crc32.h crc32.c \
  gzio.h gzio.c \
  uncompr.h uncompr.c \
  deflate.h deflate.c \
  trees.h trees.c \
  zutil.h zutil.c \
  inflate.h inflate.c \
  infback.h infback.c \
  inftrees.h inftrees.c \
  inffast.h inffast.c

libz_la_LDFLAGS = -no-undefined" \
> Makefile.am


# zlib 1.2.4 currently (31. March 2010) does not work with ffmpeg

#loadpkt "zlib-1.2.4" ".tar.gz" \
#        "http://www.zlib.net/"

#echo "AC_INIT([zlib], [1.2.4], [fuppes@ulrich-voelkel.de])
#AM_INIT_AUTOMAKE([foreign])

#AC_PROG_CC
#AC_LIBTOOL_WIN32_DLL
#AC_PROG_LIBTOOL
#AC_PROG_INSTALL
#AM_PROG_INSTALL_STRIP

#AC_OUTPUT([Makefile])" \
#> configure.ac

#echo "include_HEADERS = zlib.h zconf.h  

#lib_LTLIBRARIES = libz.la

#libz_la_SOURCES = \
#  adler32.c \
#  compress.c \
#  crc32.h crc32.c \
#  deflate.h deflate.c \
#  gzclose.c \
#  gzlib.c \
#  gzread.c \
#  gzwrite.c \
#  infback.c \
#  inffast.h inffast.c \
#  inflate.h inflate.c \
#  inftrees.h inftrees.c \
#  trees.h trees.c \
#  uncompr.c \
#  zutil.h zutil.c

#libz_la_LDFLAGS = -no-undefined" \
#> Makefile.am

autoreconf -vfi
./configure --host=$HOST --prefix=$PREFIX
$MAKE
$MAKE_INSTALL
cd ..

else
  echo "skipped zlib"
fi


if test "$HAVE_UUID" == "yes"; then

  loadpkt "e2fsprogs-1.41.11" ".tar.gz" \
          "http://prdownloads.sourceforge.net/e2fsprogs/"
        
  ./configure --host=$HOST --prefix=$PREFIX \
  --disable-testio-debug  --disable-imager \
  --disable-resizer --disable-tls --disable-uuidd --disable-nls --disable-rpath
  $MAKE
  cd lib/uuid/
  $MAKE_INSTALL
  cd ../..

else
  echo "skipped libuuid (e2fsprogs)"
fi


# libiconv 1.13 (recommended)
if test "$HAVE_ICONV" == "yes"; then

  echo "start building libiconv"

  loadpkt "libiconv-1.13" ".tar.gz" \
          "http://ftp.gnu.org/pub/gnu/libiconv/"
  ./configure --host=$HOST --prefix=$PREFIX
  $MAKE
  $MAKE_INSTALL
  cd ..

else
  echo "skipped libiconv"
fi



# PCRE 7.9 (required)
echo "start building pcre"
loadpkt "pcre-7.9" ".zip" \
        "ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/"
./configure --host=$HOST --prefix=$PREFIX \
--enable-utf8 --enable-newline-is-anycrlf --disable-cpp
$MAKE
$MAKE_INSTALL
cd ..


# SQLite 3.6.23 (required)
echo "start building sqlite"
loadpkt "sqlite-amalgamation-3.6.23.1" ".tar.gz" \
        "http://www.sqlite.org/"
cd sqlite-3.6.23.1
./configure --host=$HOST --prefix=$PREFIX \
--enable-tempstore=yes --enable-threadsafe
$MAKE
$MAKE_INSTALL
cd ..


# libxml2 2.6.32 (required)
echo "start building libxml2"
loadpkt "libxml2-2.6.32" ".tar.gz" \
        "ftp://xmlsoft.org/libxml2/old/"

#loadpkt "libxml2-2.7.6" ".tar.gz" \
#        "ftp://xmlsoft.org/libxml2/"

./configure --host=$HOST --prefix=$PREFIX \
--enable-ipv6=no --with-debug=no --with-http=no \
--with-ftp=no --with-docbook=no --with-schemas=yes --with-schematron=no \
--with-catalog=no --with-html=no --with-legacy=no --with-pattern=no \
--with-push=yes --with-python=no --with-readline=no --with-regexps=no \
--with-sax1=no --with-xinclude=no --with-xpath=no --with-xptr=no --with-modules=no \
--with-valid=no --with-reader=yes --with-writer=yes
$MAKE
$MAKE_INSTALL
cd ..





#
# OPTIONAL
#

# taglib
if test "$HAVE_TAGLIB" == "yes"; then

  echo "start building taglib"

  loadpkt "taglib-1.6.1" ".tar.gz" \
          "http://developer.kde.org/~wheeler/files/src/"
  CXXFLAGS="$CXXFLAGS -DMAKE_TAGLIB_LIB" \
  LDFLAGS="$LDFLAGS" \
  ./configure --host=$HOST --prefix=$PREFIX
  $MAKE
  $MAKE_INSTALL
  cd ..
  
else
  echo "skipped taglib"
fi


# exiv2
if test "$HAVE_EXIV2" == "yes"; then

  echo "start building exiv2"
  loadpkt "exiv2-0.18.2" ".tar.gz" \
          "http://www.exiv2.org/"

  PARAMS="--disable-xmp --disable-visibility"
  if test "$HAVE_ZLIB" != "yes"; then
    PARAMS="$PARAMS --without-zlib"
  fi
  
  ./configure --host=$HOST --prefix=$PREFIX $PARAMS

  sed -i -e 's/LDFLAGS = /LDFLAGS = -no-undefined /' config/config.mk
  sed -i -e 's/-lm//' config/config.mk
  $MAKE
  mv src/.libs/exiv2 src/.libs/exiv2.exe
  $MAKE_INSTALL
  cd ..

else
  echo "skipped exiv2"
fi

# todo libtiff & libungif


#libjpeg (for ImageMagick, simage and ffmpegthumbnailer)
if test "$HAVE_JPEG" == "yes"; then
  echo "start building jpeg"  
  loadpkt "jpegsrc.v7" ".tar.gz" \
          "http://www.ijg.org/files/"
  cd jpeg-7
  ./configure --host=$HOST --prefix=$PREFIX
  $MAKE
  $MAKE_INSTALL
  cd ..

else
  echo "skipped libjpeg"
fi

#libpng (for ImageMagick, simage and ffmpegthumbnailer)
if test "$HAVE_PNG" == "yes"; then
  echo "start building png"
  loadpkt "libpng-1.4.1" ".tar.bz2" \
          "http://prdownloads.sourceforge.net/libpng/"
  ./configure --host=$HOST --prefix=$PREFIX
  $MAKE
  $MAKE_INSTALL
  cd ..

else
  echo "skipped libpng"
fi


#simage
if test "$HAVE_SIMAGE" == "yes"; then

  echo "start building simage"
  loadpkt "simage-1.6.1" ".tar.gz" \
          "http://ftp.coin3d.org/coin/src/all/"

  ./configure --host=$HOST --prefix=$PREFIX
  $MAKE
  $MAKE_INSTALL
  cd ..

else
  echo "skipped simage"
fi


# ImageMagick
if test "$HAVE_IMAGEMAGICK" == "yes"; then

  echo "start building ImageMagick"
  loadpkt "ImageMagick-6.6.0-9" ".tar.gz" \
          "ftp://ftp.imagemagick.org/pub/ImageMagick/"


  ./configure --host=$HOST --prefix=$PREFIX \
  --disable-deprecated --without-perl --with-modules \
  --without-x --without-gslib --with-magick-plus-plus=no \
  --with-quantum-depth=8 --enable-embeddable
  #--disable-installed 
  
  #sed -i -e 's/GetEnvironmentValue("MAGICK_CODER_MODULE_PATH")/ConstantString(".\\\\magick-modules")/' magick/module.c
  #sed -i -e 's/GetEnvironmentValue("MAGICK_CODER_FILTER_PATH")/ConstantString(".\\\\magick-filters")/' magick/module.c
  
  
  $MAKE
  $MAKE_INSTALL
  cd ..

else
  echo "skipped ImageMagick"
fi


# So. 24 Jan :: rev 21414

if test "$HAVE_FFMPEG" == "yes"; then

echo "start building ffmpeg"

if ! test -d ffmpeg-svn; then
  svn co svn://svn.mplayerhq.hu/ffmpeg/trunk ffmpeg-svn
  cd ffmpeg-svn
else
  cd ffmpeg-svn
  svn update
fi

#sed -i -e 's/__MINGW32_MINOR_VERSION >= 15/__MINGW32_MINOR_VERSION >= 13/' configure
#sed -i -e 's/usleep/Sleep/' ffmpeg.c
 
./configure --prefix=$PREFIX --enable-cross-compile --target-os=linux --arch=sparc \
--cross-prefix=$HOST- --enable-shared \
--enable-gpl --disable-ffmpeg --disable-ffplay --disable-ffserver \
--disable-demuxer=dv1394 --disable-indevs \
--disable-decoder=flv --disable-encoder=flv --disable-demuxer=flv \
--disable-muxer=flv \
--as=/usr/bin/sparc-linux-as


#./configure --prefix=$PREFIX --enable-cross-compile  \
# --cross-prefix=$HOST- --enable-shared \
#--disable-static --disable-ffmpeg --disable-ffplay --disable-ffserver \
#--disable-demuxer=dv1394 --disable-indevs --arch=sparc  \
#--disable-decoder=flv --disable-encoder=flv --disable-demuxer=flv \
#--disable-muxer=flv



$MAKE
$MAKE_INSTALL
cd ..

else
  echo "skipped ffmpeg"
fi


if test "$HAVE_FFMPEGTHUMBNAILER" == "yes"; then

  echo "start building ffmpegthumbnailer"
  loadpkt "ffmpegthumbnailer-2.0.0" ".tar.gz" \
          "http://ffmpegthumbnailer.googlecode.com/files/"

  ./configure --host=$HOST --prefix=$PREFIX 
  $MAKE
  $MAKE_INSTALL
  cd ..

else
  echo "skipped ffmpegthumbnailer"
fi





# FUPPES
cd $FUPPES_DIR


#CPPFLAGS="$CPPFLAGS -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE=1" \
LIBS="$LIBS -ldl -lz" \
./configure --host=$HOST --prefix=$PREFIX --sysconfdir=$PREFIX/etc \
--enable-force-inotify=yes --disable-taglib

sed -i -e 's/FUPPES_DATADIR=\\"$(datadir)\/fuppes\\"/FUPPES_DATADIR=\\"\/opt\/fuppes\/data\\"/' src/Makefile
sed -i -e 's/FUPPES_PLUGINDIR=\\"$(libdir)\/fuppes\\"/FUPPES_PLUGINDIR=\\"\/opt\/fuppes\/plugins\\"/' src/Makefile
sed -i -e 's/FUPPES_SYSCONFDIR=\\"$(sysconfdir)\/fuppes\\"/FUPPES_SYSCONFDIR=\\"\/opt\/fuppes\/etc\\"/' src/Makefile

$MAKE
$MAKE_INSTALL


# strip libraries and executables
if test "$DO_STRIP" == "yes"; then
  $HOST-strip -s $PREFIX/bin/*
  $HOST-strip -s $PREFIX/lib/*
  $HOST-strip -s $PREFIX/lib/fuppes/*
  touch $PREFIX/bin/*
  touch $PREFIX/lib/fuppes/*
  touch $PREFIX/share/fuppes/*
fi
