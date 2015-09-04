#ifndef WORKSPACEDIALOG_H
#define WORKSPACEDIALOG_H

#ifdef USING_QT5
    #include <QtWidgets/QDialog>
#else
    #include <QDialog>
#endif

namespace Ui {
    class WorkspaceDialog;
}

class WorkspaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WorkspaceDialog(QWidget *parent = 0);
    ~WorkspaceDialog();

    QString get_workspace();
    bool get_not_ask_workspace();

private slots:
    void save_settings();
    void read_settings();

    void on_pb_browse_clicked();
    void on_le_workspace_textChanged(const QString &arg1);

    void on_check_not_ask_stateChanged(int arg1);

private:
    Ui::WorkspaceDialog *ui;
    QString workspace;
    bool not_ask_workspace;
};

#endif // WORKSPACEDIALOG_H
