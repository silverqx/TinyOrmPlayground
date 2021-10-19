# TinyOrmPlayground header and source files
# Create header and source files lists and return them
function(tiny_sources out_headers out_sources)

    set(headers
        common.hpp
        config.hpp
        testorm.hpp
        version.hpp
    )

    set(sources
        main.cpp
        testorm.cpp
    )

    list(TRANSFORM headers PREPEND "src/")
    list(TRANSFORM sources PREPEND "src/")

    set(${out_headers} ${headers} PARENT_SCOPE)
    set(${out_sources} ${sources} PARENT_SCOPE)

endfunction()
