#ifndef ARCHIVESELECTIONDIALOG_H
#define ARCHIVESELECTIONDIALOG_H

#ifdef USING_QT5
    #include <QtWidgets/QDialog>
#else
    #include <QDialog>
#endif

namespace Ui {
    class ArchiveSelectionDialog;
}

class ArchiveSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    enum ArchiveType {
        COMPRESSED,
        URL
    };

    explicit ArchiveSelectionDialog(QWidget * parent, ArchiveType type);
    ~ArchiveSelectionDialog();

    QString get_archive();
    QString get_sha1();

    QString get_previous_directory();
    void set_previous_directory(QString p);

private slots:
    void on_pb_browse_clicked();
    void on_le_sha1_editingFinished();

    void on_buttonBox_accepted();

private:
    ArchiveType archive_type;
    Ui::ArchiveSelectionDialog *ui;
    QString previous_directory;

    QString sha1;

    QString get_sha1sum(const QString & file_name);
};

#endif // ARCHIVESELECTIONDIALOG_H
