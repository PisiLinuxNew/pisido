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

private:
    Ui::RemoveContainersDialog *ui;
};

#endif // REMOVECONTAINERSDIALOG_H
