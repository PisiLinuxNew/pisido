#include "h/dockersettingsdialog.h"
#include "ui_dockersettingsdialog.h"

#include <QSettings>
#include <QFileDialog>

DockerSettingsDialog::DockerSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DockerSettingsDialog)
{
    ui->setupUi(this);
    read_settings();
}

DockerSettingsDialog::~DockerSettingsDialog()
{
    delete ui;
}

void DockerSettingsDialog::read_settings()
{
    QSettings settings;

    settings.beginGroup( "docker" );
    if(!settings.value("docker_path").toString().isEmpty())
        ui->le_docker_path->setText(settings.value("docker_path").toString());
    else
        ui->le_docker_path->setText(QDir::homePath()+"/PisiLinux_2.0");
    settings.endGroup();
}

void DockerSettingsDialog::write_settings()
{
    QSettings settings;

    settings.beginGroup( "docker" );
    settings.setValue("docker_path", ui->le_docker_path->text());
    settings.endGroup();
}

void DockerSettingsDialog::on_buttonBox_accepted(){
    write_settings();
}

void DockerSettingsDialog::on_pb_open_folder_clicked(){
    QString folder= QFileDialog::getExistingDirectory(0,trUtf8("Select Directory"),QDir::homePath());
    if(!folder.isEmpty())
        ui->le_docker_path->setText(folder);
}
