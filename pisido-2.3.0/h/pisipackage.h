#ifndef PISIPACKAGE_H
#define PISIPACKAGE_H

#include "pisispbase.h"

class QDomElement;

class PisiPackage : public PisiSPBase
{
public:
    PisiPackage();

    enum FileType {
        EXECUTABLE,
        LIBRARY,
        DATA,
        CONFIG,
        DOC,
        MAN,
        INFO,
        LOCALEDATA,
        HEADER,
        ALL
    };

    virtual void clear();
    virtual void load_from_dom(const QDomElement & dom_element) throw(QString);
    virtual void save_to_dom(QDomElement & dom_element) throw(QString);

    QString get_psummary();
    QStringList get_runtime_dependencies_as_stringlist();
    QMap<QString, QMap<VRTFAttr,QString> > get_runtime_dependencies() const;
    QMap<QString, QMap<FileType, bool> > get_files() const;
    QMap<QString, QMap<QString, bool> > get_files_as_string_type();

    void set_runtime_dependencies(QMap<QString, QMap<VRTFAttr,QString> > runtime_dependencies);
    void set_runtime_dependencies(QString runtime_dependency_string);
    void set_files(QMap<QString, QMap<FileType, bool> > files);
    void set_files(QMap<QString, QMap<QString, bool> > files);

    void set_psummary(QString summary);

    bool operator ==(const PisiPackage & other);
    bool operator !=(const PisiPackage & other);

protected:
    virtual bool is_mandatory(QDomElement root, QString tag) throw(QString);
    QMap<QString, QMap<FileType, bool> > get_files(QDomElement elm) throw(QString);
    void set_files(QDomElement root, QMap<QString, QMap<FileType, bool> > files) throw(QString);

private:
    // dependency, {(versionFrom,xxxx),(versionTo,xxxx),...,(release,xxxx)}
    QMap<QString, QMap<VRTFAttr,QString> > runtime_dependencies;
    // files, {(FileType, permanent)}
    QMap<QString, QMap<FileType, bool> > files;
    //sumary string
    QString psummary=NULL;

    QString get_files_file_type(PisiPackage::FileType attr);
    FileType get_files_file_type(QString attr_name) throw(QString);
};

#endif // PISIPACKAGE_H
