# Separated models, to simply include them in TinyORM tests
include(models/models.pri)

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/config.hpp \
    $$PWD/macros.hpp \
    $$PWD/support/globals.hpp \
    $$PWD/support/messagehandler.hpp \
    $$PWD/support/utils.hpp \
    $$PWD/testorm.hpp \
    $$PWD/version.hpp \

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/support/messagehandler.cpp \
    $$PWD/support/utils.cpp \
    $$PWD/testorm.cpp \
