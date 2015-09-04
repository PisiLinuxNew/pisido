#include "application.h"
#include <QDir>
#include <QLocale>
#include <QTranslator>
#include <QDebug>

#include "mainwindow.h"

void load_translation(QApplication * a, QTranslator * translator);



int main(int argc, char *argv[])
{
    Application a(argc, argv);

    // set Application information
    a.setOrganizationName(QString("pisido"));
    a.setOrganizationDomain(QString("huseyinkozan.com.tr"));
    a.setApplicationName(QString("PiSiDo"));
    a.setApplicationVersion(QString("%1").arg(PISIDO_VERSION));

    QTranslator t;
    load_translation(&a, &t);

    MainWindow w;
    w.show();

    return a.exec();
}



void load_translation(QApplication * application, QTranslator * translator)
{
    // do translation bussiness
    QSettings settings;
    settings.beginGroup("configuration");
    QString lang_code = settings.value("language", QString()).toString();
    settings.endGroup();
    if(lang_code.isEmpty())
        lang_code = QLocale::system().name();
    QDir lang_dir(PISIDO_LANG_DIR);
    QString lang_file = lang_dir.absoluteFilePath(QString("pisido_%1.qm").arg(lang_code));
    if( ! QFile::exists(lang_file)){
        lang_file.replace(lang_code, QLocale().system().name());
    }
    if(QFile::exists(lang_file)){
        if(translator->load(lang_file)){
            application->installTranslator(translator);
        }
        else{
            qDebug() << QObject::tr("Translator failed to load : ") << lang_file;
        }
    }
    else{
//        qDebug() << QObject::tr("Translator file does not exists : ") << lang_file;
    }
}
