VERSION = 1.0.0

QT     += core svg webenginewidgets webchannel
CONFIG += c++11 precompile_header

TEMPLATE = app
TARGET   = Void

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_LFLAGS_RELEASE   -= -O1
QMAKE_LFLAGS_RELEASE   += -O3

RC_ICONS = ../res/icon.ico
ICON     = ../res/icon.icns

OBJECTS_DIR = ../object
MOC_DIR     = ../moc

INCLUDEPATH += . /usr/local/opt/nss/include/nss /usr/local/opt/nspr/include/nspr /usr/local/include
LIBS        += -L/usr/local/opt/nss/lib -L/usr/local/opt/nspr/lib -L/usr/local/lib -lnss3 -lnspr4

HEADERS += \
    Crypto.h \
    Store.h \
    StoreFile.h \
    StoreFS.h \
    WelcomeScreen.h \
    WelcomeScreenBridge.h \
    StoreScreen.h \
    StoreScreenBridge.h

SOURCES += \
    main.cpp \
    Crypto.cpp \
    Store.cpp \
    StoreFile.cpp \
    StoreFS.cpp \
    WelcomeScreen.cpp \
    WelcomeScreenBridge.cpp \
    StoreScreen.cpp \
    StoreScreenBridge.cpp

PRECOMPILED_HEADER = precompiled.h

RESOURCES += ../resources.qrc

DISTFILES += ../uncrustify.cfg
