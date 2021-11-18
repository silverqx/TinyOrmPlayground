# Separated models, to simply include them in TinyORM tests
include(models/models.pri)

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/config.hpp \
    $$PWD/configuration.hpp \
    $$PWD/configurationsservice.hpp \
    $$PWD/connectionsservice.hpp \
    $$PWD/macros.hpp \
    $$PWD/support/globals.hpp \
    $$PWD/support/messagehandler.hpp \
    $$PWD/support/utils.hpp \
    $$PWD/testorm.hpp \
    $$PWD/tests/test.hpp \
    $$PWD/tests/testallothertests.hpp \
    $$PWD/tests/testconnection.hpp \
    $$PWD/tests/testforplay.hpp \
    $$PWD/tests/testquerybuilder.hpp \
    $$PWD/tests/testquerybuilderdbspecific.hpp \
    $$PWD/tests/testtinyorm.hpp \
    $$PWD/version.hpp \

SOURCES += \
    $$PWD/configuration.cpp \
    $$PWD/configurationsservice.cpp \
    $$PWD/connectionsservice.cpp \
    $$PWD/main.cpp \
    $$PWD/support/messagehandler.cpp \
    $$PWD/support/utils.cpp \
    $$PWD/testorm.cpp \
    $$PWD/tests/test.cpp \
    $$PWD/tests/testallothertests.cpp \
    $$PWD/tests/testconnection.cpp \
    $$PWD/tests/testforplay.cpp \
    $$PWD/tests/testquerybuilder.cpp \
    $$PWD/tests/testquerybuilderdbspecific.cpp \
    $$PWD/tests/testtinyorm.cpp \
