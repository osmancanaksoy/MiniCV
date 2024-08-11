QT       += core gui
QT += charts
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bmpheader.cpp \
    bmpreader.cpp \
    bmpwriter.cpp \
    image.cpp \
    imio.cpp \
    main.cpp \
    mainwindow.cpp \
    mat.cpp \
    matrix.cpp \
    object.cpp \
    pixel.cpp \
    point.cpp \
    shape.cpp

HEADERS += \
    bmpheader.h \
    bmpreader.h \
    bmpwriter.h \
    image.h \
    imio.h \
    mainwindow.h \
    mat.h \
    matrix.h \
    object.h \
    pixel.h \
    point.h \
    shape.h \
    utils.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc

RC_ICONS = icon.ico
