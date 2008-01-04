SOURCES += main.cpp \
 contour.cpp
TEMPLATE = app
CONFIG += warn_on \
          thread \
          qt
TARGET = ../bin/contourtest

QT += gui
HEADERS += contour.h

LIBS += -lsvl

