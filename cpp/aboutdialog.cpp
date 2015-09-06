#include "aboutdialog.h"
#include "ui_aboutdialog.h"


AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    ui->lbl_program_name->setText(qApp->applicationName());
    ui->lbl_program_version->setText(qApp->applicationVersion());
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
