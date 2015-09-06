#include "pisisource.h"

#include <QDomElement>
#include <QDebug>

PisiSource::PisiSource()
    : PisiSPBase()
{
    clear();
}

void PisiSource::clear()
{
    PisiSPBase::clear();
    home_page.clear();
    packager.clear();
    archives.clear();
}

void PisiSource::load_from_dom(const QDomElement & dom_element) throw(QString)
{
    PisiSPBase::load_from_dom(dom_element);

    if(dom_element.isNull())
        throw QObject::tr("Dom Element is null while loading to PisiSource !");

    QDomElement elm = dom_element.firstChildElement("Homepage");
    if(elm.isNull()) throw QObject::tr("No Homepage tag !");
    home_page = elm.text();

    elm = dom_element.firstChildElement("Packager");
    if(elm.isNull()) throw QObject::tr("No Packager tag !");

    elm = elm.firstChildElement("Name");
    if(elm.isNull()) throw QObject::tr("No Packager::Name tag !");
    QString packager_name = elm.text();

    elm = elm.nextSiblingElement("Email");
    if(elm.isNull()) throw QObject::tr("No Packager::Email tag !");
    packager[packager_name] = elm.text();

    elm = dom_element.firstChildElement("Archive");
    if(elm.isNull()) throw QObject::tr("No Archive tag !");

    for( ; ! elm.isNull(); elm = elm.nextSiblingElement("Archive"))
    {
        QMap<ArchiveAttr,QString> attributes;
        QDomNamedNodeMap elm_node_map = elm.attributes();
        int count = elm_node_map.count();
        for(int i=0; i<count; ++i)
        {
            QDomNode n = elm_node_map.item(i);
            QDomAttr a = n.toAttr();
            if(a.isNull()) throw QObject::tr("Can not convert to attribute !");
            attributes[get_archive_attribute(a.name())] = a.value();
//                qDebug() << "ArchiveAttr = " << a.name() << " : " << a.value();
        }
        archives[elm.text()] = attributes;
    }

    elm = dom_element.firstChildElement("Patches");
    if( ! elm.isNull())
    {
        elm = dom_element.firstChildElement("Patch");
        if( ! elm.isNull())
        {
            for( ; ! elm.isNull(); elm = elm.nextSiblingElement("Patch"))
            {
                QMap<PatchAttr,QString> attributes;
                QDomNamedNodeMap elm_node_map = elm.attributes();
                int count = elm_node_map.count();
                for(int i=0; i<count; ++i)
                {
                    QDomNode n = elm_node_map.item(i);
                    QDomAttr a = n.toAttr();
                    if(a.isNull()) throw QObject::tr("Can not convert to attribute !");
                    attributes[get_patch_attribute(a.name())] = a.value();
                    //                qDebug() << "ArchiveAttr = " << a.name() << " : " << a.value();
                }
                patches[elm.text()] = attributes;
            }
        }
    }
}

void PisiSource::save_to_dom(QDomElement & root) throw(QString)
{
    PisiSPBase::save_to_dom(root);

    if(root.isNull())
        throw QObject::tr("Dom Element is null while saving from PisiSource to dom !");

    set_element_value(root, "Homepage", home_page, "Name");

    QDomElement elm = root.firstChildElement("Packager");
    if( ! elm.isNull())
        root.removeChild(elm);
    elm = insert_element_after(root, "Packager", "Homepage");

    QList<QString> packager_names = packager.keys();
    if(packager_names.count() > 1){
        throw QObject::tr("More than one packager info !");
    }
    else if(packager_names.count() < 1){
        throw QObject::tr("There is no packager info !");
    }
    else{
        set_element_value(elm, "Name", packager_names.first());
        set_element_value(elm, "Email", packager[packager_names.first()]);
    }

    elm = root.firstChildElement("Archive");
    while( ! elm.isNull()){
        root.removeChild(elm);
        elm = root.firstChildElement("Archive");
    }

    if(archives.count() < 1){
        throw QObject::tr("There is no archive !");
    }
    QList<QString> archive_keys = archives.keys();
    foreach (QString archive_path, archive_keys) {
        elm = append_element(root, "Archive");
        append_text_element(elm, archive_path);
        if( ! elm.isNull()){
            QList<ArchiveAttr> attributes = archives[archive_path].keys();
            foreach (ArchiveAttr attr, attributes){
                elm.setAttribute(get_archive_attribute(attr), archives[archive_path][attr]);
            }
        }
    }

    elm = root.firstChildElement("Patches");
    if( ! elm.isNull())
        root.removeChild(elm);

    QList<QString> patch_list = patches.keys();
    if(patch_list.count())
        elm = append_element(root, "Patches");
    for(int i=0; i<patch_list.count(); ++i)
    {
        QString patch = patch_list.at(i);
        QDomElement patch_elm = append_element(elm, "Patch");
        append_text_element(patch_elm, patch);

        QMap<PatchAttr,QString> attr = patches[patch];
        QList<PatchAttr> attributes = attr.keys();
        for(int j=0; j<attributes.count(); ++j)
        {
            PatchAttr a = attributes.at(j);
            QString a_value = attr[a];
            patch_elm.setAttribute(get_patch_attribute(a), a_value);
        }
    }
}

QString PisiSource::get_home_page() const
{
    return home_page;
}

QMap<QString,QString> PisiSource::get_packager() const
{
    return packager;
}

QMap<QString, QMap<PisiSource::ArchiveAttr,QString> > PisiSource::get_archives() const
{
    return archives;
}

QMap<QString, QMap<PisiSource::PatchAttr,QString> > PisiSource::get_patches() const
{
    return patches;
}

void PisiSource::set_home_page(QString home_page) throw(QString)
{
    if(home_page.isEmpty())
        throw QObject::tr("Homepage can not be empty !");

    this->home_page = home_page;
}

void PisiSource::set_packager(QString name, QString email) throw(QString)
{
    if(name.isEmpty())
        throw QObject::tr("Empty packager name !");
    if(email.isEmpty())
        throw QObject::tr("Empty packager email !");

    QMap<QString,QString> packager;
    packager[name] = email;
    this->packager = packager;
}

void PisiSource::set_archives(QMap<QString, QMap<PisiSource::ArchiveAttr, QString> > archives) throw(QString)
{
    if(archives.count() == 0)
        throw QObject::tr("Empty archive !");

    this->archives = archives;
}

void PisiSource::set_patches(QMap<QString, QMap<PatchAttr, QString> > patches)
{
    // optional, no check
    this->patches = patches;
}

PisiSource::ArchiveAttr PisiSource::get_archive_attribute(QString attr_name) throw(QString)
{
    if(attr_name.toLower() == "sha1sum")
        return SHA1SUM;
    else if(attr_name.toLower() == "type")
        return TYPE;
    else if(attr_name.toLower() == "target")
        return TARGET_ARCHIVE;
    else throw QObject::tr("Wrong archive atribute name : %1").arg(attr_name);
}

QString PisiSource::get_archive_attribute(ArchiveAttr attr) throw(QString)
{
    if(attr == SHA1SUM)
        return QString("sha1sum");
    else if(attr == TYPE)
        return QString("type");
    else if(attr == TARGET_ARCHIVE)
        return QString("target");
    else
        throw QObject::tr("Wrong archive atribute index : %1").arg(attr);
}

PisiSource::PatchAttr PisiSource::get_patch_attribute(QString attr_name) throw(QString)
{
    if(attr_name.toLower() == "compressiontype")
        return COMPRESSIONTYPE;
    else if(attr_name.toLower() == "level")
        return LEVEL;
    else if(attr_name.toLower() == "target")
        return TARGET_PATCH;
    else if(attr_name.toLower() == "reverse")
        return REVERSE;
    else throw QObject::tr("Wrong patch atribute name : %1").arg(attr_name);
}

QString PisiSource::get_patch_attribute(PatchAttr attr) throw(QString)
{
    if(attr == COMPRESSIONTYPE)
        return QString("compressionType");
    else if(attr == LEVEL)
        return QString("level");
    else if(attr == TARGET_PATCH)
        return QString("target");
    else if(attr == REVERSE)
        return QString("reverse");
    else
        throw QObject::tr("Wrong patch atribute index : %1").arg(attr);
}

bool PisiSource::operator ==(const PisiSource & other)
{
    return (
                PisiSPBase::operator ==(other)
                && get_home_page() == other.get_home_page()
                && get_packager() == other.get_packager()
                && get_archives() == other.get_archives()
                && get_patches() == other.get_patches()
                );
}

bool PisiSource::operator !=(const PisiSource & other)
{
    return ! (*this == other);
}

bool PisiSource::is_mandatory(QDomElement root, QString tag_) throw(QString)
{
    QString tag = tag_.toLower();
    QString root_tag = root.tagName().toLower();

    if(root_tag == "source"){
        if(tag == "name")
            return true;
        else if(tag == "license")
            return true;
        else if(tag == "summary")
            return true;
        else if(tag == "description")
            return true;
        else if(tag == "partof")
            return false;
        else if(tag == "isa")
            return false;
        else if(tag == "homepage")
            return true;
        else if(tag == "archive")
            return true;
        else
            throw QObject::tr("Undefined tag name \"%1\" in PisiSource::is_mandatory() !").arg(tag);
    }
    else if(root_tag == "packager"){
        if(tag == "name")
            return true;
        else if(tag == "email")
            return true;
        else
            throw QObject::tr("Undefined tag name \"%1\"for packager tag in PisiSource::is_mandatory() !").arg(tag);
    }
    else
        throw QObject::tr("Undefined root_tag name \"%1\" in PisiSource::is_mandatory() !").arg(root_tag);
}

QString PisiSource::get_archive_type(const QString & file_name)
{
    QString file_type;
    if(file_name.endsWith(".tgz") || file_name.endsWith(".tar.gz"))
        file_type = "targz";
    else if(file_name.endsWith(".tar.bz2"))
        file_type = "tarbz2";
    else if(file_name.endsWith(".tar.lzma"))
        file_type = "tarlzma";
    else if(file_name.endsWith(".tar.xz"))
        file_type = "tarxz";
    else if(file_name.endsWith(".tar.Z"))
        file_type = "tarZ";
    else if(file_name.endsWith(".tar"))
        file_type = "tar";
    else if(file_name.endsWith(".zip"))
        file_type = "zip";
    else if(file_name.endsWith(".gz"))
        file_type = "gz";
    else if(file_name.endsWith(".gzip"))
        file_type = "gzip";
    else if(file_name.endsWith(".bz2"))
        file_type = "bz2";
    else if(file_name.endsWith(".bzip2"))
        file_type = "bzip2";
    else if(file_name.endsWith(".lzma"))
        file_type = "lzma";
    else if(file_name.endsWith(".xz"))
        file_type = "xz";
    else
        file_type = "binary";
    return file_type;
}

QStringList PisiSource::get_archive_type_list(){
    QStringList list;
    list << "targz";
    list << "tarbz2";
    list << "tarlzma";
    list << "tarxz";
    list << "tarZ";
    list << "tar";
    list << "zip";
    list << "gz";
    list << "gzip";
    list << "bz2";
    list << "bzip2";
    list << "lzma";
    list << "xz";
    list << "7z";
    list << "binary";
    return list;
}


