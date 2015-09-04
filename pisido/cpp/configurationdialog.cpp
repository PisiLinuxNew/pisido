#include "configurationdialog.h"
#include "ui_configurationdialog.h"

#include <QSettings>

ConfigurationDialog::ConfigurationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigurationDialog)
{
    ui->setupUi(this);
    read_settings();
}

ConfigurationDialog::~ConfigurationDialog()
{
    delete ui;
}

void ConfigurationDialog::read_settings()
{
    QSettings settings;

    settings.beginGroup( "configuration" );
    ui->le_pisi_packaging_dir->setText(settings.value("pisi_packaging_dir").toString());
    ui->le_action_api_page->setText(settings.value("action_api_page").toString());
    ui->le_pisi_spec->setText(settings.value("pisi_spec").toString());
    settings.endGroup();

    settings.beginGroup("packager");
    ui->le_packager_name->setText(settings.value("packager_name").toString());
    ui->le_packager_email->setText(settings.value("packager_email").toString());
    settings.endGroup();
}

void ConfigurationDialog::write_settings()
{
    QSettings settings;

    settings.beginGroup( "configuration" );
    settings.setValue("pisi_packaging_dir", ui->le_pisi_packaging_dir->text());
    settings.setValue("action_api_page", ui->le_action_api_page->text());
    settings.setValue("pisi_spec", ui->le_pisi_spec->text());
    settings.endGroup();

    settings.beginGroup("packager");
    settings.setValue("packager_name", ui->le_packager_name->text());
    settings.setValue("packager_email", ui->le_packager_email->text());
    settings.endGroup();
}

void ConfigurationDialog::on_buttonBox_accepted()
{
    write_settings();
}
