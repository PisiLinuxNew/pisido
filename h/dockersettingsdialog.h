#ifndef DOCKERSETTINGSDIALOG_H
#define DOCKERSETTINGSDIALOG_H

#include <QDir>
#include <QDialog>

namespace Ui {
class DockerSettingsDialog;
}

class DockerSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DockerSettingsDialog(QWidget *parent = 0);
    ~DockerSettingsDialog();

private slots:
    void on_buttonBox_accepted();
    void on_pb_open_folder_clicked();

private:
    Ui::DockerSettingsDialog *ui;

    void read_settings();
    void write_settings();
};

#endif // DOCKERSETTINGSDIALOG_H
