QT *= core sql
QT -= gui

TEMPLATE = app

# Configuration
# ---

CONFIG *= console c++2a strict_c++ silent warn_on link_prl
CONFIG -= c++11 app_bundle

# Some info output
# ---

CONFIG(debug, debug|release): message( "Project is built in DEBUG mode." )
CONFIG(release, debug|release): message( "Project is built in RELEASE mode." )

# TinyOrmPlayground defines
# ---

DEFINES += PROJECT_TINYORM_PLAYGROUND
# Log queries with time measurement, used only when viewing source code
DEFINES += TINYORM_DEBUG_SQL

# Qt defines
# ---

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
# Disables all the APIs deprecated before Qt 6.0.0
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

#DEFINES += QT_NO_CAST_FROM_ASCII
#DEFINES += QT_RESTRICTED_CAST_FROM_ASCII
DEFINES += QT_NO_CAST_TO_ASCII
DEFINES += QT_NO_CAST_FROM_BYTEARRAY
DEFINES += QT_USE_QSTRINGBUILDER
DEFINES += QT_STRICT_ITERATORS

# Other libraries defines
# ---

DEFINES += TINYORM_LINKING_SHARED

# WinApi
# ---

# Windows 10 1903 "19H1" - 0x0A000007
DEFINES += NTDDI_VERSION=0x0A000007
# Windows 10 - 0x0A00
DEFINES += _WIN32_WINNT=0x0A00
DEFINES += _WIN32_IE=0x0A00
DEFINES += UNICODE
DEFINES += _UNICODE
DEFINES += WIN32
DEFINES += _WIN32
DEFINES += WIN32_LEAN_AND_MEAN
DEFINES += NOMINMAX

# Compilers
# ---

win32-msvc* {
    # I don't use -MP flag, because using jom
    QMAKE_CXXFLAGS += -guard:cf -permissive- -Zc:ternary
    QMAKE_CXXFLAGS_DEBUG += -bigobj
    QMAKE_LFLAGS += /guard:cf
    QMAKE_LFLAGS_RELEASE += /OPT:REF /OPT:ICF=5
}

# My variables
# ---

TINY_QT_VERSION_UNDERSCORED = $$replace(QT_VERSION, \., _)
CONFIG(release, debug|release) {
    TINY_RELEASE_TYPE = release
    TINY_RELEASE_TYPE_CAMEL = Release
}
else:CONFIG(debug, debug|release) {
    TINY_RELEASE_TYPE = debug
    TINY_RELEASE_TYPE_CAMEL = Debug
}

# Dependencies include and library paths
# ---

INCLUDEPATH += $$quote($$PWD/../../TinyOrm/TinyOrm/include)

win32-g++* {
}
else:win32-msvc* {
    LIBS += $$quote(-L$$PWD/../../TinyOrm/TinyOrm-builds/build-TinyOrm-Desktop_Qt_$${TINY_QT_VERSION_UNDERSCORED}_MSVC2019_64bit-$${TINY_RELEASE_TYPE_CAMEL}/src/$${TINY_RELEASE_TYPE}/)
}
else:unix {
}

LIBS += -lTinyOrm

# File version and windows manifest
# ---

win32:VERSION = 0.1.0.0
else:VERSION = 0.1.0

win32-msvc* {
    QMAKE_TARGET_PRODUCT = TinyOrmPlayground
    QMAKE_TARGET_DESCRIPTION = Playground for TinyOrm user-friendly ORM
    QMAKE_TARGET_COMPANY = Crystal Studio
    QMAKE_TARGET_COPYRIGHT = Copyright (©) 2020 Crystal Studio
#    RC_ICONS = images/TinyOrmPlayground.ico
    RC_LANG = 1033
}

# Use Precompiled headers (PCH)
# ---

PRECOMPILED_HEADER = $$quote($$PWD/src/pch.h)

precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
}

HEADERS += $$quote($$PWD/src/pch.h)

# TinyOrmPlayground header and source files
# ---

include(src/src.pri)

# Default rules for deployment
# ---

release {
    win32-msvc*: target.path = C:/optx64/$${TARGET}
    else: unix:!android: target.path = /opt/$${TARGET}/bin
    !isEmpty(target.path): INSTALLS += target
}
