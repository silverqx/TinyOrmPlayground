# Policies <= CMP0126 default to NEW
cmake_minimum_required(VERSION 3.21...3.21 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CommonModules"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules"
)

# Basic project and cmake build setup
# ---

project(TinyOrmPlayground
    DESCRIPTION "Playground for TinyOrm user-friendly ORM"
    HOMEPAGE_URL "https://github.com/silverqx/TinyOrmPlayground"
    LANGUAGES CXX
    VERSION 0.1.0.0
)

# Specify the C++ standard
if(MINGW)
    set(CMAKE_CXX_STANDARD 20)
else()
    set(CMAKE_CXX_STANDARD 23)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)

# Version requirements - older vesions may work, but you are on your own
set(minMsvcVersion 19.28)
set(minQtVersion 5.15.2)
set(minRangeV3Version 0.11.0)
set(minNlohmannJsonVersion 3.9.1)
set(minTinyOrmVersion 0.1.0)

include(TinyHelpers)
# Make minimum toolchain version a requirement
tiny_toolchain_requirement(MSVC ${minMsvcVersion})

# TinyORM build options
# ---

include(FeatureSummary)
include(TinyFeatureOptions)
include(TinyInitDefaultVariables)

# Initialize Tiny variables, earlier init.
tiny_init_tiny_variables_pre()

feature_option(BUILD_SHARED_LIBS
    "Build using shared libraries" ON)
feature_option(MSVC_RUNTIME_DYNAMIC
    "Use MSVC dynamic runtime library (-MD) instead of static (-MT)" ON)
feature_option(VERBOSE_CONFIGURE
    "Show information about PACKAGES_FOUND and PACKAGES_NOT_FOUND in the configure \
output" OFF)

# MSVC_PARALLEL option
include(TinyMsvcParallel)
tiny_msvc_parallel("\
Enables /MP flag for parallel builds using MSVC. Specify an \
integer value to control the number of threads used (Only \
works on some older versions of Visual Studio). Setting to \
ON lets the toolchain decide how many threads to use. Set to \
OFF to disable /MP completely.")

# Initialize Tiny and default cmake variables
tiny_init_cmake_variables()
tiny_init_tiny_variables()

# TinyORM library header and source files
# ---

include(TinySources)
tiny_sources()

add_executable(${TinyOrmPlayground_target}
    ${headers}
    ${sources}
)

if(MINGW)
    target_compile_options(${TinyOrmPlayground_target}
        PRIVATE
            $<$<CONFIG:DEBUG>:-Wa,-mbig-obj>
            # Avoid string table overflow
            $<$<CONFIG:DEBUG>:-O1>
    )
endif()

# Use Precompiled headers (PCH)
# ---

target_precompile_headers(${TinyOrmPlayground_target}
    PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"pch.h">
)

if(NOT CMAKE_DISABLE_PRECOMPILE_HEADERS)
    target_compile_definitions(${TinyOrmPlayground_target} PRIVATE USING_PCH)
endif()

# TinyOrmPlayground specific configuration
# ---

set_target_properties(${TinyOrmPlayground_target}
    PROPERTIES
        C_VISIBILITY_PRESET "hidden"
        CXX_VISIBILITY_PRESET "hidden"
        VISIBILITY_INLINES_HIDDEN YES
        VERSION ${PROJECT_VERSION}
)

target_include_directories(${TinyOrmPlayground_target} PRIVATE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/models>"
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/${TINY_BUILD_GENDIR}/include>"
)

# TinyOrmPlayground defines
# ---

target_compile_definitions(${TinyOrmPlayground_target}
    PRIVATE
        PROJECT_TINYORM_PLAYGROUND
        # Log queries with a time measurement
        TINYORM_DEBUG_SQL
)

# TinyORM defines
# ---

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${TinyOrmPlayground_target}
        PRIVATE TINYORM_LINKING_SHARED
    )
endif()

# File version and Windows resource and manifest
# ---

include(TinyFileVersionAndManifest)
tiny_file_version_and_manifest(${TinyOrmPlayground_target} "src/"
    "${TINY_BUILD_GENDIR}/include/"
    "${TINY_BUILD_GENDIR}/tmp/"
)

# Resolve and link dependencies
# ---

# Unconditional dependencies
find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core Sql)
tiny_find_package(Qt${QT_VERSION_MAJOR} ${minQtVersion} CONFIG
    REQUIRED COMPONENTS Core Sql
)
tiny_find_package(range-v3 ${minRangeV3Version} CONFIG REQUIRED)
tiny_find_package(nlohmann_json ${minNlohmannJsonVersion} CONFIG REQUIRED)
tiny_find_package(TinyOrm ${minTinyOrmVersion} CONFIG REQUIRED)

target_link_libraries(${TinyOrmPlayground_target}
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Sql
        range-v3::range-v3
        nlohmann_json::nlohmann_json
        TinyOrm::TinyOrm
)

# Default rules for deployment
# ---

# Some info output
# ---

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
        message(STATUS "Disabling debug output")
    endif()

    feature_summary(WHAT ALL)
else()
    feature_summary(WHAT ENABLED_FEATURES DISABLED_FEATURES)
endif()
