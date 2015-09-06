#ifndef LANGUAGEDIALOG_H
#define LANGUAGEDIALOG_H

#ifdef USING_QT5
    #include <QtWidgets/QDialog>
#else
    #include <QDialog>
#endif

namespace Ui {
    class LanguageDialog;
}

class LanguageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LanguageDialog(QStringList list, QString current_lang, QWidget *parent = 0);
    ~LanguageDialog();

    QString selectedLanguage();

private slots:
    void on_comboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::LanguageDialog *ui;
    QString language;
};

#endif // LANGUAGEDIALOG_H
