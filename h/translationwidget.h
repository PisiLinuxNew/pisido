#ifndef TRANSLATIONWIDGET_H
#define TRANSLATIONWIDGET_H

#ifdef USING_QT5
    #include <QtWidgets/QWidget>
#else
    #include <QWidget>
#endif

namespace Ui {
    class TranslationWidget;
}

class TranslationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TranslationWidget(
                            QWidget *parent,
                            const QString & language);
    ~TranslationWidget();

    QString get_summary();
    QString get_description();
    QString get_language();

    void set_summary(const QString & text);
    void set_description(const QString & text);

signals:
    void delete_me(TranslationWidget *);

private slots:
    void on_tb_delete_clicked();

private:
    Ui::TranslationWidget *ui;
};

#endif // TRANSLATIONWIDGET_H
