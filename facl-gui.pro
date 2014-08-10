#-------------------------------------------------
#
# Project created by QtCreator 2011-11-18T21:35:35
#
#-------------------------------------------------

QT      += core gui

TARGET = facl-gui
TEMPLATE = app

SOURCES += main.cpp\
        FaclGui.cpp \
    model/AclTableModel.cpp \
    delegate/CheckBoxStyledDelegate.cpp \
    dialog/PreferencesDialog.cpp \
    dialog/AddDialog.cpp

HEADERS += FaclGui.h \
    FaclGui_p.h \
    model/AclTableModel.h \
    model/AclTableModel_p.h \
    delegate/CheckBoxStyledDelegate.h \
    dialog/PreferencesDialog.h \
    dialog/AddDialog.h \
    dialog/AddDialog_p.h

FORMS   += FaclGui.ui \
    dialog/PreferencesDialog.ui \
    dialog/AddDialog.ui

LIBS    += -lacl

RESOURCES += \
    resources/resources.qrc

TRANSLATIONS += \
    resources/translation/facl-gui_it.ts

OTHER_FILES += \
    resources/translation/facl-gui_it.qm
























