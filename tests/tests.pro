VERSION = 1.0.0

QT     += core testlib qml testlib
QT     -= gui
CONFIG += c++17 precompile_header testcase sdk_no_version_check

TEMPLATE = app
TARGET   = VoidTests

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_LFLAGS_RELEASE   -= -O1
QMAKE_LFLAGS_RELEASE   += -O3

RC_ICONS = ../res/icon.ico
ICON     = ../res/icon.icns

OBJECTS_DIR = ../object
MOC_DIR     = ../moc

INCLUDEPATH += $$PWD/../src
CONFIG += link_pkgconfig
PKGCONFIG += nss openssl

unix {
    LIBS += $$OBJECTS_DIR/Crypto.o \
            $$OBJECTS_DIR/StoreFS.o \
            $$OBJECTS_DIR/moc_Store.o \
            $$OBJECTS_DIR/Store.o \
            $$OBJECTS_DIR/StoreFile.o
}

win32 {
    LIBS += $$OBJECTS_DIR/Crypto.obj \
            $$OBJECTS_DIR/StoreFS.obj \
            $$OBJECTS_DIR/moc_Store.obj \
            $$OBJECTS_DIR/Store.obj \
            $$OBJECTS_DIR/StoreFile.obj
}

HEADERS = VoidTest.h
SOURCES = VoidTest.cpp

PRECOMPILED_HEADER = ../src/precompiled.h

RESOURCES += ../resources.qrc

DISTFILES += ../uncrustify.cfg
