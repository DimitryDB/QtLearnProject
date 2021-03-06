#-------------------------------------------------
#
# Project created by QtCreator 2017-05-16T15:49:19
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = store
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES +=  application.cpp \
            books.cpp \
            catalogue.cpp \
            catitemedit.cpp \
            confdialog.cpp \
            doalogtpl.cpp \
            main.cpp \
            mainwindow.cpp \
            pasreq.cpp \
            posaction.cpp \
    filter.cpp









HEADERS  += application.h \
            books.h      \
            catalogue.h  \
            catitemedit.h \
            confdialog.h \
            dialogtpl.h \
            mainwindow.h \
            pasreq.h \
            posaction.h \
    filter.h






FORMS +=   buttonsFrame.ui \
           CatItemFrame.ui \
           filter.ui \
           pasreq.ui


