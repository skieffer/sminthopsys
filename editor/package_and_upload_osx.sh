#!/bin/sh -x

MACPORTS=/opt/local
QT=/usr/local/Qt4.8

export PATH=$QT/bin:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin:/usr/X11/bin

( 
cd ..

macdeployqt Dunnart.app

for LIB in Dunnart.app/Contents/Frameworks/libdunnartcanvas.1.0.0.dylib Dunnart.app/Contents/PlugIns/*.dylib
do
	for QTLIB in QtCore QtGui QtSvg QtXml
	do
		/usr/bin/install_name_tool -change $QT/lib/$QTLIB.framework/Versions/4/$QTLIB @executable_path/../Frameworks/$QTLIB.framework/Versions/4/$QTLIB $LIB
		/usr/bin/install_name_tool -change $QTLIB.framework/Versions/4/$QTLIB @executable_path/../Frameworks/$QTLIB.framework/Versions/4/$QTLIB $LIB
	done 
done


rm -rf dunnart-osx.tar.gz

tar zcf dunnart-osx.tar.gz Dunnart.app examples

scp dunnart-osx.tar.gz skieffer@marvl.infotech.monash.edu.au:./Sites/dunnart/binaries/

rm -rf Dunnart.app
)
