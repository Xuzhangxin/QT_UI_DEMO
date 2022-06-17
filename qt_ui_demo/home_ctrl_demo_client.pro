QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ./src
INCLUDEPATH += ./include

SOURCES += \
    src/roomlist.cpp \
    src/scenelist.cpp \
    src/devicelist.cpp \
    src/dpmanager.cpp \
    src/functionswitch.cpp \
    src/loginwidget.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/requestsender.cpp \
    src/voiceupload.cpp

HEADERS += \
    include/comminclude.h \
    include/devicelist.h \
    include/dpmanager.h \
    include/functionswitch.h \
    include/loginwidget.h \
    include/mainwindow.h \
    include/requestsender.h \
    include/voiceupload.h \
    include/scenelist.h \
    include/roomlist.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
