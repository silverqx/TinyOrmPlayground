# TinyOrmPlayground header and source files
# Create header and source files lists and return them
function(tiny_sources out_headers out_sources)

    set(headers
        config.hpp
        configuration.hpp
        configurationsservice.hpp
        connectionsservice.hpp
        macros.hpp
        support/globals.hpp
        support/messagehandler.hpp
        support/utils.hpp
        testorm.hpp
        tests/test.hpp
        tests/testallothertests.hpp
        tests/testconnection.hpp
        tests/testforplay.hpp
        tests/testquerybuilder.hpp
        tests/testquerybuilderdbspecific.hpp
        tests/testtinyorm.hpp
        version.hpp
    )

    set(sources
        configuration.cpp
        configurationsservice.cpp
        connectionsservice.cpp
        main.cpp
        support/messagehandler.cpp
        support/utils.cpp
        testorm.cpp
        tests/test.cpp
        tests/testallothertests.cpp
        tests/testconnection.cpp
        tests/testforplay.cpp
        tests/testquerybuilder.cpp
        tests/testquerybuilderdbspecific.cpp
        tests/testtinyorm.cpp
    )

    list(TRANSFORM headers PREPEND "src/")
    list(TRANSFORM sources PREPEND "src/")

    set(${out_headers} ${headers} PARENT_SCOPE)
    set(${out_sources} ${sources} PARENT_SCOPE)

endfunction()
