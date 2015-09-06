#include "h/removecontainersdialog.h"
#include "ui_removecontainersdialog.h"

RemoveContainersDialog::RemoveContainersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemoveContainersDialog)
{
    ui->setupUi(this);
}

RemoveContainersDialog::~RemoveContainersDialog()
{
    delete ui;
}
