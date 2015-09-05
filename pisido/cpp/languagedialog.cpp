#include "languagedialog.h"
#include "ui_languagedialog.h"

LanguageDialog::LanguageDialog(QStringList list, QString current_lang, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LanguageDialog)
{
    ui->setupUi(this);

    ui->comboBox->clear();
    ui->comboBox->addItems(list);
    if( ! current_lang.isEmpty())
        ui->comboBox->setCurrentIndex(ui->comboBox->findText(current_lang));
}

LanguageDialog::~LanguageDialog()
{
    delete ui;
}

void LanguageDialog::on_comboBox_currentIndexChanged(const QString &arg1)
{
    language = arg1;
}

QString LanguageDialog::selectedLanguage()
{
    return language;
}
