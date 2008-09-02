#!/bin/sh

fixlibs() {
	echo $1
	install_name_tool -id `basename $1` $1
	finklibs=`otool -L $1 | grep '/sw/lib' | grep -v $1 | cut -d ' ' -f 1 | sed -e 's#^ *##'`
	for i in $finklibs
	do
		cp $i $FRMDIR
		finklib=`basename $i`
		echo $finklib
		install_name_tool -change $i @executable_path/../Frameworks/${finklib} $1
		echo $i | grep -q `basename $1` || fixlibs ${FRMDIR}/$finklib
	done
}

EXEDIR="Bequnge.app/Contents/MacOS"
RESDIR="Bequnge.app/Contents/Resources"
FRMDIR="Bequnge.app/Contents/Frameworks"
PLUGDIR="Bequnge.app/Contents/plugins"

mkdir -p $EXEDIR
mkdir $RESDIR
mkdir $FRMDIR
mkdir $PLUGDIR
cp bequnge $EXEDIR/Bequnge
cp Info.plist Bequnge.app/Contents
cp qt.conf ${RESDIR}
cp Bequnge.icns ${RESDIR}

copyqtlib()
{
	cp -R /Library/Frameworks/${1}.framework ${FRMDIR}
	rm -rf ${FRMDIR}/${1}.framework/Headers/
	rm -f ${FRMDIR}/${1}.framework/Versions/4/${1}_debug
	install_name_tool -id @executable_path/../Frameworks/${1}.framework/Versions/4.0/${1} ${FRMDIR}/${1}.framework/Versions/4/${1}
}

# Copy in Qt frameworks
copyqtlib QtCore
copyqtlib QtGui
copyqtlib QtOpenGL
copyqtlib QtSvg
copyqtlib QtXml
copyqtlib QtDBus
copyqtlib QtNetwork
copyqtlib phonon

# Copy in plugins
cp -R /Developer/Applications/Qt/plugins/imageformats $PLUGDIR
cp -R /Developer/Applications/Qt/plugins/phonon_backend $PLUGDIR
rm -f ${PLUGDIR}/imageformats/*_debug.dylib
rm -f ${PLUGDIR}/phonon_backend/*_debug.dylib

# Fix path names to Qt in bequnge
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore $EXEDIR/bequnge
install_name_tool -change QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui $EXEDIR/bequnge
install_name_tool -change QtOpenGL.framework/Versions/4/QtOpenGL @executable_path/../Frameworks/QtOpenGL.framework/Versions/4.0/QtOpenGL $EXEDIR/bequnge
install_name_tool -change phonon.framework/Versions/4/phonon @executable_path/../Frameworks/phonon.framework/Versions/4.0/phonon $EXEDIR/bequnge
install_name_tool -change QtXml.framework/Versions/4/QtXml @executable_path/../Frameworks/QtXml.framework/Versions/4.0/QtXml $EXEDIR/bequnge
install_name_tool -change QtDBus.framework/Versions/4/QtDBus @executable_path/../Frameworks/QtDBus.framework/Versions/4.0/QtDBus $EXEDIR/bequnge

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

# Fix other libraries
fixlibs ${EXEDIR}/Bequnge

