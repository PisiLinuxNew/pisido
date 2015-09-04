#include "workspacedialog.h"
#include "ui_workspacedialog.h"

#include <QSettings>
#include <QDesktopServices>

#ifdef USING_QT5
    #include <QtWidgets/QFileDialog>
    #include <QStandardPaths>
#else
    #include <QFileDialog>
#endif


WorkspaceDialog::WorkspaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WorkspaceDialog)
{
    ui->setupUi(this);

    connect(this, SIGNAL(accepted()), SLOT(save_settings()));

    read_settings();
}

WorkspaceDialog::~WorkspaceDialog()
{
    delete ui;
}

void WorkspaceDialog::on_pb_browse_clicked()
{
    QString new_workspace = QFileDialog::getExistingDirectory(this, tr("Select Workspace"), workspace);
    if(new_workspace.isEmpty())
        return;
    workspace = new_workspace;
    ui->le_workspace->setText(workspace);
}

void WorkspaceDialog::on_le_workspace_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    if(QDir(workspace).exists())
    {
        ui->check_not_ask->setEnabled(true);
        ui->pb_ok->setEnabled(true);
    }
    else
    {
        ui->check_not_ask->setEnabled(false);
        ui->pb_ok->setEnabled(false);
    }
}

void WorkspaceDialog::save_settings()
{
    if(workspace.isEmpty())
        return;
    QSettings settings;
    settings.beginGroup("main");
    settings.setValue("not_ask_workspace", not_ask_workspace);
    settings.setValue("workspace", workspace);
    settings.endGroup();
}

void WorkspaceDialog::read_settings()
{
#ifdef USING_QT5
    QString default_workspace = QStandardPaths::standardLocations(QStandardPaths::HomeLocation)[0];
#else
    QString default_workspace = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
#endif
    QSettings settings;
    settings.beginGroup("main");
    workspace = settings.value("workspace", default_workspace).toString();
    not_ask_workspace = settings.value("not_ask_workspace", false).toBool();
    settings.endGroup();
    ui->le_workspace->setText(workspace);
    ui->check_not_ask->setChecked(not_ask_workspace);
}

QString WorkspaceDialog::get_workspace()
{
    return workspace;
}

bool WorkspaceDialog::get_not_ask_workspace()
{
    return not_ask_workspace;
}

void WorkspaceDialog::on_check_not_ask_stateChanged(int arg1)
{
    not_ask_workspace = (arg1 == Qt::Checked);
}
