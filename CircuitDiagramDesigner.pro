QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    circuitdiagrameditor.cpp \
    electroniccomponent.cpp \
    junction.cpp \
    main.cpp \
    mainwindow.cpp \
    previewwire.cpp \
    wire.cpp

HEADERS += \
    circuitdiagrameditor.h \
    electroniccomponent.h \
    junction.h \
    mainwindow.h \
    parameters.h \
    previewwire.h \
    wire.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    CircuitDiagramDesigner_ru_RU.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
