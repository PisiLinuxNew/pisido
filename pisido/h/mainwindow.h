#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifdef USING_QT5
    #include <QtWidgets/QMainWindow>
    #include <QtWidgets/QListWidgetItem>
#else
    #include <QMainWindow>
    #include <QListWidgetItem>
#endif

#include <QMap>
#include <QMultiMap>
#include <QSettings>
#include <QDomDocument>
#include <QDir>

#include "pisi.h"
#include "archivewidget.h"
#include "translationwidget.h"

namespace Ui {
    class MainWindow;
}

class QsciScintilla;
class QsciLexerPython;
class QTermWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void init_package_name_completer();
    void check_package_dirs();
    void save_actions_editor_change();
    void complete_word();

    void append_archive(const QString & archive, const QString & sha1);
    void delete_archive(ArchiveWidget *);
    void delete_translation(TranslationWidget *);

    void append_file(const QString & path, const QString & file_type, bool permanent);

    void update_package_files();
    void package_files_process(const QString & dir);

    void clear_tableW_files();
    void clear_tableW_patches();
    void clear_tableW_aditional_files();
    void clear_tableW_history();
    void clear_archive_widgets();
    void clear_translation_widgets();

    void on_action_Change_Workspace_triggered();

    void on_action_Open_PISI_Packaging_Dir_triggered();

    void on_action_Configure_Application_triggered();

    void on_action_Help_triggered();

    void on_action_Open_PISI_Spec_File_triggered();
    void on_action_Open_Actions_API_Page_triggered();
    void on_action_Open_Workspace_triggered();

    void on_action_About_triggered();
    void on_action_About_Qt_triggered();

    void on_tb_delete_last_update_clicked();
    void on_tb_add_update_clicked();

    void on_action_Application_Language_triggered();

    void on_le_package_name_textChanged(const QString &text);
    void on_le_package_name_returnPressed();
    void on_tb_import_package_clicked();

    void on_le_license_textChanged(const QString & text);
    void on_le_is_a_textChanged(const QString & text);
    void on_combo_part_of_currentIndexChanged(const QString &  text);

    void on_le_homepage_textChanged(const QString & text);
    void on_pte_description_textChanged();
    void on_le_build_dependency_textChanged(const QString & text);
    void on_le_runtime_dependency_textChanged(const QString & text);

    void on_action_Reset_Fields_triggered();

    void on_combo_actions_template_currentIndexChanged(int index);
    void on_tb_zoom_in_clicked();
    void on_tb_zoom_out_clicked();

    void on_tb_patch_up_clicked();
    void on_tb_patch_down_clicked();

    void on_tb_edit_aditional_files_clicked();

    void on_tb_open_patches_dir_clicked();

    void on_tb_open_aditional_files_dir_clicked();

    void on_tb_open_install_dir_clicked();

    void on_tb_open_package_dir_clicked();

    void on_tb_add_label_clicked();

    void on_tb_delete_label_clicked();

    void on_tableW_files_itemSelectionChanged();

    void on_tb_add_archive_clicked();

    void on_tb_build_up_to_clicked();

    void on_tb_build_only_clicked();

    void on_tb_build_all_clicked();

    void on_tb_refresh_treeV_files_clicked();

    void on_tb_refresh_tableW_patches_clicked();

    void on_tb_refresh_tableW_aditional_files_clicked();

    void on_tb_add_translation_clicked();

    void on_btn_add_package_clicked(QString name="new package");

    void package_to_gui(int key=0) throw (QString);

    void on_btn_remove_package_clicked();

    void time_elapsed();

    void save_act_editor_conf();

    void get_act_editor_conf();

    void on_actionOpen_Log_triggered();

    void on_action_Run_Docker_triggered();

    void on_action_Update_Docker_triggered();

    void on_action_Clear_Docker_triggered();

    void on_tb_import_from_folder_clicked();

    void on_action_Find_docker_containers_triggered();

    void container_triggered(QAction *action);

    void on_action_Checkelf_triggered();

    void on_action_Exit_Container_triggered();
    void on_action_Root_triggered();
    void on_action_Start_Daemon_triggered();


protected:
    virtual void closeEvent(QCloseEvent * event);

private:
    Ui::MainWindow *ui;
    QSettings settings;
    QMap<int, QString> actions_templates_defaults;
    QMap<int, QString> actions_templates;
    QsciScintilla * actions_editor;
    QsciLexerPython * python_lexer;
    QTermWidget * w_terminal;
    QDomDocument dom_pspec;
    Pisi pisi;



    bool not_ask_workspace;

    QDir workspace_dir;
    QDir output_dir, archives_dir, packages_dir;
    QDir package_dir;
    QDir package_files_dir;
    QDir package_install_dir;

    QList<ArchiveWidget *> archive_widgets;
    QList<TranslationWidget *> translation_widgets;
    QTimer * workspace_dir_timer;
    QStringList workspace_package_names;
    QHash<QString, int> language_hash;

    QString package_name;
    QString homepage;
    QString licenses;
    QString is_a_s;
    QString part_of;
    QString summary;
    QString description;
    QString build_dependency;
    QString runtime_dependency;
    QMultiMap<int, QString> patches;
    QMultiMap<int, QString> temp_patches;
    QMap<QString, QMap<PisiSPBase::AFileAttr,QString> > aditional_files;
    QMap<QString, QMap<PisiSPBase::AFileAttr,QString> > temp_aditional_files;
    QMap<QString, QMap<QString, bool> > files;

    QListWidgetItem *lw_item;

    QTimer *timer;
    QTime time;

    QThread *thread;


    // <package_name     <lang , summary) > >
    QMap <QString, QMap<QString, QString > > translations;

    void apply_default_settings();
    void write_settings();
    void read_settings();

    QStringList get_file_strings(const QString & file_name);
    QString get_file_contents(const QString & file_name);
    bool save_text_file(const QString & file_name, const QString & data);

    PisiUpdate get_history_update(int row) throw(QString);
    void set_history_update(const PisiUpdate & update);
    void fill_tableW_patches();
    void fill_tableW_aditional_files();

    bool create_build_files();
    void call_pisi_build_command(const QString & build_step = QString());

    void pisi_to_gui() throw (QString);
    void pisi_from_gui() throw (QString);
    void save_package(int key);

    bool Zoom,start_process,first_run,docker_is_running,is_root;


};

#endif // MAINWINDOW_H
