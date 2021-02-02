# Separated models, to simply include them in TinyORM tests
include(models/models.pri)

INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/testorm.cpp \

HEADERS += \
    $$PWD/common.hpp \
    $$PWD/testorm.hpp \
