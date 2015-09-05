#ifndef APPLICATION_H
#define APPLICATION_H

#ifdef USING_QT5
    #include <QtWidgets/QApplication>
#else
    #include <QApplication>
#endif

class Application : public QApplication
{
    Q_OBJECT
public:
    explicit Application( int & argc, char ** argv );
    virtual bool notify ( QObject * receiver, QEvent * event );

signals:

public slots:

};

#endif // APPLICATION_H
