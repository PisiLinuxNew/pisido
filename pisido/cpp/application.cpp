#include "application.h"
#include <QDebug>

Application::Application( int & argc, char ** argv ):
    QApplication(argc, argv)
{
}

bool Application::notify(QObject *receiver, QEvent *event)
{
    try{
        return QApplication::notify(receiver, event);
    }catch(char const * str){
        qDebug() << "Exception : " << str;
        return false;
    }catch(...){
        qDebug() << "Unknown exception !";
        abort();
    }
}
