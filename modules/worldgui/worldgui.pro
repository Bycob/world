#-------------------------------------------------
#
# Project created by QtCreator 2017-03-13T00:12:31
#
#-------------------------------------------------

QT       += core gui 3dextras 3drender

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = worldgui
TEMPLATE = app

CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += "$${WORLDAPI_SOURCES}/modules" \
                "$${WORLDAPI_SOURCES}/includes" \
                "$${WORLDAPI_SOURCES}/includes/armadillo" \
                "$${WORLDAPI_SOURCES}/includes/opencv"

LIBS    += -L$${WORLDAPI_LIBDIR} -lworldapi

SOURCES += main.cpp\
    mainwindow.cpp \
    panelterrain.cpp \
    generatepanel.cpp \
    scene.cpp \
    qtworld.cpp \
    previewpanel3d.cpp \
    panelworldmap.cpp

HEADERS  += mainwindow.h \
    panelterrain.h \
    generatepanel.h \
    scene.h \
    qtworld.h \
    previewpanel3d.h \
    panelworldmap.h

FORMS    += mainwindow.ui \
    panelterrain.ui \
    panelworldmap.ui

RESOURCES += \
    icons.qrc
