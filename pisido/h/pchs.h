#ifndef PCHS_H
#define PCHS_H


/* Add C includes here */

#if defined __cplusplus
/* Add C++ includes here */

#ifdef USING_QT5
    #include <QtWidgets/QApplication>
    #include <QtWidgets/QDialog>
    #include <QtWidgets/QFileDialog>
    #include <QtWidgets/QMessageBox>
    #include <QtWidgets/QShortcut>
    #include <QtWidgets/QFileSystemModel>
    #include <QtWidgets/QMainWindow>
    #include <qt5/Qsci/qsciscintilla.h>
    #include <qt5/Qsci/qscilexerpython.h>
    #include <qtermwidget5/qtermwidget.h>
#else
    #include <QApplication>
    #include <QDialog>
    #include <QFileDialog>
    #include <QMessageBox>
    #include <QShortcut>
    #include <QFileSystemModel>
    #include <QMainWindow>
    #include <Qsci/qsciscintilla.h>
    #include <Qsci/qscilexerpython.h>
    #include <qtermwidget4/qtermwidget.h>
#endif


#include <QDebug>
#include <QCloseEvent>
#include <QDate>
#include <QDesktopServices>
#include <QProcess>
#include <QUrl>
#include <QCryptographicHash>
#include <QTimer>
#include <QFileSystemWatcher>
#include <QStandardItemModel>
#include <QDirIterator>
#include <QDomDocument>
#include <QSettings>
#include <QMap>
#include <QMultiMap>
#include <QDir>




#endif


#endif // PCHS_H
