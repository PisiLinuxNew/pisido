#ifndef REMOVECONTAINERSDIALOG_H
#define REMOVECONTAINERSDIALOG_H

#include <QDialog>

namespace Ui {
class RemoveContainersDialog;
}

class RemoveContainersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RemoveContainersDialog(QWidget *parent = 0);
    ~RemoveContainersDialog();

    void set_containers(QStringList Containers);
    QStringList get_selected_containers();

private:
    Ui::RemoveContainersDialog *ui;

private slots:
    void on_pb_Select_All_clicked();
};

#endif // REMOVECONTAINERSDIALOG_H
