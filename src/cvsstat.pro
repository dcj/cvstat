# #####################################################################
# Automatically generated by qmake (2.01a) ?? 7? 30 17:31:14 2009
# #####################################################################
TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += cvstat.h \
    mainframe.h \
    newwizard.h \
    waitupdatedialog.h \
    queryextoolsdialog.h \
    sqlhighlighter.h \
    queryreportdialog.h
FORMS += mainframe.ui \
    newwizard.ui \
    waitupdatedialog.ui \
    queryextoolsdialog.ui \
    queryreportdialog.ui
SOURCES += cvstat.cpp \
    main.cpp \
    mainframe.cpp \
    newwizard.cpp \
    waitupdatedialog.cpp \
    queryextoolsdialog.cpp \
    sqlhighlighter.cpp \
    queryreportdialog.cpp
#LIBS += -lsqlite3
RESOURCES += mainframe.qrc
QT += sql xml
TRANSLATIONS=zh_cn.ts