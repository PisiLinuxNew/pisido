#ifndef PISIUPDATE_H
#define PISIUPDATE_H

#include <QString>
#include <QDate>

class QDomElement;

class PisiUpdate
{
public:
    PisiUpdate();

    void clear();
    void load_from_dom(const QDomElement & dom_element) throw(QString);
    void save_to_dom(QDomElement & dom_element) throw(QString);

    int get_release() const;
    QDate get_date() const;
    QString get_version() const;
    QString get_comment() const;
    QString get_packager_name() const;
    QString get_packager_email() const;

    void set_release(int r) throw(QString);
    void set_date(QDate d) throw(QString);
    void set_version(QString v) throw(QString);
    void set_comment(QString c) throw(QString);
    void set_packager_name(QString p_n) throw(QString);
    void set_packager_email(QString p_e) throw(QString);

    bool operator ==(const PisiUpdate & other);
    bool operator !=(const PisiUpdate & other);
    bool operator <(const PisiUpdate & other);
    bool operator >(const PisiUpdate & other);

private:
    QString get_element_value(QDomElement elm, QString tag, bool mandatory) throw(QString);
    QDomElement append_text_element(QDomElement root, QString tag, QString value) throw(QString);

private:
    int release;

    QDate date;
    QString version;
    QString comment;
    QString packager_name;
    QString packager_email;
};

#endif // PISIUPDATE_H
