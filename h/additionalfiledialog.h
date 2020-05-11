#ifndef ADITIONALFILEDIALOG_H
#define ADITIONALFILEDIALOG_H

#ifdef USING_QT5
    #include <QtWidgets/QDialog>
#else
    #include <QDialog>
#endif
#include <QMap>
#include "pisispbase.h"

namespace Ui {
    class AdditionalFileDialog;
}

class AdditionalFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdditionalFileDialog(QWidget * parent, QString a_file, QMap<PisiSPBase::AFileAttr,QString> attr);
    ~AdditionalFileDialog();

    QMap<PisiSPBase::AFileAttr,QString> get_attr();

public slots:
    void dialog_accepted();

private:
    Ui::AdditionalFileDialog *ui;
    QString a_file;
    QMap<PisiSPBase::AFileAttr,QString> attr;

    QString get_permissions();
    void set_permissions(const QString & permission);
};

#endif // ADITIONALFILEDIALOG_H
