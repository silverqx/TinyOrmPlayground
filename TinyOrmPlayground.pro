QT *= core sql
QT -= gui

TEMPLATE = app

# Version requirements
# ---
# Older vesions may work, but you are on your own
# Req - requirement, throws error
# Rec - recommended, shows info message

# 16.10/16.11 (1929) - to support #pragma system_header
tinyMinReqMsvc    = 19.29
tinyMinRecClang   = 12
tinyMinRecGCC     = 10.2
tinyMinRecQt      = 5.15.2
tinyMinReqClangCl = 14.0.3

# Make minimum toolchain version a requirement
load(tiny_toolchain_requirement)

# Configuration
# ---

CONFIG *= cmdline

# TinyOrmPlayground defines
# ---

DEFINES += PROJECT_TINYORM_PLAYGROUND

# Release build
CONFIG(release, debug|release): DEFINES += TINYPLAY_NO_DEBUG
# Debug build
CONFIG(debug, debug|release): DEFINES *= TINYPLAY_DEBUG

# Qt defines
# ---

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

!build_pass {
    CONFIG(debug, debug|release): \
        message( "Project is built in DEBUG mode." )

    CONFIG(release, debug|release): \
        message( "Project is built in RELEASE mode (disabled debug output and asserts)." )
}

# Common Logic
# ---

include(qmake/common.pri)

# Configure TinyORM library
# ---

# Provides:
# - TINY_BUILD_SUBFOLDER - folder by release type (/debug, /release, or empty)
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

# Others
# ---

#CONFIG -= silent
