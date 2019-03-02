VERSION = 1.0.0

QT     += core svg webenginewidgets webchannel multimedia multimediawidgets testlib
CONFIG += c++17 precompile_header sdk_no_version_check

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

mac {
    INCLUDEPATH += . /usr/local/opt/nss/include/nss /usr/local/opt/nspr/include/nspr /usr/local/opt/openssl/include /usr/local/include
    LIBS        += -L/usr/local/opt/nss/lib -L/usr/local/opt/nspr/lib -L/usr/local/opt/openssl/lib -L/usr/local/lib -lnss3 -lnspr4 -lssl -lcrypto -lz
    HEADERS += MacOSKeychainManagement.h
    OBJECTIVE_SOURCES += MacOSKeychainManagement.mm
    LIBS += -framework Foundation -framework Security
}

linux {
    CONFIG += link_pkgconfig
    PKGCONFIG += nss openssl
}

HEADERS += \
    Crypto.h \
    Store.h \
    StoreFile.h \
    StoreFS.h \
    WelcomeScreen.h \
    WelcomeScreenBridge.h \
    StoreScreen.h \
    StoreScreenBridge.h \
    SchemeHandler.h \
    Runner.h \
    VideoPlayer.h \
    VideoPlayerWidget.h

SOURCES += \
    main.cpp \
    Crypto.cpp \
    Store.cpp \
    StoreFile.cpp \
    StoreFS.cpp \
    WelcomeScreen.cpp \
    WelcomeScreenBridge.cpp \
    StoreScreen.cpp \
    StoreScreenBridge.cpp \
    SchemeHandler.cpp \
    Runner.cpp \
    VideoPlayer.cpp \
    VideoPlayerWidget.cpp

PRECOMPILED_HEADER = precompiled.h

QMAKE_RESOURCE_FLAGS += --compress 9 --threshold 1
RESOURCES += ../resources.qrc

DISTFILES += ../uncrustify.cfg

FORMS += VideoPlayerWidget.ui
