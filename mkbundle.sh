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

mkdir -p $EXEDIR
mkdir $RESDIR
mkdir $FRMDIR
cp bequnge $EXEDIR/Bequnge
cp Info.plist Bequnge.app/Contents
cp Bequnge.icns ${RESDIR}

cp -R /Library/Frameworks/QtCore.framework $FRMDIR
rm -rf ${FRMDIR}/QtCore.framework/Headers/
install_name_tool -id @executable_path/../Frameworks/QtCore.Framework/Versions/4.0/QtCore ${FRMDIR}/QtCore.framework/Versions/4/QtCore
cp -R /Library/Frameworks/QtGui.framework $FRMDIR
rm -rf ${FRMDIR}/QtGui.framework/Headers/
install_name_tool -id @executable_path/../Frameworks/QtGui.Framework/Versions/4.0/QtGui ${FRMDIR}/QtGui.framework/Versions/4/QtGui
cp -R /Library/Frameworks/QtOpenGL.framework $FRMDIR
rm -rf ${FRMDIR}/QtOpenGL.framework/Headers/
install_name_tool -id @executable_path/../Frameworks/QtOpenGL.Framework/Versions/4.0/QtOpenGL ${FRMDIR}/QtOpenGL.framework/Versions/4/QtOpenGL

# Fix path names to Qt in bequnge
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore $EXEDIR/bequnge
install_name_tool -change QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui $EXEDIR/bequnge
install_name_tool -change QtOpenGL.framework/Versions/4/QtOpenGL @executable_path/../Frameworks/QtOpenGL.framework/Versions/4.0/QtOpenGL $EXEDIR/bequnge

# Fix path names in QtGui & QtOpenGL
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore $FRMDIR/QtGui.framework/Versions/4.0/QtGui
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore $FRMDIR/QtOpenGL.framework/Versions/4.0/QtOpenGL
install_name_tool -change QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui $FRMDIR/QtOpenGL.framework/Versions/4.0/QtOpenGL


fixlibs ${EXEDIR}/bequnge
