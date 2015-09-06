#ifndef PISI_H
#define PISI_H

#include "pisisource.h"
#include "pisipackage.h"
#include "pisiupdate.h"
#include <QMap>

class QDomDocument;

class Pisi
{
public:
    Pisi();

    int packages_count();

    void clear();
    bool is_empty() const;

    void load_from_dom(const QDomDocument & dom) throw(QString);
    bool save_to_dom(QDomDocument & dom) throw(QString);

    PisiSource get_source() const;
    PisiPackage get_package(int value=1);// const;
    QMap<int, PisiUpdate> get_updates() const;
    PisiUpdate get_last_update() const;

    void set_source(PisiSource source) throw(QString);
    void set_package(int key,PisiPackage package) throw(QString);
    void set_updates(QMap<int, PisiUpdate> updates) throw(QString);
    void remove_package(int key);


private:
    bool empty;

    PisiSource source;
    PisiPackage package;
    QMap<int, PisiPackage> packages;
    QMap<int, PisiUpdate> updates;
    PisiUpdate last_update;

};

#endif // PISI_H
