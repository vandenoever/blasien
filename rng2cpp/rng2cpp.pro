TEMPLATE = app
QT += xml
QT -= gui
INCLUDEPATH += ..
QMAKE_CXXFLAGS = -std=c++11

SOURCES += rng2cpp.cpp \
    rng2cpp2.cpp \
    testrng.cpp

HEADERS += \
    rngtest.h
