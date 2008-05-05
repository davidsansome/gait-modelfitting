#!/bin/sh

fixlibs() {
	echo $1
	install_name_tool -id `basename $1` $1
	finklibs=`otool -L $1 | grep '/opt/local/lib' | grep -v $1 | cut -d ' ' -f 1 | sed -e 's#^ *##'`
	for i in $finklibs
	do
		cp $i $FRMDIR
		finklib=`basename $i`
		echo $finklib
		install_name_tool -change $i @executable_path/../Frameworks/${finklib} $1
		echo $i | grep -q `basename $1` || fixlibs ${FRMDIR}/$finklib
	done
}

APPNAME="Modelfitting"
EXENAME="modelfitting"
EXEDIR="$APPNAME.app/Contents/MacOS"
RESDIR="$APPNAME.app/Contents/Resources"
FRMDIR="$APPNAME.app/Contents/Frameworks"
PLUGDIR="$APPNAME.app/Contents/plugins"

mkdir -p $EXEDIR
mkdir $RESDIR
mkdir $FRMDIR
mkdir $PLUGDIR
cp ../bin/$EXENAME $EXEDIR/$APPNAME
cp Info.plist $APPNAME.app/Contents
cp qt.conf ${RESDIR}

# Copy in Qt frameworks
cp -R /Library/Frameworks/QtCore.framework $FRMDIR
rm -rf ${FRMDIR}/QtCore.framework/Headers/
install_name_tool -id @executable_path/../Frameworks/QtCore.Framework/Versions/4.0/QtCore ${FRMDIR}/QtCore.framework/Versions/4/QtCore
cp -R /Library/Frameworks/QtGui.framework $FRMDIR
rm -rf ${FRMDIR}/QtGui.framework/Headers/
install_name_tool -id @executable_path/../Frameworks/QtGui.Framework/Versions/4.0/QtGui ${FRMDIR}/QtGui.framework/Versions/4/QtGui
cp -R /Library/Frameworks/QtOpenGL.framework $FRMDIR
rm -rf ${FRMDIR}/QtOpenGL.framework/Headers/
install_name_tool -id @executable_path/../Frameworks/QtOpenGL.Framework/Versions/4.0/QtOpenGL ${FRMDIR}/QtOpenGL.framework/Versions/4/QtOpenGL
cp -R /Library/Frameworks/QtSvg.framework $FRMDIR
rm -rf ${FRMDIR}/QtSvg.framework/Headers/
install_name_tool -id @executable_path/../Frameworks/QtSvg.Framework/Versions/4.0/QtSvg ${FRMDIR}/QtSvg.framework/Versions/4/QtSvg
cp -R /Library/Frameworks/QtXml.framework $FRMDIR
rm -rf ${FRMDIR}/QtXml.framework/Headers/
install_name_tool -id @executable_path/../Frameworks/QtXml.Framework/Versions/4.0/QtXml ${FRMDIR}/QtXml.framework/Versions/4/QtXml
cp -R /Library/Frameworks/QtNetwork.framework $FRMDIR
rm -rf ${FRMDIR}/QtNetwork.framework/Headers/
install_name_tool -id @executable_path/../Frameworks/QtNetwork.Framework/Versions/4.0/QtNetwork ${FRMDIR}/QtNetwork.framework/Versions/4/QtNetwork
cp -R /Library/Frameworks/QtDBus.framework $FRMDIR
rm -rf ${FRMDIR}/QtDBus.framework/Headers/
install_name_tool -id @executable_path/../Frameworks/QtDBus.Framework/Versions/4.0/QtDBus ${FRMDIR}/QtDBus.framework/Versions/4/QtDBus
cp -R /Library/Frameworks/phonon.framework $FRMDIR
rm -rf ${FRMDIR}/phonon.framework/Headers/
install_name_tool -id @executable_path/../Frameworks/phonon.Framework/Versions/4.0/phonon ${FRMDIR}/phonon.framework/Versions/4/phonon

# Copy in plugins
cp -R /Developer/Applications/Qt/plugins/imageformats $PLUGDIR
cp -R /Developer/Applications/Qt/plugins/phonon_backend $PLUGDIR

# Copy in fftw
cp /opt/local/lib/libfftw3.3.dylib $FRMDIR
cp /opt/local/lib/libfftw3_threads.3.dylib $FRMDIR

# Fix path names to Qt 
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore $EXEDIR/$APPNAME
install_name_tool -change QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui $EXEDIR/$APPNAME
install_name_tool -change QtOpenGL.framework/Versions/4/QtOpenGL @executable_path/../Frameworks/QtOpenGL.framework/Versions/4.0/QtOpenGL $EXEDIR/$APPNAME
install_name_tool -change phonon.framework/Versions/4/phonon @executable_path/../Frameworks/phonon.framework/Versions/4.0/phonon $EXEDIR/$APPNAME

# Fix path names in Qt Libraries
# QtCore
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore $FRMDIR/QtGui.framework/Versions/4.0/QtGui
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore $FRMDIR/QtOpenGL.framework/Versions/4.0/QtOpenGL
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore $FRMDIR/QtSvg.framework/Versions/4.0/QtSvg
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore $FRMDIR/QtXml.framework/Versions/4.0/QtXml
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore $FRMDIR/QtNetwork.framework/Versions/4.0/QtNetwork
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore $FRMDIR/phonon.framework/Versions/4.0/phonon
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore $FRMDIR/QtDBus.framework/Versions/4.0/QtDBus

# QtGui
install_name_tool -change QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui $FRMDIR/QtOpenGL.framework/Versions/4.0/QtOpenGL
install_name_tool -change QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui $FRMDIR/QtSvg.framework/Versions/4.0/QtSvg
install_name_tool -change QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui $FRMDIR/phonon.framework/Versions/4.0/phonon

# More phonon
install_name_tool -change QtXml.framework/Versions/4/QtXml @executable_path/../Frameworks/QtXml.framework/Versions/4.0/QtXml $FRMDIR/phonon.framework/Versions/4.0/phonon
install_name_tool -change QtDBus.framework/Versions/4/QtDBus @executable_path/../Frameworks/QtDBus.framework/Versions/4.0/QtDBus $FRMDIR/phonon.framework/Versions/4.0/phonon

# QtDBus
install_name_tool -change QtXml.framework/Versions/4/QtXml @executable_path/../Frameworks/QtXml.framework/Versions/4.0/QtXml $FRMDIR/QtDBus.framework/Versions/4.0/QtDBus

# Fix plugins
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore $PLUGDIR/imageformats/libqgif.dylib
install_name_tool -change QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui $PLUGDIR/imageformats/libqgif.dylib
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore $PLUGDIR/imageformats/libqico.dylib
install_name_tool -change QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui $PLUGDIR/imageformats/libqico.dylib
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore $PLUGDIR/imageformats/libqjpeg.dylib
install_name_tool -change QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui $PLUGDIR/imageformats/libqjpeg.dylib
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore $PLUGDIR/imageformats/libqmng.dylib
install_name_tool -change QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui $PLUGDIR/imageformats/libqmng.dylib
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore $PLUGDIR/imageformats/libqsvg.dylib
install_name_tool -change QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui $PLUGDIR/imageformats/libqsvg.dylib
install_name_tool -change QtXml.framework/Versions/4/QtXml @executable_path/../Frameworks/QtXml.framework/Versions/4.0/QtXml $PLUGDIR/imageformats/libqsvg.dylib
install_name_tool -change QtSvg.framework/Versions/4/QtSvg @executable_path/../Frameworks/QtSvg.framework/Versions/4.0/QtSvg $PLUGDIR/imageformats/libqsvg.dylib
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore $PLUGDIR/imageformats/libqtiff.dylib
install_name_tool -change QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui $PLUGDIR/imageformats/libqtiff.dylib

install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore $PLUGDIR/phonon_backend/libphonon_qt7.dylib
install_name_tool -change QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui $PLUGDIR/phonon_backend/libphonon_qt7.dylib
install_name_tool -change QtNetwork.framework/Versions/4/QtNetwork @executable_path/../Frameworks/QtNetwork.framework/Versions/4.0/QtNetwork $PLUGDIR/phonon_backend/libphonon_qt7.dylib
install_name_tool -change QtOpenGL.framework/Versions/4/QtOpenGL @executable_path/../Frameworks/QtOpenGL.framework/Versions/4.0/QtOpenGL $PLUGDIR/phonon_backend/libphonon_qt7.dylib
install_name_tool -change QtDBus.framework/Versions/4/QtDBus @executable_path/../Frameworks/QtDBus.framework/Versions/4.0/QtDBus $PLUGDIR/phonon_backend/libphonon_qt7.dylib
install_name_tool -change QtXml.framework/Versions/4/QtXml @executable_path/../Frameworks/QtXml.framework/Versions/4.0/QtXml $PLUGDIR/phonon_backend/libphonon_qt7.dylib
install_name_tool -change phonon.framework/Versions/4/phonon @executable_path/../Frameworks/phonon.framework/Versions/4.0/phonon $PLUGDIR/phonon_backend/libphonon_qt7.dylib

# Fix libz
install_name_tool -change /opt/local/lib/libz.1.dylib /usr/lib/libz.1.dylib ${EXEDIR}/$EXENAME

# Fix other libraries
fixlibs ${EXEDIR}/$APPNAME

