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

EXEDIR="Modelfitting.app/Contents/MacOS"
RESDIR="Modelfitting.app/Contents/Resources"
FRMDIR="Modelfitting.app/Contents/Frameworks"
PLGDIR="Modelfitting.app/Contents/Plugins"

rm -rf Modelfitting.app
mkdir -p $EXEDIR
mkdir $RESDIR
mkdir $FRMDIR
mkdir -p $PLGDIR/imageformats
cp ../bin/modelfitting $EXEDIR/Modelfitting
cp Info.plist Modelfitting.app/Contents
cp qt.conf $RESDIR


# Copy Qt libraries
cp -R /Library/Frameworks/QtCore.framework $FRMDIR
rm -rf ${FRMDIR}/QtCore.framework/Headers/
install_name_tool -id @executable_path/../Frameworks/QtCore.Framework/Versions/4.0/QtCore ${FRMDIR}/QtCore.framework/Versions/4/QtCore
cp -R /Library/Frameworks/QtGui.framework $FRMDIR
rm -rf ${FRMDIR}/QtGui.framework/Headers/
install_name_tool -id @executable_path/../Frameworks/QtGui.Framework/Versions/4.0/QtGui ${FRMDIR}/QtGui.framework/Versions/4/QtGui
cp -R /Library/Frameworks/QtOpenGL.framework $FRMDIR
rm -rf ${FRMDIR}/QtOpenGL.framework/Headers/
install_name_tool -id @executable_path/../Frameworks/QtOpenGL.Framework/Versions/4.0/QtOpenGL ${FRMDIR}/QtOpenGL.framework/Versions/4/QtOpenGL

# Fix path names to Qt in app
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore $EXEDIR/modelfitting
install_name_tool -change QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui $EXEDIR/modelfitting
install_name_tool -change QtOpenGL.framework/Versions/4/QtOpenGL @executable_path/../Frameworks/QtOpenGL.framework/Versions/4.0/QtOpenGL $EXEDIR/modelfitting

# Fix path names in QtGui & QtOpenGL
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore $FRMDIR/QtGui.framework/Versions/4.0/QtGui
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore $FRMDIR/QtOpenGL.framework/Versions/4.0/QtOpenGL
install_name_tool -change QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui $FRMDIR/QtOpenGL.framework/Versions/4.0/QtOpenGL

# Copy Qt plugins
cp /Developer/Applications/Qt/plugins/imageformats/libqgif.dylib /Developer/Applications/Qt/plugins/imageformats/libqsvg.dylib $PLGDIR/imageformats

# Copy other libraries
cp /opt/local/lib/libfftw3.3.dylib $FRMDIR
cp /opt/local/lib/libfftw3_threads.3.dylib $FRMDIR

# Fix linking to libz
install_name_tool -change /opt/local/lib/libz.1.dylib /usr/lib/libz.1.dylib ${EXEDIR}/modelfitting


fixlibs ${EXEDIR}/modelfitting
