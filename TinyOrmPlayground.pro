QT *= core sql
QT -= gui

TEMPLATE = app

# Version requirements
# ---
# Older vesions may work, but you are on your own
# Req - requirement, throws error
# Rec - recommended, shows message

# 16.10/16.11 (1929) - to support #pragma system_header
tinyMinReqMsvc  = 19.29
tinyMinRecClang = 12
tinyMinRecGCC   = 10.2
tinyMinRecQt    = 5.15.2

# Make minimum toolchain version a requirement
load(tiny_toolchain_requirement)

# Configuration
# ---

CONFIG *= console c++2a strict_c++ warn_on utf8_source link_prl hide_symbols silent
CONFIG -= c++11 app_bundle

# TinyOrmPlayground defines
# ---

DEFINES += PROJECT_TINYORM_PLAYGROUND

# Release build
CONFIG(release, debug|release): DEFINES += TINYPLAY_NO_DEBUG
# Debug build
CONFIG(debug, debug|release): DEFINES *= TINYPLAY_DEBUG

# Qt defines
# ---

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
# Disables all the APIs deprecated before Qt 6.0.0
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

#DEFINES *= QT_ASCII_CAST_WARNINGS
#DEFINES *= QT_NO_CAST_FROM_ASCII
#DEFINES *= QT_RESTRICTED_CAST_FROM_ASCII
DEFINES *= QT_NO_CAST_TO_ASCII
DEFINES *= QT_NO_CAST_FROM_BYTEARRAY
DEFINES *= QT_USE_QSTRINGBUILDER
DEFINES *= QT_STRICT_ITERATORS

# Disable debug output in release mode
CONFIG(release, debug|release): DEFINES *= QT_NO_DEBUG_OUTPUT

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

# Find version.hpp
tinyRcIncludepath = $$quote($$PWD/src/)
# Find Windows manifest
mingw: tinyRcIncludepath += $$quote($$PWD/resources/)

load(tiny_resource_and_manifest)
tiny_resource_and_manifest($$tinyRcIncludepath)

# Use Precompiled headers (PCH)
# ---

precompile_header: \
    include($$PWD/src/pch.pri)

# Default rules for deployment
# ---

CONFIG(release, debug|release) {
    win32-msvc: target.path = C:/optx64/$${TARGET}
#    else: unix:!android: target.path = /opt/$${TARGET}/bin
    !isEmpty(target.path): INSTALLS += target
}

# Some info output
# ---

CONFIG(debug, debug|release):!build_pass: message( "Project is built in DEBUG mode." )
CONFIG(release, debug|release):!build_pass: \
    message( "Project is built in RELEASE mode (disabled debug output and assert)." )

# Common Logic
# ---

include(qmake/common.pri)

# Link against TinyORM library (also adds defines and include headers)
# ---

include($$PWD/../../TinyOrm/TinyORM/qmake/TinyOrm.pri)

# User Configuration
# ---

exists(conf.pri): \
    include(conf.pri)

else: \
    error( "'conf.pri' for '$${TARGET}' project does not exist. See an example\
            configuration in 'conf.pri.example' in the project's root." )

# Link against TinyORM library
# ---

LIBS *= -lTinyOrm
