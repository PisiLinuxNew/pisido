#ifndef CONFIGURATIONDIALOG_H
#define CONFIGURATIONDIALOG_H

#ifdef USING_QT5
    #include <QtWidgets/QDialog>
#else
    #include <QDialog>
#endif

namespace Ui {
    class ConfigurationDialog;
}

class ConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigurationDialog(QWidget *parent = 0);
    ~ConfigurationDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::ConfigurationDialog *ui;

    void read_settings();
    void write_settings();
};

#endif // CONFIGURATIONDIALOG_H
