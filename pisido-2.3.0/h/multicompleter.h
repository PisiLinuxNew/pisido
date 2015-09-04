/*

Original code got from :
http://www.mimec.org/node/304
"QCompleter with multi-selection" article from Michał Męciński's blog.

*/

#ifndef MULTICOMPLETER_H
#define MULTICOMPLETER_H


#ifdef USING_QT5
    #include <QtWidgets/QCompleter>
    #include <QtWidgets/QLineEdit>
#else
    #include <QCompleter>
    #include <QLineEdit>
#endif

class MultiCompleter : public QCompleter
{
    Q_OBJECT
public:
    MultiCompleter( const QStringList& items, QObject* parent )
        : QCompleter( items, parent ) {}
    ~MultiCompleter() {}

public:
    QString pathFromIndex( const QModelIndex& index ) const
    {
        QString path = QCompleter::pathFromIndex( index );

        QString text = static_cast<QLineEdit*>( widget() )->text();

        int pos = text.lastIndexOf( ',' );
        if ( pos >= 0 )
            path = text.left( pos ) + ", " + path;

        return path;
    }
    QStringList splitPath( const QString& path ) const
    {
        int pos = path.lastIndexOf( ',' ) + 1;

        while ( pos < path.length() && path.at( pos ) == QLatin1Char( ' ' ) )
            pos++;

        return QStringList( path.mid( pos ) );
    }
};

#endif // MULTICOMPLETER_H
