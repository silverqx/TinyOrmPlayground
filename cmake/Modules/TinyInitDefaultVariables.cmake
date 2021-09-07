include(TinyHelpers)

# Initialize CMake default variables by project options
macro(tiny_init_cmake_variables)

    set(CMAKE_FIND_PACKAGE_SORT_ORDER NATURAL)
    set(CMAKE_FIND_PACKAGE_SORT_DIRECTION DEC)
    set(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION YES)

    tiny_invert_bool(PRECOMPILE_HEADERS ${PRECOMPILE_HEADERS})
    set(CMAKE_DISABLE_PRECOMPILE_HEADERS ${PRECOMPILE_HEADERS})

    if(MSVC_RUNTIME_DYNAMIC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()

    # Fix install prefix for the x64 toolchain
    if(WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT
            AND CMAKE_SIZEOF_VOID_P EQUAL 8
    )
        get_property(intall_prefix_docs CACHE CMAKE_INSTALL_PREFIX PROPERTY HELPSTRING)

        set(CMAKE_INSTALL_PREFIX "C:/Program Files/${PROJECT_NAME}"
            CACHE PATH "${intall_prefix_docs}" FORCE
        )
    endif()

endmacro()
