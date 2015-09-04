#include "aditionalfiledialog.h"
#include "ui_aditionalfiledialog.h"

#include <QChar>

AditionalFileDialog::AditionalFileDialog(QWidget *parent, QString a_file, QMap<PisiSPBase::AFileAttr, QString> attr) :
    QDialog(parent),
    ui(new Ui::AditionalFileDialog),
    a_file(a_file),
    attr(attr)
{
    ui->setupUi(this);

    ui->le_file->setText(a_file);
    ui->le_target->setText(attr.value(PisiSPBase::TARGET));
    set_permissions(attr.value(PisiSPBase::PERMISSION));
    ui->le_owner->setText(attr.value(PisiSPBase::OWNER));
    ui->le_group->setText(attr.value(PisiSPBase::GROUP));

    connect(this, SIGNAL(accepted()), SLOT(dialog_accepted()));
}

AditionalFileDialog::~AditionalFileDialog()
{
    delete ui;
}

void AditionalFileDialog::dialog_accepted()
{
    a_file = ui->le_file->text();
    QMap<PisiSPBase::AFileAttr,QString> attr;
    attr.insert(PisiSPBase::TARGET, ui->le_target->text());
    attr.insert(PisiSPBase::PERMISSION, get_permissions());
    attr.insert(PisiSPBase::OWNER, ui->le_owner->text());
    attr.insert(PisiSPBase::GROUP, ui->le_group->text());
    this->attr = attr;
}

QString AditionalFileDialog::get_permissions()
{
    quint8 special =
            (ui->c_uid->isChecked() ? 0x04 : 0)
            | (ui->c_gid->isChecked() ? 0x02 : 0)
            | (ui->c_sticky->isChecked() ? 0x01 : 0);
    quint8 user =
            (ui->c_u_r->isChecked() ? 0x04 : 0)
            | (ui->c_u_w->isChecked() ? 0x02 : 0)
            | (ui->c_u_x->isChecked() ? 0x01 : 0);
    quint8 group =
            (ui->c_g_r->isChecked() ? 0x04 : 0)
            | (ui->c_g_w->isChecked() ? 0x02 : 0)
            | (ui->c_g_x->isChecked() ? 0x01 : 0);
    quint8 others =
            (ui->c_o_r->isChecked() ? 0x04 : 0)
            | (ui->c_o_w->isChecked() ? 0x02 : 0)
            | (ui->c_o_x->isChecked() ? 0x01 : 0);
    QString permission = QString("%1%2%3%4").arg(special).arg(user).arg(group).arg(others);
    return permission;
}

void AditionalFileDialog::set_permissions(const QString & permission)
{
    QString perm = permission.rightJustified(4, '0');
    if(perm.count() > 4)
        perm = perm.right(4);
    qint8 special = perm.at(0).toLatin1();
    qint8 user = perm.at(1).toLatin1();
    qint8 group = perm.at(2).toLatin1();
    qint8 others = perm.at(3).toLatin1();
    ui->c_uid->setChecked(special & 0x04);
    ui->c_gid->setChecked(special & 0x02);
    ui->c_sticky->setChecked(special & 0x01);
    ui->c_u_r->setChecked(user & 0x04);
    ui->c_u_w->setChecked(user & 0x02);
    ui->c_u_x->setChecked(user & 0x01);
    ui->c_g_r->setChecked(group & 0x04);
    ui->c_g_w->setChecked(group & 0x02);
    ui->c_g_x->setChecked(group & 0x01);
    ui->c_o_r->setChecked(others & 0x04);
    ui->c_o_w->setChecked(others & 0x02);
    ui->c_o_x->setChecked(others & 0x01);
}

QMap<PisiSPBase::AFileAttr,QString> AditionalFileDialog::get_attr()
{
    return attr;
}






