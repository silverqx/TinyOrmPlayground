QT *= core sql
QT -= gui

TEMPLATE = app
TARGET = TinyOrmPlayground

# Configuration
# ---

CONFIG *= console c++2a strict_c++ warn_on utf8_source link_prl hide_symbols silent
CONFIG -= c++11 app_bundle

# TinyOrmPlayground defines
# ---

DEFINES += PROJECT_TINYORM_PLAYGROUND

# Qt defines
# ---

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

# TinyORM defines
# ---

# Log queries with a time measurement
DEFINES += TINYORM_DEBUG_SQL
DEFINES += TINYORM_LINKING_SHARED

# Platform specific configuration
# ---

win32: include(qmake/winconf.pri)
macx: include(qmake/macxconf.pri)
mingw|if(unix:!macx): include(qmake/unixconf.pri)

# TinyOrmPlayground header and source files
# ---

include(src/src.pri)

# File version
# ---

# Find version numbers in the version header file and assign them to the
# <TARGET>_VERSION_<MAJOR,MINOR,PATCH,TWEAK> and also to the VERSION variable.
load(tiny_version_numbers)
tiny_version_numbers()

# Windows resource and manifest files
# ---

win32 {
    # Find icons, Windows manifest on MinGW and orm/version.hpp
    RC_INCLUDEPATH = \
        $$quote($$PWD/resources/) \
        $$quote($$PWD/src/)

    load(tiny_resource_and_manifest)
    tiny_resource_and_manifest()
}

# File version and windows manifest
# ---

#win32-msvc {
#    QMAKE_TARGET_PRODUCT = TinyOrmPlayground
#    QMAKE_TARGET_DESCRIPTION = Playground for TinyORM user-friendly ORM
#    QMAKE_TARGET_COMPANY = Crystal Studio
#    QMAKE_TARGET_COPYRIGHT = Copyright (©) 2021 Crystal Studio
##    RC_ICONS = images/TinyOrmPlayground.ico
#    RC_LANG = 1033
#}

# Use Precompiled headers (PCH)
# ---

precompile_header: \
    include($$PWD/src/pch.pri)

# Deployment
# ---

CONFIG(release, debug|release) {
    win32-msvc: target.path = C:/optx64/$${TARGET}
#    else: unix:!android: target.path = /opt/$${TARGET}/bin
    !isEmpty(target.path): INSTALLS += target
}

# Some info output
# ---

CONFIG(debug, debug|release): message( "Project is built in DEBUG mode." )
CONFIG(release, debug|release): message( "Project is built in RELEASE mode." )

# Disable debug output in release mode
CONFIG(release, debug|release) {
    message( "Disabling debug output." )
    DEFINES += QT_NO_DEBUG_OUTPUT
}

# Common Logic
# ---

include(qmake/common.pri)

# User Configuration
# ---

exists(conf.pri): \
    include(conf.pri)

else: \
    error( "'conf.pri' for '$${TARGET}' project does not exist. See an example\
            configuration in 'conf.pri.example' in the project's root." )

# Dependencies include and library paths
# ---

LIBS += -lTinyOrm
