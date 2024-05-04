# Policies <= CMP0128 default to NEW
cmake_minimum_required(VERSION 3.22...3.29 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CommonModules"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules"
)

# Initialize Project Version
# ---

include(TinyHelpers)
include(TinyInitDefaultVariables)

# Initialize Tiny variables, early init.
tiny_init_tiny_variables_pre()

tiny_read_version(TINY_VERSION
    TINY_VERSION_MAJOR TINY_VERSION_MINOR TINY_VERSION_PATCH TINY_VERSION_TWEAK
    VERSION_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/version.hpp"
    PREFIX TINYPLAY
    HEADER_FOR "${TinyOrmPlayground_ns}"
)

# Basic project and cmake build setup
# ---

project(${TinyOrmPlayground_ns}
    DESCRIPTION "Playground for TinyOrm user-friendly ORM"
    HOMEPAGE_URL "https://github.com/silverqx/TinyOrmPlayground"
    LANGUAGES CXX
    VERSION ${TINY_VERSION}
)

# Specify the C++ standard
if(MSVC)
    # Make available <format> on MSVC
    set(CMAKE_CXX_STANDARD 23)
else()
    set(CMAKE_CXX_STANDARD 20)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)


# Version requirements
# ---
# Older vesions may work, but you are on your own
# Req - requirement, throws error
# Rec - recommended, shows info message

# Compilers
# 16.10/16.11 (1929) to support pragma system_header
set(minReqMsvcVersion    19.29)
set(minReqClangClVersion 14.0.3)
set(minRecGCCVersion     10.2)
set(minRecClangVersion   12)
# Packages
set(minQtVersion           5.15.2)
# As the range-v3 uses exact version policy in the package config file so passing version
# makes real problems on CI pipelines where different OS images can have installed
# different versions of range-v3 (doesn't matter if it's vcpkg or some package manager)
#set(minRangeV3Version      0.12.0)
set(minNlohmannJsonVersion 3.9.1)
set(minTinyOrmVersion      0.37.2)
# tabulate doesn't provide Package Version File
#set(minTabulateVersion 1.4.0)

# Make minimum toolchain version a requirement
tiny_toolchain_requirement(
    MSVC     ${minReqMsvcVersion}
    CLANG_CL ${minReqClangClVersion}
    GCC      ${minRecGCCVersion}
    CLANG    ${minRecClangVersion}
)

# TinyORM build options
# ---

include(FeatureSummary)
include(TinyFeatureOptions)

feature_option(BUILD_SHARED_LIBS
    "Build using shared libraries" ON
)
feature_option_dependent(MSVC_RUNTIME_DYNAMIC
    "Use MSVC dynamic runtime library (-MD) instead of static (-MT), also considers \
a Debug configuration (-MTd, -MDd)" ON
    "MSVC;NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY" MSVC_RUNTIME_DYNAMIC-NOTFOUND
)
feature_option(VERBOSE_CONFIGURE
    "Show information about PACKAGES_FOUND and PACKAGES_NOT_FOUND in the configure \
output" OFF
)

# MSVC_PARALLEL option
include(TinyMsvcParallel)
tiny_msvc_parallel("\
Enables /MP flag for parallel builds using MSVC. Specify an \
integer value to control the number of threads used (Only \
works on some older versions of Visual Studio). Setting to \
ON lets the toolchain decide how many threads to use. Set to \
OFF to disable /MP completely.")

# Initialize Tiny and default CMake variables
tiny_init_cmake_variables()
tiny_init_tiny_variables()

# Throw a fatal error for unsupported environments
tiny_check_unsupported_build()

# TinyOrmPlayground executable header and source files
# ---

include(TinySources)
tiny_sources(${TinyOrmPlayground_target}_headers ${TinyOrmPlayground_target}_sources)

add_executable(${TinyOrmPlayground_target}
    ${${TinyOrmPlayground_target}_headers}
    ${${TinyOrmPlayground_target}_sources}
)
add_executable(${TinyOrmPlayground_ns}::${TinyOrmPlayground_target}
    ALIAS ${TinyOrmPlayground_target}
)

# Use Precompiled headers (PCH)
# ---

target_precompile_headers(${TinyOrmPlayground_target}
    PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"pch.h">
)

if(NOT CMAKE_DISABLE_PRECOMPILE_HEADERS)
    target_compile_definitions(${TinyOrmPlayground_target} PRIVATE TINYPLAY_USING_PCH)
endif()

# TinyOrmPlayground specific configuration
# ---

set_target_properties(${TinyOrmPlayground_target}
    PROPERTIES
        C_VISIBILITY_PRESET "hidden"
        CXX_VISIBILITY_PRESET "hidden"
        VISIBILITY_INLINES_HIDDEN YES
        VERSION ${PROJECT_VERSION}
        # Append a major version number after the executable
        OUTPUT_NAME "${TinyOrmPlayground_target}${PROJECT_VERSION_MAJOR}"
        DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
)

target_include_directories(${TinyOrmPlayground_target} PRIVATE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/models>"
)

# TinyOrmPlayground compiler options
# ---

if(MINGW)
    target_compile_options(${TinyOrmPlayground_target}
        PRIVATE
            $<$<CONFIG:DEBUG>:-Wa,-mbig-obj>
            # Avoid a string table overflow
            $<$<CONFIG:DEBUG>:-O1>
    )
endif()

# TinyOrmPlayground defines
# ---

target_compile_definitions(${TinyOrmPlayground_target}
    PRIVATE
        PROJECT_TINYORMPLAYGROUND
        # Release build
        $<$<NOT:$<CONFIG:Debug>>:QT_NO_DEBUG_OUTPUT>
        $<$<NOT:$<CONFIG:Debug>>:TINYPLAY_NO_DEBUG>
        # Debug build
        $<$<CONFIG:Debug>:TINYPLAY_DEBUG>
)

# TinyORM defines
# ---

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${TinyOrmPlayground_target}
        PRIVATE TINYORM_LINKING_SHARED
    )
endif()

target_compile_definitions(${TinyOrmPlayground_target}
    PRIVATE
        # Release build
        $<$<NOT:$<CONFIG:Debug>>:TINYORM_NO_DEBUG>
        $<$<NOT:$<CONFIG:Debug>>:TINYORM_NO_DEBUG_SQL>
        # Debug build
        $<$<CONFIG:Debug>:TINYORM_DEBUG>
        $<$<CONFIG:Debug>:TINYORM_DEBUG_SQL>
)

# Windows resource and manifest files
# ---

# Find Windows manifest file
if(MINGW)
    tiny_set_rc_flags("-I \"${CMAKE_CURRENT_SOURCE_DIR}/resources\"")
endif()

include(TinyResourceAndManifest)
tiny_resource_and_manifest(${TinyOrmPlayground_target}
    OUTPUT_DIR "${TINY_BUILD_GENDIR}/tmp/"
)

# Resolve and link dependencies
# ---

# Unconditional dependencies
find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core Sql)
tiny_find_package(Qt${QT_VERSION_MAJOR} ${minQtVersion} CONFIG
    REQUIRED COMPONENTS Core #[[Network]] Sql
)
tiny_find_package(range-v3 ${minRangeV3Version} CONFIG REQUIRED)
tiny_find_package(nlohmann_json ${minNlohmannJsonVersion} CONFIG REQUIRED)
tiny_find_package(TinyOrm ${minTinyOrmVersion} CONFIG REQUIRED)

target_link_libraries(${TinyOrmPlayground_target}
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
#        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Sql
        range-v3::range-v3
        nlohmann_json::nlohmann_json
        TinyOrm::TinyOrm
)

# Default rules for deployment
# ---

# Some info output
# ---

tiny_print_linking_against(TinyOrm::TinyOrm)

set_package_properties(QT
    PROPERTIES
        URL "https://doc.qt.io/qt-${QT_VERSION_MAJOR}/"
        DESCRIPTION "Qt is a full development framework"
        TYPE REQUIRED
        PURPOSE "Provides SQL database layer by the QtSql module, QVariant, and QString"
)
set_package_properties(Qt${QT_VERSION_MAJOR}
    PROPERTIES
        URL "https://doc.qt.io/qt-${QT_VERSION_MAJOR}/"
        DESCRIPTION "Qt is a full development framework"
        TYPE REQUIRED
        PURPOSE "Provides SQL database layer by the QtSql module, QVariant, and QString"
)
set_package_properties(Qt${QT_VERSION_MAJOR}Core
    PROPERTIES
        URL "https://doc.qt.io/qt-${QT_VERSION_MAJOR}/qtcore-index.html"
        DESCRIPTION "Core non-graphical classes used by other modules"
        TYPE REQUIRED
        PURPOSE "Provides QVariant, QString, and Qt containers"
)
#set_package_properties(Qt${QT_VERSION_MAJOR}Network
#    PROPERTIES
#        URL "https://doc.qt.io/qt-${QT_VERSION_MAJOR}/qtnetwork-index.html"
#        DESCRIPTION "Classes to make network programming easier and more portable"
#        TYPE REQUIRED
#        PURPOSE "Provides QHostInfo (for now unused)"
#)
set_package_properties(Qt${QT_VERSION_MAJOR}Sql
    PROPERTIES
        URL "https://doc.qt.io/qt-${QT_VERSION_MAJOR}/qtsql-index.html"
        DESCRIPTION "Classes for database integration using SQL"
        TYPE REQUIRED
        PURPOSE "Provides SQL database layer"
)
set_package_properties(range-v3
    PROPERTIES
        URL "https://ericniebler.github.io/range-v3/"
        DESCRIPTION "Range algorithms, views, and actions for STL"
        TYPE REQUIRED
        PURPOSE "Used to have a nice and clear code"
)
set_package_properties(nlohmann_json
    PROPERTIES
        URL "https://json.nlohmann.me/"
        DESCRIPTION "JSON for Modern C++"
        TYPE REQUIRED
        PURPOSE "In fact, it is not used, only tried in some example code"
)
set_package_properties(TinyOrm
    PROPERTIES
        URL "https://silverqx.github.io/TinyORM/"
        DESCRIPTION "c++ ORM library for Qt framework"
        TYPE REQUIRED
        PURPOSE "An ORM framework for which is this playground for"
)

if(VERBOSE_CONFIGURE)
    if(NOT TINY_IS_MULTI_CONFIG AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(STATUS "Disabled debug output and asserts")
    endif()

    feature_summary(WHAT ALL)
else()
    feature_summary(WHAT ENABLED_FEATURES DISABLED_FEATURES)
endif()
