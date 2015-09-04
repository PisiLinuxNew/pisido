#include "addinstallfilelabeldialog.h"
#include "ui_addinstallfilelabeldialog.h"

AddInstallFileLabelDialog::AddInstallFileLabelDialog(QWidget *parent, QString path) :
    QDialog(parent),
    ui(new Ui::AddInstallFileLabelDialog)
{
    ui->setupUi(this);

    ui->le_path->setText(path);
    ui->combo_file_type->setCurrentIndex(ui->combo_file_type->findText("ALL"));
    ui->check_permanent->setChecked(false);

    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(buttonBox_accepted()));
}

AddInstallFileLabelDialog::~AddInstallFileLabelDialog()
{
    delete ui;
}

void AddInstallFileLabelDialog::buttonBox_accepted()
{
    accept();
}

QString AddInstallFileLabelDialog::get_file_type()
{
    return ui->combo_file_type->currentText();
}

bool AddInstallFileLabelDialog::get_permanent()
{
    return ui->check_permanent->isChecked();
}
