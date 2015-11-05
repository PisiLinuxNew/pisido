#ifndef ARCHIVEWIDGET_H
#define ARCHIVEWIDGET_H

#ifdef USING_QT5
    #include <QtWidgets/QWidget>
#else
    #include <QWidget>
#endif

namespace Ui {
    class ArchiveWidget;
}

class ArchiveWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ArchiveWidget(QWidget * parent,
                           const QString & archive,
                           const QString & sha1,
                           const QStringList & types,
                           const QString & selected_type);
    ~ArchiveWidget();

    QString get_archive();
    QString get_sha1();
    QString get_type();

    void set_sha1(QString sha1);

signals:
    void delete_me(ArchiveWidget *);

private slots:
    void on_tb_delete_clicked();


private:
    Ui::ArchiveWidget *ui;
};

#endif // ARCHIVEWIDGET_H
