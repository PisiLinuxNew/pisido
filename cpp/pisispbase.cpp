#include "pisispbase.h"

#include <QDomElement>

#include <QTextStream>
#include <QDebug>

PisiSPBase::PisiSPBase()
{
    clear();
}

void PisiSPBase::clear()
{
    name.clear();
    summary.clear();
    description.clear();
    part_of.clear();
    licenses.clear();
    is_a_s.clear();
    build_dependencies.clear();
    additional_files.clear();
}

void PisiSPBase::load_from_dom(const QDomElement & root) throw(QString)
{
    if(root.isNull())
        throw QObject::tr("Dom Element is null while loading to PisiSPBase !");


    name = get_element_value(root, "Name");

    licenses.clear();
    QDomElement elm = root.firstChildElement("License");
    while( ! elm.isNull()){
        QString text = elm.text().trimmed();
        if( ! text.isEmpty())
        licenses << text;
        elm = elm.nextSiblingElement("License");
    }

    is_a_s.clear();
    elm = root.firstChildElement("IsA");
    while( ! elm.isNull()){
        QString text = elm.text().trimmed();
        if( ! text.isEmpty())
            is_a_s << text;
        elm = elm.nextSiblingElement("IsA");
    }

    summary = get_element_value(root, "Summary");
    description = get_element_value(root, "Description");
    part_of = get_element_value(root, "PartOf");

    elm = root.firstChildElement("BuildDependencies");
    build_dependencies = get_dependency(elm);
    elm = root.firstChildElement("AdditionalFiles");
    additional_files = get_additional_file(elm);
}

void PisiSPBase::save_to_dom(QDomElement & root) throw(QString)
{
    if(root.isNull())
        throw QObject::tr("Dom Element is null while saving from PisiSPBase to dom !");

    set_element_value(root, "Name", name);
    foreach (QString license, licenses) {
        if( ! license.isEmpty()){
            QDomElement elm = append_element(root, "License");
            append_text_element(elm, license);
        }
    }
    set_element_value(root, "PartOf", part_of);
    foreach (QString is_a, is_a_s) {
        if( ! is_a.isEmpty()){
            QDomElement elm = append_element(root, "IsA");
            append_text_element(elm, is_a);
        }
    }
    set_element_value(root, "Summary", summary);
    set_element_value(root, "Description", description);

    if( ! build_dependencies.isEmpty())
    {
        // optional
        QDomElement elm = root.firstChildElement("BuildDependencies");
        if( ! elm.isNull())
            root.removeChild(elm);
        elm = append_element(root, "BuildDependencies");
        set_dependency(elm, build_dependencies);
    }

    if( ! additional_files.isEmpty())
    {
        // optional
        QDomElement elm = root.firstChildElement("AdditionalFiles");
        if( ! elm.isNull())
            root.removeChild(elm);
        elm = append_element(root, "AdditionalFiles");
        set_additional_file(elm, additional_files);
    }

}

QString PisiSPBase::get_name() const
{
    return name;
}

QString PisiSPBase::get_summary() const
{
    return summary;
}

QString PisiSPBase::get_description() const
{
    return description;
}

QString PisiSPBase::get_part_of() const
{
    return part_of;
}

QStringList PisiSPBase::get_licenses() const
{
    return licenses;
}

QStringList PisiSPBase::get_is_a_s() const
{
    return is_a_s;
}

QStringList PisiSPBase::get_build_dependencies_as_stringlist()
{
    return get_dependency_list(build_dependencies);
}

QMap<QString, QMap<PisiSPBase::VRTFAttr,QString> > PisiSPBase::get_build_dependencies() const
{
    return build_dependencies;
}

QMap<QString, QMap<PisiSPBase::AFileAttr,QString> > PisiSPBase::get_additional_files() const
{
    return additional_files;
}

void PisiSPBase::set_name(QString name) throw(QString)
{
    if(name.isEmpty())
        throw QObject::tr("Empty name !");

    this->name = name;
}

void PisiSPBase::set_summary(QString summary) throw(QString)
{
    if(summary.isEmpty())
        throw QObject::tr("Empty summary !");

    this->summary = summary;
}

void PisiSPBase::set_description(QString description)
{
    // zero or more, no check
    this->description = description;
}

void PisiSPBase::set_part_of(QString part_of)
{
    // optional, no check
    this->part_of = part_of;
}

void PisiSPBase::set_licenses(QStringList licenses) throw(QString)
{
    if(licenses.isEmpty())
        throw QObject::tr("Empty license !");

    this->licenses = licenses;
}

void PisiSPBase::set_is_a_s(QStringList is_a_s)
{
    // zero or more, no check
    this->is_a_s = is_a_s;
}

void PisiSPBase::set_build_dependencies(QMap<QString, QMap<PisiSPBase::VRTFAttr, QString> > build_dependencies)
{
    // optional, no check
    this->build_dependencies = build_dependencies;
}

void PisiSPBase::set_build_dependencies(QString build_dependency_string)
{
    QMap<QString, QMap<VRTFAttr, QString> > build_dependencies = get_dependency_list(build_dependency_string);
    set_build_dependencies(build_dependencies);
}

void PisiSPBase::set_additional_files(QMap<QString, QMap<PisiSPBase::AFileAttr,QString> > additional_files)
{
    // optional, no check
    this->additional_files = additional_files;
}

QString PisiSPBase::get_element_value(QDomElement root, QString tag) throw(QString)
{
    QDomElement elm = root.firstChildElement(tag);
    if(elm.isNull())
    {
        if(is_mandatory(root, tag))
            throw QObject::tr("No %1 tag !").arg(tag);
        else return QString();
    }
    else
    {
//        qDebug() << tag << " : " << elm.text();
        return elm.text();
    }
}

QDomElement PisiSPBase::set_element_value(QDomElement root, QString tag, QString value, QString insert_after) throw(QString)
{
    if(value.isEmpty()){
        if(is_mandatory(root, tag))
            throw QObject::tr("%1 tag is mandatory but empty !").arg(tag);
        else
            return QDomElement();
    }

    QDomElement elm = root.firstChildElement(tag);
    if( ! insert_after.isEmpty()){
        if( ! elm.isNull())
            root.removeChild(elm);
        elm = insert_element_after(root, tag, insert_after);
    }
    else{
        if(elm.isNull())
            elm = append_element(root, tag);
    }
    append_text_element(elm, value);
    return elm;
}

QDomElement PisiSPBase::append_element(QDomElement & root, QString tag) throw(QString)
{
    QDomElement elm = root.ownerDocument().createElement(tag);
    if(elm.isNull() || root.appendChild(elm).isNull())
        throw QObject::tr("Error while creating dom element %1 in %2").arg(tag).arg(root.tagName());
    return elm;
}

QDomElement PisiSPBase::insert_element_after(QDomElement & root, QString tag, QString after_this_tag) throw(QString)
{
    QDomElement pivot_elm = root.firstChildElement(after_this_tag);
    if(pivot_elm.isNull())
        throw QObject::tr("Trying to insert %1 after %2 but no %2 tag !").arg(tag).arg(after_this_tag);
    QDomElement elm = root.ownerDocument().createElement(tag);
    if(elm.isNull() || root.insertAfter(elm,pivot_elm).isNull())
        throw QObject::tr("Error while creating dom element %1 in %2").arg(tag).arg(root.tagName());
    return elm;
}

QDomElement PisiSPBase::insert_element_before(QDomElement & root, QString tag, QString before_this_tag) throw(QString)
{
    QDomElement pivot_elm = root.firstChildElement(before_this_tag);
    if(pivot_elm.isNull())
        throw QObject::tr("Trying to insert %1 before %2 but no %2 tag !").arg(tag).arg(before_this_tag);
    QDomElement elm = root.ownerDocument().createElement(tag);
    if(elm.isNull() || root.insertBefore(elm,pivot_elm).isNull())
        throw QObject::tr("Error while creating dom element %1 in %2").arg(tag).arg(root.tagName());
    return elm;
}

QDomText PisiSPBase::append_text_element(QDomElement root, QString value) throw(QString)
{
    QDomText text = root.ownerDocument().createTextNode(value);
    if(text.isNull() || root.appendChild(text).isNull())
        throw QObject::tr("Error creating text element with %1 in to the %2").arg(value).arg(root.tagName());
    return text;
}

QMap<QString, QMap<PisiSPBase::VRTFAttr,QString> > PisiSPBase::get_dependency(QDomElement elm) throw(QString)
{
    if(elm.isNull())
    {
        return QMap<QString, QMap<VRTFAttr,QString> >();
    }

    QMap<QString, QMap<VRTFAttr,QString> > dependencies;

    elm = elm.firstChildElement("Dependency");
    if(elm.isNull())
        return dependencies;

    for( ; ! elm.isNull(); elm = elm.nextSiblingElement("Dependency"))
    {
        QMap<VRTFAttr,QString> attributes;
        QDomNamedNodeMap elm_node_map = elm.attributes();
        int count = elm_node_map.count();
        for(int i=0; i<count; ++i)
        {
            QDomNode n = elm_node_map.item(i);
            QDomAttr a = n.toAttr();
            if(a.isNull())
                throw QObject::tr("Can not convert to attribute !");
            attributes[get_dependency_attribute(a.name())] = a.value();
//            qDebug() << "DepAttr = " << a.name() << " : " << a.value();
        }
        dependencies[elm.text()] = attributes;
//        qDebug() << "Dep:" << elm.text();
    }
    return dependencies;
}

void PisiSPBase::set_dependency(QDomElement root, QMap<QString, QMap<VRTFAttr,QString> > deps)
{
    if(deps.isEmpty())
    {
        return;
    }

    QList<QString> dependencies = deps.keys();
    for(int i=0; i<dependencies.count(); ++i)
    {
        QString dependency = dependencies.at(i);
        QDomElement elm = append_element(root, "Dependency");
        append_text_element(elm, dependency);

        QMap<VRTFAttr,QString> attr = deps[dependency];
        QList<VRTFAttr> attributes = attr.keys();
        for(int j=0; j<attributes.count(); ++j)
        {
            VRTFAttr a = attributes.at(j);
            QString a_value = attr[a];
            elm.setAttribute(get_dependency_attribute(a), a_value);
        }
    }
}

PisiSPBase::VRTFAttr PisiSPBase::get_dependency_attribute(QString attr_name, bool abbreviation) throw(QString)
{
    if( ! abbreviation)
    {
        if(attr_name.toLower() == "versionfrom")
            return VERSIONFROM;
        else if(attr_name.toLower() == "versionto")
            return VERSIONTO;
        else if(attr_name.toLower() == "version")
            return VERSION;
        else if(attr_name.toLower() == "releasefrom")
            return RELEASEFROM;
        else if(attr_name.toLower() == "releaseto")
            return RELEASETO;
        else if(attr_name.toLower() == "release")
            return RELEASE;
        else throw QObject::tr("Wrong dependency atribute name : %1").arg(attr_name);
    }
    else
    {
        if(attr_name.startsWith("=="))
            return RELEASE;
        else if(attr_name.startsWith("<<"))
            return RELEASETO;
        else if(attr_name.startsWith(">>"))
            return RELEASEFROM;
        else if(attr_name.startsWith("="))
            return VERSION;
        else if(attr_name.startsWith("<"))
            return VERSIONTO;
        else if(attr_name.startsWith(">"))
            return VERSIONFROM;
        else throw QObject::tr("No VersionReleaseToFrom attribute : %1").arg(attr_name);
    }
}

QString PisiSPBase::get_dependency_attribute(PisiSPBase::VRTFAttr attr, bool abbreviation)
{
    switch(attr)
    {
    case VERSIONFROM:
        if(abbreviation) return ">"; else return "versionFrom";
        break;
    case VERSIONTO:
        if(abbreviation) return "<"; else return "versionTo";
        break;
    case VERSION:
        if(abbreviation) return "="; else return "version";
        break;
    case RELEASEFROM:
        if(abbreviation) return ">>"; else return "releaseFrom";
        break;
    case RELEASETO:
        if(abbreviation) return "<<"; else return "releaseTo";
        break;
    case RELEASE:
        if(abbreviation) return "=="; else return "release";
        break;
    default:
        return "";
        break;
    }
}

QMap<QString, QMap<PisiSPBase::AFileAttr,QString> > PisiSPBase::get_additional_file(QDomElement elm) throw(QString)
{
    if(elm.isNull())
    {
        return QMap<QString, QMap<AFileAttr,QString> >();
    }
    elm = elm.firstChildElement("AdditionalFile");

    if(elm.isNull())
    {
        return QMap<QString, QMap<AFileAttr,QString> >();
    }

    QMap<QString, QMap<AFileAttr,QString> > additional_files;

    for( ; ! elm.isNull(); elm = elm.nextSiblingElement("AdditionalFile"))
    {
        QMap<AFileAttr,QString> attributes;
        QDomNamedNodeMap elm_node_map = elm.attributes();
        int count = elm_node_map.count();
        for(int i=0; i<count; ++i)
        {
            QDomNode n = elm_node_map.item(i);
            QDomAttr a = n.toAttr();
            if(a.isNull())
                throw QObject::tr("Can not convert to attribute !");
            attributes[get_additional_file_attribute(a.name())] = a.value();
//            qDebug() << "AFileAttr = " << a.name() << " : " << a.value();
        }
        additional_files[elm.text()] = attributes;
//        qDebug() << "AFile:" << elm.text();
    }
    return additional_files;
}

void PisiSPBase::set_additional_file(QDomElement root, QMap<QString, QMap<AFileAttr,QString> > a_files)
{
    if(a_files.isEmpty())
    {
        return;
    }

    QList<QString> additional_files = a_files.keys();
    for(int i=0; i<additional_files.count(); ++i)
    {
        QString additional_file = additional_files.at(i);
        QDomElement elm = append_element(root, "AdditionalFile");
        append_text_element(elm, additional_file);

        QMap<AFileAttr,QString> attr = a_files[additional_file];
        QList<AFileAttr> attributes = attr.keys();
        for(int j=0; j<attributes.count(); ++j)
        {
            AFileAttr a = attributes.at(j);
            QString a_value = attr[a];
            elm.setAttribute(get_additional_file_attribute(a), a_value);
        }
    }
}

PisiSPBase::AFileAttr PisiSPBase::get_additional_file_attribute(QString attr_name) throw(QString)
{
        if(attr_name.toLower() == "target")
            return TARGET;
        else if(attr_name.toLower() == "permission")
            return PERMISSION;
        else if(attr_name.toLower() == "owner")
            return OWNER;
        else if(attr_name.toLower() == "group")
            return GROUP;
        else
            throw QObject::tr("Wrong additional_files atribute name : %1").arg(attr_name);
}

QString PisiSPBase::get_additional_file_attribute(PisiSPBase::AFileAttr attr)
{
    switch(attr)
    {
    case TARGET:
        return "target";
        break;
    case PERMISSION:
        return "permission";
        break;
    case OWNER:
        return "owner";
        break;
    case GROUP:
        return "group";
        break;
    default:
        return "";
        break;
    }
}

/**
  returns dependencies like : qt[>4.7;<4.5;==4.6], gtk[>>2], libz, libusb1[=1]
*/
QStringList PisiSPBase::get_dependency_list(QMap<QString, QMap<PisiSPBase::VRTFAttr,QString> > dependencies)
{
    QStringList dep_list;
    QMap<QString, QMap<VRTFAttr,QString> >::const_iterator dependency_it = dependencies.constBegin();
    for( ; dependency_it != dependencies.constEnd(); dependency_it++)
    {
        QStringList attr_list;
        QMap<VRTFAttr,QString> attr = dependency_it.value();
        QMap<VRTFAttr,QString>::const_iterator attr_it = attr.constBegin();
        for( ; attr_it != attr.constEnd(); attr_it++)
        {
            attr_list << QString("%1%2").arg(get_dependency_attribute(attr_it.key(), true)).arg(attr_it.value());
        }
        QString attributes = ((attr_list.count()>0)?QString("%1%2%3").arg("[").arg(attr_list.join(";")).arg("]"):"");
        dep_list << QString("%1%2").arg(dependency_it.key()).arg(attributes);
//        qDebug() << "dep list : " << dep_list;
    }
    return dep_list;
}

/**
  takes string like : qt[>4.7;<4.5;==4.6], gtk[>>2], libz, libusb1[=1], returns dependencies
*/
QMap<QString, QMap<PisiSPBase::VRTFAttr,QString> > PisiSPBase::get_dependency_list(QString dependency_string)
{
    QMap<QString, QMap<VRTFAttr,QString> > dependencies;
    QStringList dependency_list = dependency_string.split(',', QString::SkipEmptyParts);
    foreach (QString dep, dependency_list) {
        dep = dep.trimmed();
//        qDebug() << "dep:"<< dep;
        if( ! dep.isEmpty())
        {
            QString dep_name;
            QString dep_attr;
            int start_index = dep.indexOf('[');
            int end_index = dep.indexOf(']');
            if(start_index > 0)
            {
                dep_name = dep.left(start_index);
                if(end_index > 0)
                    dep_attr = dep.mid(start_index+1);
                dep_attr = dep_attr.left(dep_attr.size()-1);
//                qDebug() << "dep_attr:" << dep_attr;
            }
            else
                dep_name = dep;
//            qDebug() << "dep:" << dep_name;
            dependencies[dep_name] = get_dependency_attr_list(dep_attr);
        }
    }
    return dependencies;
}

/**
  helper function to return dependency attributes, takes strin like : >4.7;<4.5;==4.6
*/

QMap<PisiSPBase::VRTFAttr,QString> PisiSPBase::get_dependency_attr_list(QString attr_string)
{
    QMap<VRTFAttr,QString> attributes;
    QStringList attr_list = attr_string.split(';', QString::SkipEmptyParts);
    foreach (QString attr, attr_list) {
        attr = attr.trimmed();
        if( ! attr.isEmpty()){
            VRTFAttr a = get_dependency_attribute(attr, true);
            QString t = get_dependency_attribute(a, true);
            int start_index = attr.indexOf(t, Qt::CaseInsensitive) + t.length();
            QString v = attr.mid(start_index);
            attributes[a] = v;
        }
    }
    return attributes;
}


bool PisiSPBase::operator ==(const PisiSPBase & other) const
{
    return (
                get_name()                  == other.get_name()
                && get_summary()            == other.get_summary()
                && get_description()        == other.get_description()
                && get_part_of()            == other.get_part_of()
                && get_licenses()           == other.get_licenses()
                && get_is_a_s()             == other.get_is_a_s()
                && get_build_dependencies() == other.get_build_dependencies()
                && get_additional_files()    == other.get_additional_files()
                );
}

bool PisiSPBase::operator !=(const PisiSPBase & other) const
{
    return ! (*this == other);
}
