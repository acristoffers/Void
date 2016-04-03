VERSION = 1.0.0

QT     += core widgets concurrent svg
CONFIG += c++11 #precompile_header

TEMPLATE = app
TARGET   = Void

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_LFLAGS_RELEASE   -= -O1
QMAKE_LFLAGS_RELEASE   += -O3

RC_ICONS = resources/icon.ico
ICON = resources/icon.icns

INCLUDEPATH += . /usr/local/opt/nss/include/nss /usr/local/opt/nspr/include/nspr /usr/local/include
LIBS        += -L/usr/local/opt/nss/lib -L/usr/local/opt/nspr/lib -L/usr/local/lib -lnss3 -lnspr4

HEADERS += \
    src/Crypto.h \
    src/Store.h \
    src/StoreFile.h

SOURCES += \
    src/main.cpp \
    src/Crypto.cpp \
    src/Store.cpp \
    src/StoreFile.cpp

PRECOMPILED_HEADER = src/precompiled.h

RESOURCES += resources.qrc
