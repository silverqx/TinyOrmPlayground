# Separated models, to simply include them in TinyORM tests
include(models/models.pri)

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/concerns/logsquerycounters.hpp \
    $$PWD/config.hpp \
    $$PWD/configuration.hpp \
    $$PWD/macros.hpp \
    $$PWD/services/configurationsservice.hpp \
    $$PWD/services/querycountersservice.hpp \
    $$PWD/services/service.hpp \
    $$PWD/support/globals.hpp \
    $$PWD/support/messagehandler.hpp \
    $$PWD/support/utils.hpp \
    $$PWD/testorm.hpp \
    $$PWD/tests/test.hpp \
    $$PWD/tests/testforplay.hpp \
    $$PWD/version.hpp \
    # Moved down to be able quicky comment them out
    $$PWD/tests/testallothertests.hpp \
    $$PWD/tests/testconnection.hpp \
    $$PWD/tests/testquerybuilder.hpp \
    $$PWD/tests/testquerybuilderdbspecific.hpp \
    $$PWD/tests/testschemabuilder.hpp \
    $$PWD/tests/testtinyorm.hpp \

SOURCES += \
    $$PWD/concerns/logsquerycounters.cpp \
    $$PWD/configuration.cpp \
    $$PWD/main.cpp \
    $$PWD/services/configurationsservice.cpp \
    $$PWD/services/querycountersservice.cpp \
    $$PWD/services/service.cpp \
    $$PWD/support/messagehandler.cpp \
    $$PWD/support/utils.cpp \
    $$PWD/testorm.cpp \
    $$PWD/tests/test.cpp \
    $$PWD/tests/testforplay.cpp \
    # Moved down to be able quicky comment them out
    $$PWD/tests/testallothertests.cpp \
    $$PWD/tests/testconnection.cpp \
    $$PWD/tests/testquerybuilder.cpp \
    $$PWD/tests/testquerybuilderdbspecific.cpp \
    $$PWD/tests/testschemabuilder.cpp \
    $$PWD/tests/testtinyorm.cpp \
