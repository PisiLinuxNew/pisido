#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#ifdef USING_QT5
    #include <QtWidgets/QDialog>
#else
    #include <QDialog>
#endif

namespace Ui {
    class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
