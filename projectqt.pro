QT       += sql printsupport
QT += charts
QT += core gui serialport
QT += network
QT += core gui sql printsupport
QT       += core gui sql charts
QT += charts
QT += core gui sql charts printsupport
QT += printsupport
QT += serialport
QT += widgets
QT += core gui serialport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

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
    arduino.cpp \
    arduinoo.cpp \
    barchartwidget.cpp \
    commande.cpp \
    connection.cpp \
    employe.cpp \
    lacommande.cpp \
    main.cpp \
    mainwindow.cpp \
    mainwindow2.cpp \
    mainwindoww3.cpp \
    produit.cpp \
    serialcommunication.cpp \
    services.cpp

HEADERS += \
    arduino.h \
    arduino.h \
    arduinoo.h \
    barchartwidget.h \
    commande.h \
    connection.h \
    employe.h \
    lacommande.h \
    mainwindow.h \
    mainwindow2.h \
    mainwindoww3.h \
    produit.h \
    serialcommunication.h \
    services.h
FORMS += \
    lacommande.ui \
    mainwindow.ui \
    mainwindow2.ui \
    mainwindoww3.ui  # Assurez-vous que mainwindoww3.ui est bien mentionné ici



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    background.qrc

DISTFILES +=

STATECHARTS +=
