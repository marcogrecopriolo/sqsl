QT       += core gui

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET   = qtdemo
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += qtdemo.cpp csqmc.c

HEADERS += qtdemo.h csqmc.c

FORMS += qtdemo.ui

RESOURCES += resources.qrc

INCLUDEPATH += ../sqsl ../oslib /opt/informix/11.50.FC7/incl/esql
LIBPATH += /opt/informix/11.50.FC7/lib /opt/informix/11.50.FC7/lib/esql
DEFINES += ESQLVER=350
LIBS += ../sqsl/libsqsl.a -lifsql -lifasf -lifgen -lifos -lifgls -lpthread -lm -ldl -lcrypt /opt/informix/11.50.FC7/lib/esql/checkapi.o -lifglx
