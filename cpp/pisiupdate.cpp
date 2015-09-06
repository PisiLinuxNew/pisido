#include "pisiupdate.h"

#include <QDomElement>

PisiUpdate::PisiUpdate()
{
    clear();
}

void PisiUpdate::clear()
{
    date = QDate();
    version.clear();
    comment.clear();
    packager_name.clear();
    packager_email.clear();
    release = 0;
}

void PisiUpdate::load_from_dom(const QDomElement & dom_element) throw(QString)
{
    if(dom_element.isNull())
        throw QObject::tr("Dom Element is null while loading to PisiUpdate !");

    QDomElement elm = dom_element.firstChildElement("Version");
    version = get_element_value(elm, "Version", true);

    elm = dom_element.firstChildElement("Comment");
    comment = get_element_value(elm, "Comment", true);

    elm = dom_element.firstChildElement("Name");
    packager_name = get_element_value(elm, "Name", true);

    elm = dom_element.firstChildElement("Email");
    packager_email = get_element_value(elm, "Email", true);

    elm = dom_element.firstChildElement("Date");
    date = QDate::fromString(get_element_value(elm, "Date", true),"yyyy-MM-dd");

    bool ok = false;
    int v = dom_element.attribute("release", "0").toInt(&ok);
    if(ok) release = v;
    else release = 0;
}

void PisiUpdate::save_to_dom(QDomElement & dom_element) throw(QString)
{
    if(dom_element.isNull())
        throw QObject::tr("Dom Element is null while saving from PisiUpdate to dom !");

    dom_element.setAttribute("release", release);
    append_text_element(dom_element, "Date", date.toString("yyyy-MM-dd"));
    append_text_element(dom_element, "Version", version);
    append_text_element(dom_element, "Comment", comment);
    append_text_element(dom_element, "Name", packager_name);
    append_text_element(dom_element, "Email", packager_email);
}

QString PisiUpdate::get_element_value(QDomElement elm, QString tag, bool mandatory) throw(QString)
{
    if(elm.isNull())
    {
        if(mandatory)
            throw QObject::tr("No %1 tag !").arg(tag);
        else return QString();
    }
    else
    {
//        qDebug() << tag << " : " << elm.text();
        return elm.text();
    }
}

QDomElement PisiUpdate::append_text_element(QDomElement root, QString tag, QString value) throw(QString)
{
    QDomElement elm = root.firstChildElement(tag);
    if( ! elm.isNull())
        root.removeChild(elm);

    elm = root.ownerDocument().createElement(tag);
    if(elm.isNull() || root.appendChild(elm).isNull())
        throw QObject::tr("Error while creating dom element %1").arg(tag);

    if(value.isEmpty())
        throw QObject::tr("%1 tag is mandatory but empty !");

    QDomText text = root.ownerDocument().createTextNode(value);
    if(text.isNull() || elm.appendChild(text).isNull())
        throw QObject::tr("Error while creating dom text element with value = %1").arg(value);

    return elm;
}


int PisiUpdate::get_release() const
{
    return release;
}

QDate PisiUpdate::get_date() const
{
    return date;
}

QString PisiUpdate::get_version() const
{
    return version;
}

QString PisiUpdate::get_comment() const
{
    return comment;
}

QString PisiUpdate::get_packager_name() const
{
    return packager_name;
}

QString PisiUpdate::get_packager_email() const
{
    return packager_email;
}


void PisiUpdate::set_release(int r) throw(QString)
{
    if(r<1)
        throw QObject::tr("Release number error : %1").arg(r);

    if(r>=1) release = r;
}

void PisiUpdate::set_date(QDate d) throw(QString)
{
    if(d == QDate())
        throw QObject::tr("Empty update date !");

    if(d.isValid()) date = d;
}

void PisiUpdate::set_version(QString v) throw(QString)
{
    if(v.isEmpty())
        throw QObject::tr("Empty update version !");

    version = v;
}

void PisiUpdate::set_comment(QString c) throw(QString)
{
    if(c.isEmpty())
        throw QObject::tr("Empty update comment !");

    comment = c;
}

void PisiUpdate::set_packager_name(QString p_n) throw(QString)
{
    if(p_n.isEmpty())
        throw QObject::tr("Empty update packager name !");

    packager_name = p_n;
}

void PisiUpdate::set_packager_email(QString p_e) throw(QString)
{
    if(p_e.isEmpty())
        throw QObject::tr("Empty update packager email !");

    packager_email = p_e;
}

bool PisiUpdate::operator ==(const PisiUpdate & other)
{
    return ( get_comment() == other.get_comment()
             && get_date() == other.get_date()
             && get_packager_email() == other.get_packager_email()
             && get_packager_name() == other.get_packager_name()
             && get_release() == other.get_release()
             && get_version() == other.get_version() );
}

bool PisiUpdate::operator !=(const PisiUpdate & other)
{
    return  ! ( *this == other );
}

bool PisiUpdate::operator <(const PisiUpdate & other)
{
    return get_release() < other.get_release();
}

bool PisiUpdate::operator >(const PisiUpdate & other)
{
    return get_release() > other.get_release();
}
