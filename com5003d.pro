TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_on debug

HEADERS	+= scpi.h \
	parse.h \
	cmdinterpret.h \
	zhserver.h \
	zeraglobal.h \
	i2ceeprom.h \
	com5003d.h \
	i2cutils.h \
	crcutils.h \
	intelhexfileio.h \
	justdata.h \
	gaussmatrix.h \
    com5003justdata.h \
    com5003global.h \
    com5003scpi.h \
    pcbserver.h \
    debug.h

SOURCES	+= scpi.cpp \
	parse.cpp \
	main.cpp \
	cmds.cpp \
	cmdinterpret.cpp \
	zhserver.cpp \
	i2ceeprom.cpp \
	com5003d.cpp \
	i2cutils.cpp \
	crcutils.cpp \
	intelhexfileio.cpp \
	justdata.cpp \
	gaussmatrix.cpp \
    com5003justdata.cpp \
    pcbserver.cpp

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}

target.path = /usr/bin
INSTALLS += target

#The following line was inserted by qt3to4
QT += xml  qt3support 
