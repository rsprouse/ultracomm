TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += \
    StdAfx.cpp \
    main.cpp

HEADERS += \
    StdAfx.h

INCLUDEPATH += ../../inc/
INCLUDEPATH += ../../../

LIBS += -L"../../lib" -lulterius_old
LIBS += -L"../../../bin" -lulterius_old
