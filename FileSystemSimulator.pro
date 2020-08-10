QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
QMAKE_CFLAGS = -static
QMAKE_LFLAGS += -static

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dialogopenfile.cpp \
    dialogsearch.cpp \
    file.cpp \
    group.cpp \
    info.cpp \
    main.cpp \
    physicalmedium.cpp \
    simulationdisk.cpp \
    systemlogin.cpp \
    systemmain.cpp \
    user.cpp \
    usermain.cpp \
    utility.cpp

HEADERS += \
    dialogopenfile.h \
    dialogsearch.h \
    file.h \
    group.h \
    info.h \
    physicalmedium.h \
    simulationdisk.h \
    systemlogin.h \
    systemmain.h \
    user.h \
    usermain.h \
    utility.h

FORMS += \
    dialogopenfile.ui \
    dialogsearch.ui \
    systemlogin.ui \
    systemmain.ui \
    usermain.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

RC_ICONS = FileSystemSimulator.ico
