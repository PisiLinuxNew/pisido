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
    QString get_output_dir();
    QString get_archives_dir();
    QString get_packages_dir();
    bool get_not_ask_workspace();

private slots:
    void save_settings();
    void read_settings();

    void on_pb_browse_clicked();
    void on_pb_browse_OD_clicked();
    void on_pb_browse_AD_clicked();
    void on_pb_browse_PD_clicked();
    void on_le_workspace_textChanged(const QString &arg1);

    void on_check_not_ask_stateChanged(int arg1);

private:
    Ui::WorkspaceDialog *ui;
    QString workspace, output_dir, archives_dir, packages_dir;
    bool not_ask_workspace;
};

#endif // WORKSPACEDIALOG_H
