include(TinyHelpers)

# Initialize CMake default variables by project options
macro(tiny_init_cmake_variables)

    # Especially important for multi-config generators, I leave it to also kick-in for
    # single-config generators
    set(CMAKE_DEBUG_POSTFIX d CACHE STRING
        "Default filename postfix for executable for Debug configuration")

    set(CMAKE_FIND_PACKAGE_SORT_ORDER NATURAL CACHE STRING
        "The default order for sorting packages found using find_package()")
    set(CMAKE_FIND_PACKAGE_SORT_DIRECTION DEC CACHE STRING
        "The sorting direction used by CMAKE_FIND_PACKAGE_SORT_ORDER")

    set(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION ON CACHE BOOL
        "Ask cmake_install.cmake script to warn each time a file with absolute INSTALL \
DESTINATION is encountered")

    mark_as_advanced(
        CMAKE_DEBUG_POSTFIX
        CMAKE_FIND_PACKAGE_SORT_ORDER
        CMAKE_FIND_PACKAGE_SORT_DIRECTION
        CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION
    )

    # Allow to select dynamic/static MSVC runtime
    if(MSVC AND NOT MSVC_RUNTIME_DYNAMIC STREQUAL MSVC_RUNTIME_DYNAMIC-NOTFOUND)
        if(MSVC_RUNTIME_DYNAMIC)
            set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
        else()
            set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        endif()
    endif()

    # TODO test on unix silverqx
#    set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

    # Fix install prefix for the MinGW and x64 toolchain
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows"
            AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT
    )
        get_property(help_string CACHE CMAKE_INSTALL_PREFIX PROPERTY HELPSTRING)
        if(NOT help_string)
            set(help_string "Install path prefix, prepended onto install directories")
        endif()

        if(MINGW)
            set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "${help_string}" FORCE)

        elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(CMAKE_INSTALL_PREFIX "C:/Program Files/${PROJECT_NAME}"
                CACHE PATH "${help_string}" FORCE
            )
        endif()
    endif()

    # Avoid to link a release type builds against a debug build
    set(helpStringTemplate
        "Map from <CONFIG> project configuration to an imported target's configuration")

    string(REPLACE "<CONFIG>" "Release" release_helpString ${helpStringTemplate})
    string(REPLACE "<CONFIG>" "RelWithDebInfo" relWithDebInfo_helpString
        ${helpStringTemplate})
    string(REPLACE "<CONFIG>" "MinSizeRel" minSizeRel_helpString ${helpStringTemplate})
    string(REPLACE "<CONFIG>" "Debug" debug_helpString ${helpStringTemplate})

    set(CMAKE_MAP_IMPORTED_CONFIG_RELEASE Release RelWithDebInfo MinSizeRel ""
        CACHE STRING ${release_helpString})
    set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO RelWithDebInfo Release MinSizeRel ""
        CACHE STRING ${relWithDebInfo_helpString})
    set(CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL MinSizeRel RelWithDebInfo Release ""
        CACHE STRING ${minSizeRel_helpString})

    # MSVC runtime library crashes if you do not link a debug build against a debug build
    if(MSVC)
        set(CMAKE_MAP_IMPORTED_CONFIG_DEBUG Debug "" CACHE STRING ${debug_helpString})
    else()
        set(CMAKE_MAP_IMPORTED_CONFIG_DEBUG Debug RelWithDebInfo Release MinSizeRel ""
            CACHE STRING ${debug_helpString})
    endif()

    mark_as_advanced(
        CMAKE_MAP_IMPORTED_CONFIG_RELEASE
        CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO
        CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL
        CMAKE_MAP_IMPORTED_CONFIG_DEBUG
    )

    if(VERBOSE_CONFIGURE)
        message(STATUS "${TinyOrmPlayground_ns}: Set up defaults for \
CMAKE_MAP_IMPORTED_CONFIG_<CONFIG> to avoid link a release type builds against a debug \
build

 * CMAKE_MAP_IMPORTED_CONFIG_RELEASE        = ${CMAKE_MAP_IMPORTED_CONFIG_RELEASE}
 * CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO = ${CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO}
 * CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL     = ${CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL}
 * CMAKE_MAP_IMPORTED_CONFIG_DEBUG          = ${CMAKE_MAP_IMPORTED_CONFIG_DEBUG}
")
    endif()

    # Used to save and restore original content of the CMAKE_RC_FLAGS variable
    set(TINY_RC_FLAGS_BACKUP "")

endmacro()

# Initialize Tiny variables, early init.
macro(tiny_init_tiny_variables_pre)

    # Top level project name, used for alias namespaces, CMAKE_MESSAGE_CONTEXT, or as
    # a main package name
    set(TinyOrmPlayground_ns TinyOrmPlayground)
    # Target names
    set(TinyOrmPlayground_target TinyOrmPlayground)

endmacro()

# Initialize Tiny variables
macro(tiny_init_tiny_variables)

    set(TINY_BUILD_GENDIR "${TinyOrmPlayground_target}_generated" CACHE INTERNAL
        "Generated content in the build tree")

    get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    set(TINY_IS_MULTI_CONFIG "${isMultiConfig}" CACHE INTERNAL
        "True when using a multi-configuration generator")
    unset(isMultiConfig)

endmacro()
