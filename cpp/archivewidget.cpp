#include "archivewidget.h"
#include "ui_archivewidget.h"

ArchiveWidget::ArchiveWidget(QWidget * parent,
                             const QString & archive,
                             const QString & sha1,
                             const QStringList & types,
                             const QString & selected_type) :
    QWidget(parent),
    ui(new Ui::ArchiveWidget)
{
    ui->setupUi(this);

    ui->le_archive->setText(archive);
    ui->le_sha1->setText(sha1);

    ui->combo_type->addItems(types);
    int type_index = ui->combo_type->findText(selected_type);
    ui->combo_type->setCurrentIndex(type_index);
}

ArchiveWidget::~ArchiveWidget()
{
    delete ui;
}

void ArchiveWidget::on_tb_delete_clicked()
{
    emit delete_me(this);
}

QString ArchiveWidget::get_archive()
{
    return ui->le_archive->text();
}

QString ArchiveWidget::get_sha1()
{
    return ui->le_sha1->text();
}

QString ArchiveWidget::get_type()
{
    return ui->combo_type->currentText();
}

void ArchiveWidget::set_sha1(QString sha1){
    ui->le_sha1->setText(sha1);
}
