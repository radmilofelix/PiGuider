#-------------------------------------------------
#
# Project created by QtCreator 2018-02-26T23:46:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PiGuider
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


SOURCES += \
    main.cpp \
    piguider.cpp \
    guider.cpp \
    dslr.cpp \
    intervalometerhard.cpp \
    intervalometersoft.cpp \
    imagelabel.cpp \
    test.cpp \
    pincontrolpi.cpp \
    wiringpiss.cpp \
    displaygeometry.cpp \
    numpad.cpp \
    dslrcameracontrol.cpp \
    loadsaveandsettings.cpp

HEADERS += \
    piguider.h \
    guider.h \
    dslr.h \
    intervalometerhard.h \
    intervalometersoft.h \
    globalsettings.h \
    imagelabel.h \
    test.h \
    pincontrolpi.h \
    wiringpiss.h \
    displaygeometry.h \
    numpad.h \
    dslrcameracontrol.h \
    loadsaveandsettings.h \
    capture-00-Miscellaneous.h \
    capture-01-Arcturus-CNE-CWC3.h \
    capture-02-Arcturus-CNE-HWC1.h \
    capture-03-Arcturus-Trust-16429.h \
    capture-04-Jupiter-CNE-HWC1.h \
    capture-05-Jupiter-Trust-16429.h \
    capture-06-Polaris-CNE-CWC3.h \
    capture-07-Polaris-Trust-16429.h \
    capture-08-Alkaid-CNE-CWC3.h \
    capture-09-Alkaid-CNE-CWC3.h \
    capture-10-Arcturus-CNE-CWC3.h \
    capture-11-Vega-CNE-CWC3.h

FORMS += \
    piguider.ui \
    guider.ui \
    dslr.ui \
    intervalometerhard.ui \
    intervalometersoft.ui \
    numpad.ui \
    loadsaveandsettings.ui

RESOURCES += \
    icons.qrc

# Comment out if building for raspbian
CONFIG += RUNONPC

RUNONPC {
# Ubuntu
    LIBS += `pkg-config opencv --libs`
    LIBS += -lgphoto2 -lgphoto2_port
} else {
    HOMEDIR = /home/rmf
    INCLUDEPATH += $$HOMEDIR/raspi/cross_built/local/include
    LIBS += `pkg-config $$HOMEDIR/raspi/cross_built/local/lib/pkgconfig/opencv.pc --libs`
    LIBS += -L $$HOMEDIR/raspi/cross_built/local/lib -ltbb -lgphoto2 -lgphoto2_port -lwiringPi -lcrypt
    target.path = /home/pi
    INSTALLS = target
}
