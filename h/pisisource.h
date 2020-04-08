#ifndef PISISOURCE_H
#define PISISOURCE_H

#include <QString>
#include <QMap>

#include "pisispbase.h"

class QDomElement;

class PisiSource : public PisiSPBase
{
public:
    PisiSource();

    enum ArchiveAttr {
        SHA1SUM,
        TYPE,
        TARGET_ARCHIVE
    };

    enum PatchAttr {
        COMPRESSIONTYPE,
        LEVEL,
        TARGET_PATCH,
        REVERSE
    };

    virtual void clear();
    virtual void load_from_dom(const QDomElement & dom_element) throw(QString);
    virtual void save_to_dom(QDomElement & root) throw(QString);

    QString get_home_page() const;
    QMap<QString,QString> get_packager() const;
    QMap<QString, QMap<ArchiveAttr,QString> > get_archives() const;
    QMap<QString, QMap<PatchAttr,QString> > get_patches() const;

    static QString get_archive_type(const QString & file_name);
    static QStringList get_archive_type_list();

    QString get_Icon() const;

    void set_home_page(QString home_page) throw(QString);
    void set_packager(QString name, QString email) throw(QString);
    void set_archives(QMap<QString, QMap<ArchiveAttr,QString> > archives) throw(QString);
    void set_patches(QMap<QString, QMap<PatchAttr,QString> > patches);

    void set_Icon(QString Icon);

    bool operator ==(const PisiSource & other);
    bool operator !=(const PisiSource & other);

protected:
    virtual bool is_mandatory(QDomElement root, QString tag) throw(QString);

private:
    ArchiveAttr get_archive_attribute(QString attr_name) throw(QString);
    QString get_archive_attribute(ArchiveAttr attr) throw(QString);

    PatchAttr get_patch_attribute(QString attr_name) throw(QString);
    QString get_patch_attribute(PatchAttr attr) throw(QString);

private:
    QString home_page,Icon;
    QMap<QString,QString> packager;                         // name=ali veli, email=q@q.com
    QMap<QString, QMap<ArchiveAttr,QString> > archives;     // archive, {(sha1sum, xxx),(type, xxx),(target, xxx)}
    QMap<QString, QMap<PatchAttr,QString> > patches;      // patch, {(compty, xxx), (level, xxx), (target, xxx), (reverse, xxx)}
};

#endif // PISISOURCE_H
