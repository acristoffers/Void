VERSION = 1.0.0

QT     += core widgets concurrent svg testlib
CONFIG += c++11 precompile_header testcase

TEMPLATE = app
TARGET   = Void

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_LFLAGS_RELEASE   -= -O1
QMAKE_LFLAGS_RELEASE   += -O3

RC_ICONS = ../resources/icon.ico
ICON     = ../resources/icon.icns

OBJECTS_DIR = ../object
MOC_DIR     = ../moc

INCLUDEPATH += . /usr/local/opt/nss/include/nss /usr/local/opt/nspr/include/nspr /usr/local/include
LIBS        += -L/usr/local/opt/nss/lib -L/usr/local/opt/nspr/lib -L/usr/local/lib -lnss3 -lnspr4

HEADERS += \
    Crypto.h \
    Store.h \
    StoreFile.h \
    StoreFS.h

SOURCES += \
    main.cpp \
    Crypto.cpp \
    Store.cpp \
    StoreFile.cpp \
    StoreFS.cpp

PRECOMPILED_HEADER = precompiled.h

RESOURCES += ../resources.qrc

DISTFILES += ../uncrustify.cfg
