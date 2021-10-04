# Separated models, to simply include them in TinyORM tests
include(models/models.pri)

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/common.hpp \
    $$PWD/testorm.hpp \
    $$PWD/version.hpp \

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/testorm.cpp \
