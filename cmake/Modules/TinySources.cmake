# TinyOrmPlayground header and source files
# Create header and source files lists and return them
function(tiny_sources out_headers out_sources)

    set(headers
        concerns/logsquerycounters.hpp
        config.hpp
        configuration.hpp
        macros.hpp
        services/configurationsservice.hpp
        services/querycountersservice.hpp
        services/service.hpp
        support/globals.hpp
        support/messagehandler.hpp
        support/utils.hpp
        testorm.hpp
        tests/test.hpp
        tests/testforplay.hpp
        version.hpp
        # Moved down to be able quicky comment them out
#        tests/testallothertests.hpp
#        tests/testconnection.hpp
#        tests/testquerybuilder.hpp
#        tests/testquerybuilderdbspecific.hpp
#        tests/testschemabuilder.hpp
#        tests/testtinyorm.hpp
    )

    set(sources
        concerns/logsquerycounters.cpp
        configuration.cpp
        main.cpp
        services/configurationsservice.cpp
        services/querycountersservice.cpp
        services/service.cpp
        support/messagehandler.cpp
        support/utils.cpp
        testorm.cpp
        tests/test.cpp
        tests/testforplay.cpp
        # Moved down to be able quicky comment them out
#        tests/testallothertests.cpp
#        tests/testconnection.cpp
#        tests/testquerybuilder.cpp
#        tests/testquerybuilderdbspecific.cpp
#        tests/testschemabuilder.cpp
#        tests/testtinyorm.cpp
    )

    list(SORT headers)
    list(SORT sources)

    set(sourceDir "${${TinyOrmPlayground_ns}_SOURCE_DIR}/src/")

    list(TRANSFORM headers PREPEND "${sourceDir}")
    list(TRANSFORM sources PREPEND "${sourceDir}")

    set(${out_headers} ${headers} PARENT_SCOPE)
    set(${out_sources} ${sources} PARENT_SCOPE)

endfunction()
