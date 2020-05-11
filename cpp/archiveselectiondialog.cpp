#include "archiveselectiondialog.h"
#include "ui_archiveselectiondialog.h"

#include <QProcess>
#include <QDesktopServices>

#ifdef USING_QT5
    #include <QStandardPaths>
    #include <QtWidgets/QFileDialog>
    #include <QtWidgets/QMessageBox>
#else
    #include <QFileDialog>
    #include <QMessageBox>
#endif

ArchiveSelectionDialog::ArchiveSelectionDialog(QWidget *parent, ArchiveType type) :
    QDialog(parent),
    archive_type(type),
    ui(new Ui::ArchiveSelectionDialog)
{
    ui->setupUi(this);

    if(archive_type == COMPRESSED){
        ui->le_archive->setEnabled(false);
        ui->le_sha1->setEnabled(false);
        ui->lbl_sha1->setEnabled(false);
    }
    else{
        ui->pb_browse->setEnabled(false);
    }
}

ArchiveSelectionDialog::~ArchiveSelectionDialog()
{
    delete ui;
}

void ArchiveSelectionDialog::on_pb_browse_clicked()
{
    QString selection;

    if(archive_type == COMPRESSED)
    {
        QString filter = tr("Compressed Files (*.targz *.tarbz2 *.tarlzma *.tarxz *.tarZ *.tar *.zip *.gz *.gzip *.bz2 *.bzip2 *.lzma *.xz *.binary)");
        QString dir;
        if(previous_directory.isEmpty())
#ifdef USING_QT5
            dir = QStandardPaths::standardLocations(QStandardPaths::HomeLocation)[0];
#else
            dir = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
#endif

        else
            dir = previous_directory;
        selection = QFileDialog::getOpenFileName(this, tr("Select Compressed File"), dir,filter);
    }

    if(selection.isEmpty())
        return;

    previous_directory = QDir(selection).absolutePath();

    ui->le_archive->setText(selection);
    sha1 = get_sha1sum(selection);
    ui->le_sha1->setText(sha1);
}

void ArchiveSelectionDialog::on_le_sha1_editingFinished()
{
    sha1 = ui->le_sha1->text();
}

QString ArchiveSelectionDialog::get_sha1sum(const QString & file_name)
{
    QProcess system;
    system.start("sha1sum", QStringList() << file_name);
    if( ! system.waitForFinished())
    {
        QMessageBox::critical(this, tr("Error"), tr("sha1sum process timeout within 30 seconds. Please set sha1 value yourself !"));
        return QString();
    }
    QString sha1(system.readAll());
    return sha1.split(' ').first();
}

QString ArchiveSelectionDialog::get_archive()
{
    return ui->le_archive->text();
}

QString ArchiveSelectionDialog::get_sha1()
{
    return sha1;
}

void ArchiveSelectionDialog::on_buttonBox_accepted()
{
    if(ui->le_archive->text().isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("Empty archive !"));
        return;
    }
    if(ui->le_sha1->text().isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("Empty sha1 !"));
        return;
    }
    accept();
}

QString ArchiveSelectionDialog::get_previous_directory()
{
    return previous_directory;
}

void ArchiveSelectionDialog::set_previous_directory(QString p)
{
    previous_directory = p;
}
