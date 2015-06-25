TEMPLATE = app
TARGET = cppxslt
QT += testlib
QT -= gui
CONFIG += ordered testcase
INCLUDEPATH += ../..
QMAKE_CXXFLAGS = -std=c++11

# Input
#HEADERS += ../../XmlWriter.h \
#    ../../LiteralXml.h
SOURCES += testserializer.cpp
