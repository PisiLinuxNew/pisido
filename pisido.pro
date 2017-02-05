#-------------------------------------------------
#
# Project created by QtCreator 2011-04-10T13:32:53
#
#-------------------------------------------------

QT       += core gui xml network
CONFIG   += exceptions warn_on precompile_header qscintilla2

greaterThan(QT_MAJOR_VERSION, 4){
    QT += widgets
    DEFINES += USING_QT5=\\\"true\\\"
    LIBS += -lqtermwidget5
}else{
    LIBS += -lqtermwidget4
}

TARGET = pisido
TEMPLATE = app
LANGUAGE = C++

VERSION = 2.3.5
DEFINES += PISIDO_VERSION=\\\"$$VERSION\\\"
INCLUDEPATH += ./h


PISIDO_BIN_ROOT = /usr/bin
PISIDO_FILES_ROOT = /usr/share/pisido
PISIDO_DOC_ROOT = /usr/share/doc/pisido
PIXMAP_ROOT = /usr/share/pixmaps
DESKTOP_ROOT = /usr/share/applications


bin_target.path = $$PISIDO_BIN_ROOT
bin_target.files += $$TARGET
INSTALLS += bin_target

translation_target.path = $$PISIDO_FILES_ROOT
translation_target.files += translations/pisido_tr_TR.qm
translation_target.files += translations/pisido_en_US.qm
translation_target.files += translations/pisido_de_DE.qm
INSTALLS += translation_target

help_target.path = $$PISIDO_DOC_ROOT
help_target.files += help/help_tr_TR.pdf
help_target.files += help/help_en_US.pdf
help_target.files += help/help_de_DE.pdf
INSTALLS += help_target

pixmap_target.path = $$PIXMAP_ROOT
pixmap_target.files += resources/images/pisido.png
INSTALLS += pixmap_target

desktop_target.path = $$DESKTOP_ROOT
desktop_target.files += resources/files/pisido.desktop
INSTALLS += desktop_target


# program will scan this dir for *.qm translation files.
# each file name must be like this : pisido_xx_XX.qm
# xx must be a valid language_COUNTRY pair, see QLocale doc.
# for example : pisido_tr_TR.qm , pisido_fr_FR.qm , ...
# same as translations; add each help file like this:
# help_tr_TR.pdf, help_en_US.pdf, ...

CONFIG(debug, debug|release){
    DEFINES += PISIDO_LANG_DIR=\\\"$$PWD/translations/\\\"
    DEFINES += PISIDO_HELP_DIR=\\\"$$PWD/help/\\\"
}
CONFIG(release, debug|release){
    DEFINES += PISIDO_LANG_DIR=\\\"$$PISIDO_FILES_ROOT/\\\"
    DEFINES += PISIDO_HELP_DIR=\\\"$$PISIDO_DOC_ROOT/\\\"
}


# Use Precompiled headers (PCHs)
PRECOMPILED_HEADER = h/pchs.h

MOC_DIR = generated/moc
OBJECTS_DIR = generated/objects
RCC_DIR = generated/resources
UI_DIR = generated/ui

SOURCES += \
    cpp/main.cpp\
    cpp/application.cpp \
    cpp/mainwindow.cpp \
    cpp/configurationdialog.cpp \
    cpp/pisispbase.cpp \
    cpp/pisi.cpp \
    cpp/pisipackage.cpp \
    cpp/pisisource.cpp \
    cpp/pisiupdate.cpp \
    cpp/addupdatedialog.cpp \
    cpp/languagedialog.cpp \
    cpp/workspacedialog.cpp \
    cpp/aditionalfiledialog.cpp \
    cpp/addinstallfilelabeldialog.cpp \
    cpp/archiveselectiondialog.cpp \
    cpp/archivewidget.cpp \
    cpp/aboutdialog.cpp \
    cpp/directorymodel.cpp \
    cpp/translationwidget.cpp \
    cpp/removecontainersdialog.cpp

HEADERS += \
    h/pchs.h \
    h/mainwindow.h \
    h/application.h \
    h/configurationdialog.h \
    h/pisispbase.h \
    h/pisi.h \
    h/pisipackage.h \
    h/pisisource.h \
    h/pisiupdate.h \
    h/addupdatedialog.h \
    h/languagedialog.h \
    h/workspacedialog.h \
    h/aditionalfiledialog.h \
    h/addinstallfilelabeldialog.h \
    h/archiveselectiondialog.h \
    h/archivewidget.h \
    h/aboutdialog.h \
    h/multicompleter.h \
    h/directorymodel.h \
    h/translationwidget.h \
    h/removecontainersdialog.h

FORMS += \
    ui/mainwindow.ui \
    ui/configurationdialog.ui \
    ui/addupdatedialog.ui \
    ui/languagedialog.ui \
    ui/workspacedialog.ui \
    ui/aditionalfiledialog.ui \
    ui/addinstallfilelabeldialog.ui \
    ui/archiveselectiondialog.ui \
    ui/archivewidget.ui \
    ui/aboutdialog.ui \
    ui/translationwidget.ui \
    ui/removecontainersdialog.ui

TRANSLATIONS += \
    translations/pisido_en_US.ts \
    translations/pisido_tr_TR.ts \
    translations/pisido_de_DE.ts

OTHER_FILES += \
    README \
    OKUBUNU \
    LICENSE \
    LISANS \
    TODO \
    YAPILACAKLAR \
    help/help_tr_TR.pdf \
    help/help_tr_TR.odt \
    help/help_en_US.pdf \
    help/help_en_US.odt \
    help/help_de_DE.pdf \
    help/help_de_DE.odt \
    resources/files/actions_template_auto.py \
    resources/files/actions_template_cmake.py \
    resources/files/actions_template_java.py \
    resources/files/actions_template_kde4.py \
    resources/files/actions_template_kde5.py \
    resources/files/actions_template_python.py \
    resources/files/actions_template_qt4.py \
    resources/files/actions_template_scons.py \
    resources/files/actions_template_qt5.py

RESOURCES += \
    resources/pisido.qrc
