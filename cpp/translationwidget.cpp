#include "translationwidget.h"
#include "ui_translationwidget.h"

TranslationWidget::TranslationWidget(QWidget *parent, const QString &language) :
    QWidget(parent),
    ui(new Ui::TranslationWidget)
{
    ui->setupUi(this);
    ui->gb_language->setTitle(language);
}

TranslationWidget::~TranslationWidget()
{
    delete ui;
}

void TranslationWidget::on_tb_delete_clicked()
{
    emit delete_me(this);
}



QString TranslationWidget::get_summary()
{
    return ui->le_summary->text();
}

QString TranslationWidget::get_description()
{
    return ui->pte_description->toPlainText();
}

QString TranslationWidget::get_language()
{
    return ui->gb_language->title();
}

void TranslationWidget::set_summary(const QString &text)
{
    ui->le_summary->setText(text);
}

void TranslationWidget::set_description(const QString &text)
{
    ui->pte_description->setPlainText(text);
}


