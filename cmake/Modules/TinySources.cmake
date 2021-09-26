# TinyOrmPlayground header and source files
# Create header and source files lists and return them
function(tiny_sources)

    set(headers
        common.hpp
        testorm.hpp
        version.hpp.in
    )

    set(sources
        main.cpp
        testorm.cpp
    )

    list(TRANSFORM headers PREPEND "src/")
    list(TRANSFORM sources PREPEND "src/")

    set(headers ${headers} PARENT_SCOPE)
    set(sources ${sources} PARENT_SCOPE)

endfunction()
