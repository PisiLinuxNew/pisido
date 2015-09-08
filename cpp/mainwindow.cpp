#include "mainwindow.h"
#include "ui_mainwindow.h"


#ifdef USING_QT5
    #include <QtWidgets/QFileDialog>
    #include <QtWidgets/QMessageBox>
    #include <QtWidgets/QShortcut>
    #include <qt5/Qsci/qsciscintilla.h>
    #include <qt5/Qsci/qscilexerpython.h>
    #include <qtermwidget5/qtermwidget.h>

#else
    #include <QFileDialog>
    #include <QMessageBox>
    #include <QShortcut>
    #include <Qsci/qsciscintilla.h>
    #include <Qsci/qscilexerpython.h>
    #include <qtermwidget4/qtermwidget.h>
#endif

#include <unistd.h>
#include <QDebug>
#include <QCloseEvent>
#include <QDate>
#include <QDesktopServices>
#include <QProcess>
#include <QUrl>
#include <QCryptographicHash>
#include <QTimer>
#include <QDirIterator>
#include <QThread>


#include "aboutdialog.h"
#include "addinstallfilelabeldialog.h"
#include "addupdatedialog.h"
#include "aditionalfiledialog.h"
#include "archiveselectiondialog.h"
#include "configurationdialog.h"
#include "directorymodel.h"
#include "languagedialog.h"
#include "workspacedialog.h"
#include "multicompleter.h"
#include "dockersettingsdialog.h"
#include "removecontainersdialog.h"

#define DEFAULT_PATCH_LEVEL 1
#define PACKAGE_NAME_REFRESH_INTERVAL 2000
#define COMBO_ACTIONS_IMPORT_INDEX 9

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    not_ask_workspace(false),
    workspace_dir(QDir::root()),
    package_dir(QDir::root()),
    package_files_dir(QDir::root()),
    package_install_dir(QDir::root())
{
    ui->setupUi(this);

    ui->menuDocker->menuAction()->setVisible(false);

    ui->tBar_docker->insertWidget(ui->action_Checkelf,ui->tb_docker_container);
    connect(ui->tb_docker_container,
            SIGNAL(triggered(QAction*)),
            ui->tb_docker_container,
            SLOT(setDefaultAction(QAction*))
            );

    addDockWidget(Qt::RightDockWidgetArea, ui->dw_history);
    // tabify for first run
    tabifyDockWidget(ui->dw_actions, ui->dw_install_files);
    tabifyDockWidget(ui->dw_install_files, ui->dw_aditional_files);
    tabifyDockWidget(ui->dw_aditional_files, ui->dw_patches);
    tabifyDockWidget(ui->dw_patches, ui->dw_history);
    ui->dw_actions->raise();
    // fill view menu
    QAction * a_dw_actions = ui->dw_actions->toggleViewAction();
    QAction * a_dw_install_files = ui->dw_install_files->toggleViewAction();
    QAction * a_dw_aditional_files = ui->dw_aditional_files->toggleViewAction();
    QAction * a_dw_patches = ui->dw_patches->toggleViewAction();
    QAction * a_dw_history = ui->dw_history->toggleViewAction();
    QAction * a_dw_build = ui->dw_build->toggleViewAction();
    QAction * a_tBar_operations = ui->tBar_operations->toggleViewAction();
    QAction * a_tBar_view = ui->tBar_view->toggleViewAction();
    QAction * a_tBar_help = ui->tBar_help->toggleViewAction();
    a_dw_actions->setIcon(QIcon(":/images/actions.png"));
    a_dw_install_files->setIcon(QIcon(":/images/install-files.png"));
    a_dw_aditional_files->setIcon(QIcon(":/images/aditional-files.png"));
    a_dw_patches->setIcon(QIcon(":/images/patches.png"));
    a_dw_history->setIcon(QIcon(":/images/history.png"));
    a_dw_build->setIcon(QIcon(":/images/build.png"));
    a_tBar_operations->setIcon(QIcon(":/images/operations.png"));
    a_tBar_view->setIcon(QIcon(":/images/toolbar.png"));
    a_tBar_help->setIcon(QIcon(":/images/help.png"));
    a_dw_actions->setStatusTip(ui->dw_actions->toolTip());
    a_dw_install_files->setStatusTip(ui->dw_install_files->toolTip());
    a_dw_aditional_files->setStatusTip(ui->dw_aditional_files->toolTip());
    a_dw_patches->setStatusTip(ui->dw_patches->toolTip());
    a_dw_history->setStatusTip(ui->dw_history->toolTip());
    a_dw_build->setStatusTip(ui->dw_build->toolTip());
    a_tBar_operations->setStatusTip(tr("Show or hide %1").arg(ui->tBar_operations->windowTitle()));
    a_tBar_view->setStatusTip(tr("Show or hide %1").arg(ui->tBar_view->windowTitle()));
    a_tBar_help->setStatusTip(tr("Show or hide %1").arg(ui->tBar_help->windowTitle()));
    ui->menu_View->addAction(a_dw_actions);
    ui->menu_View->addAction(a_dw_install_files);
    ui->menu_View->addAction(a_dw_aditional_files);
    ui->menu_View->addAction(a_dw_patches);
    ui->menu_View->addAction(a_dw_history);
    ui->menu_View->addAction(a_dw_build);
    ui->menu_View->addSeparator();
    ui->menu_View->addAction(a_tBar_operations);
    ui->menu_View->addAction(a_tBar_view);
    ui->menu_View->addAction(a_tBar_help);
    // fill view tool bar
    ui->tBar_view->addAction(a_dw_actions);
    ui->tBar_view->addAction(a_dw_install_files);
    ui->tBar_view->addAction(a_dw_aditional_files);
    ui->tBar_view->addAction(a_dw_patches);
    ui->tBar_view->addAction(a_dw_history);
    ui->tBar_view->addAction(a_dw_build);

    // initialize scintilla
    actions_editor = new QsciScintilla(ui->dw_actions_contents);
    actions_editor->setIndentationsUseTabs(false);
    actions_editor->setTabWidth(4);
    actions_editor->setMarginLineNumbers(1, true);
    actions_editor->setMarginWidth(1, "12345");
    ui->dw_actions_contents->layout()->addWidget(actions_editor);
    // add python highlight support
    python_lexer = new QsciLexerPython(this);
    python_lexer->setIndentationWarning(QsciLexerPython::Inconsistent);
    QFont f("Droid Sans Mono");
    f.setPointSize(10);
    python_lexer->setFont(f);
    actions_editor->setLexer(python_lexer);
    // autocompletion related bussiness
    actions_editor->setAutoCompletionSource(QsciScintilla::AcsDocument);
    actions_editor->setAutoCompletionCaseSensitivity(false);
    actions_editor->setAutoCompletionThreshold(1);
    actions_editor->setAutoCompletionShowSingle(false);
    get_act_editor_conf();
    // manually complete
    QShortcut * sc_auto_comp = new QShortcut(QKeySequence("Ctrl+Space"), this);
    connect(sc_auto_comp, SIGNAL(activated()), this, SLOT(complete_word()));
    // to save editor contents
    connect(actions_editor, SIGNAL(textChanged()), SLOT(save_actions_editor_change()));

    // fill comboboxes
    MultiCompleter * license_completer = new MultiCompleter(get_file_strings(":/files/license"), this);
    ui->le_license->setCompleter(license_completer);
    MultiCompleter * is_a_completer = new MultiCompleter(get_file_strings(":/files/is_a"), this);
    ui->le_is_a->setCompleter(is_a_completer);
    ui->combo_part_of->addItem("");
    ui->combo_part_of->addItems(get_file_strings(":/files/part_of"));

    apply_default_settings();

    actions_templates_defaults.clear();
    actions_templates_defaults[0] = get_file_contents(":/files/actions_template_auto.py");
    actions_templates_defaults[1] = get_file_contents(":/files/actions_template_cmake.py");
    actions_templates_defaults[2] = get_file_contents(":/files/actions_template_java.py");
    actions_templates_defaults[3] = get_file_contents(":/files/actions_template_kde4.py");
    actions_templates_defaults[4] = get_file_contents(":/files/actions_template_kde5.py");
    actions_templates_defaults[5] = get_file_contents(":/files/actions_template_python.py");
    actions_templates_defaults[6] = get_file_contents(":/files/actions_template_qt4.py");
    actions_templates_defaults[7] = get_file_contents(":/files/actions_template_qt5.py");
    actions_templates_defaults[8] = get_file_contents(":/files/actions_template_scons.py");
    actions_templates_defaults[COMBO_ACTIONS_IMPORT_INDEX] = "";
    actions_templates = actions_templates_defaults;
    actions_editor->setText(actions_templates[ui->combo_actions_template->currentIndex()]);

    DirectoryModel * model = new DirectoryModel(QDir("/invaliddirectory"),this);
    ui->treeV_files->setModel(model);

    // fill translation list
    QStringList language_list;
    for (int i = 0; i < QLocale::LastLanguage; ++i) {
        QString language = QLocale::languageToString((QLocale::Language) i);
        if( ! language.isEmpty()){
            if( ! language_list.contains(language)){
                language_list.append(language);
                language_hash.insert(language, i);
            }
        }
    }
    language_list.sort();
    ui->combo_translation->addItems(language_list);

    read_settings();

    if( ! not_ask_workspace){
        WorkspaceDialog wd(this);
        if(wd.exec() == QDialog::Accepted){
            workspace_dir = QDir(wd.get_workspace());
            output_dir = QDir(wd.get_output_dir());
            archives_dir = QDir(wd.get_archives_dir());
            packages_dir = QDir(wd.get_packages_dir());
            not_ask_workspace = wd.get_not_ask_workspace();
        }
        else{
            QTimer::singleShot(0, qApp, SLOT(quit()));
        }
    }

    if( ! workspace_dir.exists()){
        QMessageBox::critical(this,
                              tr("No Workspace"),
                              tr("Workspace does not exists !"));
        not_ask_workspace = false;
        write_settings();
        QTimer::singleShot(0, qApp, SLOT(quit()));
    }

    workspace_dir_timer = new QTimer(this);
    workspace_dir_timer->start(PACKAGE_NAME_REFRESH_INTERVAL);
    connect(workspace_dir_timer, SIGNAL(timeout()), this, SLOT(init_package_name_completer()));
    connect(workspace_dir_timer, SIGNAL(timeout()), this, SLOT(check_package_dirs()));

    w_terminal = new QTermWidget(1, ui->dw_build);
    QFont terminal_font = QApplication::font();
    terminal_font.setFamily("Monospace");
    terminal_font.setPointSize(9);
    w_terminal->setTerminalFont(terminal_font);
    //w_terminal->setColorScheme(COLOR_SCHEME_WHITE_ON_BLACK);
    w_terminal->setScrollBarPosition(QTermWidget::ScrollBarRight);
    QHBoxLayout * build_layout = qobject_cast<QHBoxLayout *>(ui->dw_build_contents->layout());
    if(build_layout){
        build_layout->insertWidget(0, w_terminal);
        w_terminal->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    }
    else{
        qDebug() << "Problem with inserting terminal widget !";
    }
    w_terminal->setWorkingDirectory(workspace_dir.absolutePath());
    w_terminal->changeDir(workspace_dir.absolutePath());

    ui->le_package_name->setFocus();

    first_run=true;

    connect(ui->btn_add_package,SIGNAL(clicked()),this,SLOT(btn_add_package_clicked()));

    connect(ui->tb_zoom_in,SIGNAL(clicked()),this,SLOT(save_act_editor_conf()));
    connect(ui->tb_zoom_out,SIGNAL(clicked()),this,SLOT(save_act_editor_conf()));

    //connect(ui->actionOpen_Log,SIGNAL(triggered()),SLOT(on_actionOpen_Log_triggered()));

    connect(ui->tb_docker_container,SIGNAL(triggered(QAction*)),this,SLOT(container_triggered(QAction*)));

    //connect(ui->le_package_name,SIGNAL(returnPressed()),this,SLOT(on_le_package_name_textChanged(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
 {
    QString exit_cmd = "exit\n";
    w_terminal->sendText(exit_cmd);

     write_settings();
     event->accept();
 }

/** Helper function to load each line from file to QStringList. */

QStringList MainWindow::get_file_strings(const QString & file_name)
{
    QFile file(file_name);
    if( ! file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::critical(this, tr("Error"), tr("Can not open \"%1\" resource !").arg(file_name));
        return QStringList();
    }
    QStringList list;
    QTextStream stream(&file);
    while( ! stream.atEnd()){
        QString line = stream.readLine().trimmed();
        if( ! line.isEmpty())
            list.append(line);
    }
    file.close();
    return list;
}

QString MainWindow::get_file_contents(const QString & file_name)
{
    QFile file(file_name);
    QString contents;
    if(file.open(QFile::ReadOnly | QFile::Text)){
        contents = file.readAll();
    }
    else{
        qDebug() << "Can not load contents of file: " << file_name;
    }
    return contents;
}

bool MainWindow::save_text_file(const QString &file_name, const QString &data)
{
    QFile file(file_name);
    if( ! file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox::critical(this, tr("Error"), tr("Can not open %1 file to write.").arg(file_name));
        return false;
    }
    QTextStream writer(&file);
    writer << data;
    return true;
}

void MainWindow::init_package_name_completer()
{
    if( ! workspace_dir.exists()){
        QCompleter * old_completer = ui->le_package_name->completer();
        ui->le_package_name->setCompleter(0);
        if(old_completer)
            delete old_completer;
        return;
    }

    QStringList list = workspace_dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot|QDir::NoSymLinks, QDir::Name);
    if(list != workspace_package_names){
        workspace_package_names = list;
        QCompleter * completer = new QCompleter(workspace_package_names, this);
        QCompleter * old_completer = ui->le_package_name->completer();
        ui->le_package_name->setCompleter(completer);
        if(old_completer)
            delete old_completer;
    }
}

void MainWindow::check_package_dirs()
{
    if( ! package_dir.isRoot() && package_dir.exists())
    {
        ui->tb_open_package_dir->setEnabled(true);
    }
    else{
        ui->tb_open_package_dir->setEnabled(false);
    }

    if(QFile::exists(package_dir.absoluteFilePath("pspec.xml"))
            && QFile::exists(package_dir.absoluteFilePath("actions.py"))){
        ui->tb_import_package->setEnabled(true);
    }
    else{
        ui->tb_import_package->setEnabled(false);
    }
}

void MainWindow::on_action_Change_Workspace_triggered()
{
    WorkspaceDialog wd(this);
    if(wd.exec() == QDialog::Accepted){
        workspace_dir = QDir(wd.get_workspace());
        not_ask_workspace = wd.get_not_ask_workspace();
        w_terminal->setWorkingDirectory(workspace_dir.absolutePath());
        w_terminal->changeDir(workspace_dir.absolutePath());
        on_le_package_name_textChanged(ui->le_package_name->text());
        init_package_name_completer();
    }
}

void MainWindow::on_action_About_triggered()
{
    AboutDialog ad(this);
    ad.exec();
}

void MainWindow::on_action_About_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_action_Configure_Application_triggered()
{
    ConfigurationDialog cd(this);
    if(cd.exec() == QDialog::Accepted){
        // apply settings if needed.
    }
}

void MainWindow::on_action_Application_Language_triggered()
{
    QDir lang_dir(PISIDO_LANG_DIR);
    QStringList file_list = lang_dir.entryList(
                            QStringList() << "*.qm",
                            QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Readable,
                            QDir::Name
                            );
    if(file_list.isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("There are no translation files in : %1").arg(lang_dir.absolutePath()));
        return;
    }

    settings.beginGroup("configuration");
    QString lang_code = settings.value("language", QString()).toString();
    settings.endGroup();
    QString current_lang;

    QMap<QString, QString> lang_map;
    lang_map[""] = QString("");
    foreach (QString file, file_list){
        file.remove(".qm");
        QStringList f = file.split('_');
        f.removeFirst();
        QString loc_abr = f.join("_");
        QLocale loc(loc_abr);
        lang_map[QLocale::languageToString(loc.language())] = loc_abr;
        if(loc_abr == lang_code)
            current_lang = QLocale::languageToString(loc.language());
    }

    LanguageDialog ld(lang_map.keys(), current_lang, this);
    int ret = ld.exec();
    if(ret == QDialog::Accepted){
        QString lang = ld.selectedLanguage();
        QString lang_code = lang_map[lang];
        settings.beginGroup("configuration");
        settings.setValue("language", lang_code);
        settings.endGroup();
    }
}

void MainWindow::on_action_Open_Actions_API_Page_triggered()
{
    settings.beginGroup("configuration");
    QString action_api_page = settings.value("action_api_page").toString();
    if( ! action_api_page.isEmpty()){
        QDesktopServices::openUrl(QUrl::fromUserInput(action_api_page));
    }
    settings.endGroup();
}

void MainWindow::on_action_Open_PISI_Spec_File_triggered()
{
    settings.beginGroup("configuration");
    QString pisi_spec = settings.value("pisi_spec").toString();
    if( ! pisi_spec.isEmpty()){
        QDesktopServices::openUrl(QUrl::fromUserInput(pisi_spec));
    }
    settings.endGroup();
}

void MainWindow::on_action_Open_PISI_Packaging_Dir_triggered()
{
    settings.beginGroup("configuration");
    QString pisi_packaging_dir = settings.value("pisi_packaging_dir").toString();
    if( ! pisi_packaging_dir.isEmpty()){
        QDir dir(pisi_packaging_dir);
        if(dir.exists())
            QDesktopServices::openUrl(QUrl::fromLocalFile(pisi_packaging_dir));
    }
    settings.endGroup();
}

void MainWindow::on_action_Open_Workspace_triggered()
{
    if( ! workspace_dir.isRoot() && workspace_dir.exists())
        QDesktopServices::openUrl(QUrl::fromLocalFile(workspace_dir.absolutePath()));
}

void MainWindow::on_action_Help_triggered()
{
    QDir help_dir(PISIDO_HELP_DIR);
    settings.beginGroup("configuration");
    QString lang_code = settings.value("language", QString()).toString();
    settings.endGroup();
    if(lang_code.isEmpty())
        lang_code = QLocale::system().name();
    QString help_file = help_dir.absoluteFilePath(QString("help_%1.pdf").arg(lang_code));
    QDesktopServices::openUrl(QUrl::fromLocalFile(help_file));
}

void MainWindow::apply_default_settings()
{
    // set default settings, needed for first run
    settings.beginGroup( "configuration" );
    QString action_api_page = settings.value("action_api_page", tr("http://wiki.pisilinux.org/tr/index.php?title=ActionsAPI")).toString();
    //pspec api sayfası yenilenecek
    QString pisi_spec = settings.value("pisi_spec", tr("http://svn.pardus.org.tr/uludag/trunk/pisi/pisi-spec.rng")).toString();
    QString pisi_packaging_dir = settings.value("pisi_packaging_dir", QString("/var/pisi/")).toString();

    settings.setValue("action_api_page", action_api_page);
    settings.setValue("pisi_spec", pisi_spec);
    settings.setValue("pisi_packaging_dir", pisi_packaging_dir);
    settings.endGroup();
}

void MainWindow::write_settings()
{
    settings.beginGroup("main");
    settings.setValue("window_state", saveState());
    settings.setValue("window_geometry", saveGeometry());
    settings.setValue("workspace", workspace_dir.absolutePath());
    settings.setValue("output_dir", output_dir.absolutePath());
    settings.setValue("archives_dir", archives_dir.absolutePath());
    settings.setValue("packages_dir", packages_dir.absolutePath());
    settings.setValue("not_ask_workspace", not_ask_workspace);
    settings.endGroup();
}

void MainWindow::read_settings()
{
    settings.beginGroup("main");
    restoreState(settings.value("window_state").toByteArray());
    restoreGeometry(settings.value("window_geometry").toByteArray());
    workspace_dir = settings.value("workspace").toString();
    output_dir = settings.value("output_dir").toString();
    archives_dir = settings.value("archives_dir").toString();
    packages_dir = settings.value("packages_dir").toString();
    not_ask_workspace = settings.value("not_ask_workspace", false).toBool();
    settings.endGroup();
}

void MainWindow::on_action_Reset_Fields_triggered()
{
    ui->le_package_name->clear();
    ui->le_homepage->clear();
    ui->le_license->clear();
    ui->le_is_a->clear();
    ui->combo_part_of->setCurrentIndex(0);
    ui->le_summary->clear();
    ui->pte_description->clear();
    ui->le_build_dependency->clear();
    ui->le_runtime_dependency->clear();
    ui->lw_packages->clear();
    ui->lw_packages->addItem("Package name");

//    These will be cleared after gui change :
//      package_name, homepage, licenses, is_a_s, part_of, summary,
//      description, build_dependency, runtime_dependency
//    These will be cleared after le_package_name clear :
//      package_files_watcher, aditional_files, patches

    clear_archive_widgets();
    clear_translation_widgets();

    actions_templates.clear();
    actions_templates = actions_templates_defaults;
    actions_editor->setText(actions_templates[ui->combo_actions_template->currentIndex()]);

    clear_tableW_history();

    clear_tableW_files();
    files.clear();

    DirectoryModel * model = qobject_cast<DirectoryModel*>(ui->treeV_files->model());
    if(model){
        model->clear();
    }
    ui->le_package_install_dir->clear();


    pisi.clear();
    dom_pspec.clear();
}

PisiUpdate MainWindow::get_history_update(int row) throw(QString)
{
    PisiUpdate update;
    if(ui->tableW_history->itemAt(row, 0) == 0)
        return update;
    bool ok = false;
    int release = ui->tableW_history->item(row,0)->text().toInt(&ok);
    if( ! ok)
        throw tr("Error at conversion release string to integer !");
    update.set_release(release);
    update.set_date(QDate::fromString(ui->tableW_history->item(row, 1)->text(),"dd.MM.yyyy"));
    update.set_version(ui->tableW_history->item(row, 2)->text());
    update.set_comment(ui->tableW_history->item(row, 3)->text());
    update.set_packager_name(ui->tableW_history->item(row, 4)->text());
    update.set_packager_email(ui->tableW_history->item(row, 5)->text());
    return update;
}

void MainWindow::set_history_update(const PisiUpdate &update)
{
    QTableWidgetItem * item_release = new QTableWidgetItem(QString::number(update.get_release()));
    QTableWidgetItem * item_date = new QTableWidgetItem(update.get_date().toString("dd.MM.yyyy"));
    QTableWidgetItem * item_version = new QTableWidgetItem(update.get_version());
    QTableWidgetItem * item_comment = new QTableWidgetItem(update.get_comment());
    QTableWidgetItem * item_name = new QTableWidgetItem(update.get_packager_name());
    QTableWidgetItem * item_email = new QTableWidgetItem(update.get_packager_email());
    int row = 0;
    ui->tableW_history->insertRow(row);
    ui->tableW_history->setItem(row, 0, item_release);
    ui->tableW_history->setItem(row, 1, item_date);
    ui->tableW_history->setItem(row, 2, item_version);
    ui->tableW_history->setItem(row, 3, item_comment);
    ui->tableW_history->setItem(row, 4, item_name);
    ui->tableW_history->setItem(row, 5, item_email);
}

void MainWindow::on_tb_delete_last_update_clicked()
{
    if(ui->tableW_history->rowCount()>0)
        ui->tableW_history->removeRow(0);
}

void MainWindow::on_tb_add_update_clicked()
{
    AddUpdateDialog ud(this);
    if(ud.exec() == QDialog::Accepted){
        QString release;
        if(ui->tableW_history->rowCount() > 0)
            release = QString::number(ui->tableW_history->item(0,0)->text().toInt()+1);
        else
            release = "1";
        QTableWidgetItem * item_release = new QTableWidgetItem(release);
        QTableWidgetItem * item_date = new QTableWidgetItem(ud.get_date());
        QTableWidgetItem * item_version = new QTableWidgetItem(ud.get_version());
        QTableWidgetItem * item_comment = new QTableWidgetItem(ud.get_comment());
        QTableWidgetItem * item_name = new QTableWidgetItem(ud.get_packager_name());
        QTableWidgetItem * item_email = new QTableWidgetItem(ud.get_packager_email());
        int row = 0;
        ui->tableW_history->insertRow(row);
        ui->tableW_history->setItem(row, 0, item_release);
        ui->tableW_history->setItem(row, 1, item_date);
        ui->tableW_history->setItem(row, 2, item_version);
        ui->tableW_history->setItem(row, 3, item_comment);
        ui->tableW_history->setItem(row, 4, item_name);
        ui->tableW_history->setItem(row, 5, item_email);
    }
}

void MainWindow::clear_tableW_history()
{
    int row_count = ui->tableW_history->rowCount();
    for(int i=0; i<row_count; ++i){
        ui->tableW_history->removeRow(0);
    }
}

void MainWindow::on_le_package_name_returnPressed()
{
    on_tb_import_package_clicked();
}

void MainWindow::on_le_package_name_textChanged(const QString &text)
{
    if(text.isEmpty())
    {
        package_name.clear();
        package_dir = QDir::root();
        package_files_dir = QDir::root();
        package_install_dir = QDir::root();
    }
    else
    {
        package_name = text.trimmed();/*split("/").last();*/
        package_dir = QDir(workspace_dir.absoluteFilePath(package_name));
        package_files_dir = QDir(package_dir.absoluteFilePath("files/"));

        package_name = text.split("/").last();

        ui->lw_packages->item(0)->setText(package_name);

        QString v = pisi.get_last_update().get_version();
        int r = pisi.get_last_update().get_release();
        if( ! v.isEmpty() && r > 0){
            package_install_dir = QDir(QString("/var/pisi/%1-%2-%3/install/")
                                       .arg(package_name).arg(v).arg(r)
                                       );
        }
        else{
            package_install_dir = QDir::root();
        }
    }

    if(package_files_dir.isRoot()){
        ui->le_package_patches_dir->setText("");
        ui->le_package_aditionals_dir->setText("");
    }
    else{
        ui->le_package_patches_dir->setText(package_files_dir.absolutePath());
        ui->le_package_aditionals_dir->setText(package_files_dir.absolutePath());
    }

    if(package_install_dir.isRoot()){
        ui->le_package_install_dir->setText("");
        ui->tb_refresh_treeV_files->setEnabled(false);
        ui->tb_open_install_dir->setEnabled(false);
    }
    else{
        ui->le_package_install_dir->setText(package_install_dir.absolutePath());
        ui->tb_refresh_treeV_files->setEnabled(true);
        ui->tb_open_install_dir->setEnabled(true);
    }
    // to handle text change event
    update_package_files();
}



void MainWindow::on_le_homepage_textChanged(const QString & text)
{
    homepage = text.trimmed();
}

void MainWindow::on_le_build_dependency_textChanged(const QString & text)
{
    build_dependency = text.trimmed();
}

void MainWindow::on_le_runtime_dependency_textChanged(const QString & text)
{
    runtime_dependency = text.trimmed();
}

void MainWindow::on_le_license_textChanged(const QString & text)
{
    licenses = text.trimmed();
}

void MainWindow::on_le_is_a_textChanged(const QString & text)
{
    is_a_s = text.trimmed();
}

void MainWindow::on_combo_part_of_currentIndexChanged(const QString & text)
{
    part_of = text.trimmed();
}

void MainWindow::on_pte_description_textChanged()
{
    description = ui->pte_description->toPlainText().trimmed();
}




void MainWindow::on_combo_actions_template_currentIndexChanged(int index)
{
    actions_editor->setText(actions_templates[index]);
}

void MainWindow::save_actions_editor_change()
{
    int current_index = ui->combo_actions_template->currentIndex();
    actions_templates[current_index] = actions_editor->text();
}

void MainWindow::on_tb_zoom_in_clicked()
{
    actions_editor->zoomIn();
    Zoom=true;
}

void MainWindow::on_tb_zoom_out_clicked()
{
    actions_editor->zoomOut();
    Zoom = false;
}

void MainWindow::complete_word()
{
    if(actions_editor->hasFocus())
        actions_editor->autoCompleteFromDocument();
}

void MainWindow::update_package_files()
{
    if( ! package_files_dir.isRoot() && package_files_dir.exists())
    {
        ui->tb_open_aditional_files_dir->setEnabled(true);
        ui->tb_open_patches_dir->setEnabled(true);

        clear_tableW_patches();
        temp_patches.clear();
        clear_tableW_aditional_files();
        temp_aditional_files.clear();

        package_files_process(package_files_dir.absolutePath());

        QDirIterator it(
                    package_files_dir.absolutePath(),
                    QDir::Dirs|QDir::NoDotAndDotDot|QDir::NoSymLinks|QDir::Readable,
                    QDirIterator::Subdirectories);

        while(it.hasNext()){
            QString sub = it.next();
            package_files_process(sub);
        }

        patches = temp_patches;
        temp_patches.clear();
        fill_tableW_patches();

        aditional_files = temp_aditional_files;
        temp_aditional_files.clear();
        fill_tableW_aditional_files();
    }
    else{
        clear_tableW_patches();
        clear_tableW_aditional_files();
        ui->tb_open_aditional_files_dir->setEnabled(false);
        ui->tb_open_patches_dir->setEnabled(false);
    }
}

/** patches must filled before call ! */

void MainWindow::fill_tableW_patches()
{
    QMultiMap<int, QString>::iterator patch_it = patches.begin();
    int patch_index = 0;
    while(patch_it != patches.end())
    {
        ui->tableW_patches->insertRow(patch_index);
        ui->tableW_patches->setItem(patch_index, 0, new QTableWidgetItem(QString::number(patch_it.key())));
        ui->tableW_patches->setItem(patch_index, 1, new QTableWidgetItem(patch_it.value()));
        patch_index++;
        patch_it++;
    }
    ui->tableW_patches->sortByColumn(0, Qt::AscendingOrder);
}

/** aditional_files must filled before call ! */
void MainWindow::fill_tableW_aditional_files()
{
    QStringList a_files_keys = aditional_files.keys();
    int a_file_index = 0;
    foreach (QString a_file, a_files_keys) {
        QMap<PisiSPBase::AFileAttr,QString> attr = aditional_files.value(a_file);
        ui->tableW_aditional_files->insertRow(a_file_index);
        ui->tableW_aditional_files->setItem(a_file_index, 0, new QTableWidgetItem(a_file));
        ui->tableW_aditional_files->setItem(a_file_index, 1, new QTableWidgetItem(attr.value(PisiSPBase::TARGET)));
        ui->tableW_aditional_files->setItem(a_file_index, 2, new QTableWidgetItem(attr.value(PisiSPBase::PERMISSION)));
        ui->tableW_aditional_files->setItem(a_file_index, 3, new QTableWidgetItem(attr.value(PisiSPBase::OWNER)));
        ui->tableW_aditional_files->setItem(a_file_index, 4, new QTableWidgetItem(attr.value(PisiSPBase::GROUP)));
        a_file_index++;
    }
}

void MainWindow::package_files_process(const QString & dir)
{
    QDir sub_dir(dir);
    QFileInfoList files_info_list = sub_dir.entryInfoList(
                QDir::NoDotAndDotDot | QDir::Files | QDir::NoSymLinks | QDir::Readable,
                QDir::Name);

    QStringList patches_values = patches.values();

    foreach (QFileInfo file_info, files_info_list) {
        QString relative_file_path = file_info.absoluteFilePath().remove(package_files_dir.absolutePath() + QDir::separator());
        if(file_info.suffix() == "patch")
        {
            if(patches_values.contains(relative_file_path)){
                temp_patches.insert(patches.key(relative_file_path), relative_file_path);
            }
            else{
                temp_patches.insert(DEFAULT_PATCH_LEVEL, relative_file_path);
            }
        }
        else{
            if(aditional_files.contains(relative_file_path)){
                temp_aditional_files.insert(relative_file_path, aditional_files.value(relative_file_path));
            }
            else{
                QMap<PisiSPBase::AFileAttr,QString> attr;
                attr.insert(PisiSPBase::TARGET, QString("/usr/share/%1/%2").arg("__package_name__").arg(file_info.fileName()));
                attr.insert(PisiSPBase::PERMISSION, QString("0644"));
                attr.insert(PisiSPBase::OWNER, QString("root"));
                attr.insert(PisiSPBase::GROUP, QString("root"));
                temp_aditional_files.insert(relative_file_path, attr);
            }
        }
    }
}

void MainWindow::on_tb_refresh_treeV_files_clicked()
{
    DirectoryModel * model = qobject_cast<DirectoryModel *>(ui->treeV_files->model());
    if(model){
        if( ! package_install_dir.isRoot() && package_install_dir.exists()){
            model->setRootDirectory(package_install_dir);
            QTimer::singleShot(500, ui->treeV_files, SLOT(expandAll()));
        }
        else{
            model->clear();
        }
    }
}

void MainWindow::on_tb_refresh_tableW_patches_clicked()
{
    update_package_files();
}

void MainWindow::on_tb_refresh_tableW_aditional_files_clicked()
{
    update_package_files();
}


void MainWindow::on_tb_patch_down_clicked()
{
    QModelIndexList list = ui->tableW_patches->selectionModel()->selectedRows(1);
    if(list.count() != 1 )
        return;
    bool ok = false;
    int key = ui->tableW_patches->item(list.first().row(), 0)->data(Qt::DisplayRole).toInt(&ok);
    if( ! ok) key = 1;
    QString value = ui->tableW_patches->item(list.first().row(), 1)->data(Qt::DisplayRole).toString();
    QMultiMap<int, QString>::iterator it = patches.find(key, value);
    if(it != patches.end()){
        patches.remove(it.key(), it.value());
        key++;
        patches.insert(key, value);
    }
    update_package_files();
}

void MainWindow::on_tb_patch_up_clicked()
{
    QModelIndexList list = ui->tableW_patches->selectionModel()->selectedRows(1);
    if(list.count() != 1 )
        return;
    bool ok = false;
    int key = ui->tableW_patches->item(list.first().row(), 0)->data(Qt::DisplayRole).toInt(&ok);
    if( ! ok) key = 1;
    if(key <= 1)
        return;
    QString value = ui->tableW_patches->item(list.first().row(), 1)->data(Qt::DisplayRole).toString();
    QMultiMap<int, QString>::iterator it = patches.find(key, value);
    if(it != patches.end()){
        patches.remove(it.key(), it.value());
        key--;
        patches.insert(key, value);
    }
    update_package_files();
}

void MainWindow::clear_tableW_patches()
{
    int row_count = ui->tableW_patches->rowCount();
    for (int i = 0; i < row_count; ++i) {
        ui->tableW_patches->removeRow(0);
    }
}

void MainWindow::clear_tableW_aditional_files()
{
    int row_count = ui->tableW_aditional_files->rowCount();
    for (int i = 0; i < row_count; ++i) {
        ui->tableW_aditional_files->removeRow(0);
    }
}

void MainWindow::on_tb_edit_aditional_files_clicked()
{
    QModelIndexList list = ui->tableW_aditional_files->selectionModel()->selectedRows(0);
    if(list.count() != 1 )
        return;
    QString a_file = ui->tableW_aditional_files->item(list.first().row(), 0)->data(Qt::DisplayRole).toString();
    AditionalFileDialog afd(this, a_file, aditional_files.value(a_file));
    if( afd.exec() == QDialog::Accepted ){
        aditional_files[a_file] = afd.get_attr();
        update_package_files();
    }
}

void MainWindow::clear_tableW_files()
{
    int row_count = ui->tableW_files->rowCount();
    for (int i = 0; i < row_count; ++i) {
        ui->tableW_files->removeRow(0);
    }
}

void MainWindow::on_tb_add_label_clicked()
{
    QModelIndexList list = ui->treeV_files->selectionModel()->selectedRows(0);
    if(list.count() != 1)
        return;
    DirectoryModel * model = qobject_cast<DirectoryModel*>(ui->treeV_files->model());
    if(model){
        bool is_dir = model->isDir(list.first());
        QString path = model->filePath(list.first()).remove(package_install_dir.absolutePath());
        if(is_dir)
            path.append("/*");
        AddInstallFileLabelDialog aifld(this, path);
        if(aifld.exec() == QDialog::Accepted){
            bool permanent = aifld.get_permanent();
            QString file_type = aifld.get_file_type();
            append_file(path, file_type, permanent);
            QMap<QString, bool> attr;
            attr[file_type] = permanent;
            files[path] = attr;
        }
    }
}

void MainWindow::append_file(const QString &path, const QString &file_type, bool permanent)
{
    ui->tableW_files->insertRow(0);
    ui->tableW_files->setItem(0, 0, new QTableWidgetItem(path));
    ui->tableW_files->setItem(0, 1, new QTableWidgetItem(file_type));
    ui->tableW_files->setItem(0, 2, new QTableWidgetItem(permanent ? tr("True"):tr("False")));
    ui->tableW_files->sortItems(0);
}

void MainWindow::on_tb_delete_label_clicked()
{
    QModelIndexList list = ui->tableW_files->selectionModel()->selectedRows(0);
    if(list.count() == 1){
        QString path = ui->tableW_files->item(list.first().row(), 0)->data(Qt::DisplayRole).toString();
        files.remove(path);
        ui->tableW_files->removeRow(list.first().row());
        if( ! ui->tableW_files->model()->rowCount())
            append_file("/", "ALL", false);
    }
}

void MainWindow::on_tableW_files_itemSelectionChanged()
{
    QModelIndexList list = ui->tableW_files->selectionModel()->selectedRows(0);
    ui->tb_delete_label->setEnabled(list.count() == 1);
}

void MainWindow::on_tb_open_patches_dir_clicked()
{
    on_tb_open_aditional_files_dir_clicked();
}

void MainWindow::on_tb_open_aditional_files_dir_clicked()
{
    if( ! package_files_dir.isRoot() && package_files_dir.exists())
        QDesktopServices::openUrl(QUrl::fromLocalFile(package_files_dir.absolutePath()));
}

void MainWindow::on_tb_open_install_dir_clicked()
{
    if( ! package_install_dir.isRoot() && package_install_dir.exists())
        QDesktopServices::openUrl(QUrl::fromLocalFile(package_install_dir.absolutePath()));
}

void MainWindow::on_tb_open_package_dir_clicked()
{
    if( ! package_dir.isRoot() && package_dir.exists())
        QDesktopServices::openUrl(QUrl::fromLocalFile(package_dir.absolutePath()));
}

void MainWindow::on_tb_add_archive_clicked()
{
    static QString previous_directory;

    ArchiveSelectionDialog::ArchiveType type = ArchiveSelectionDialog::COMPRESSED;
    if(ui->combo_archive_type->currentIndex() == 0)
        type = ArchiveSelectionDialog::COMPRESSED;
    else if(ui->combo_archive_type->currentIndex() == 1)
        type = ArchiveSelectionDialog::URL;
    else
        Q_ASSERT(false);

    ArchiveSelectionDialog asd(this, type);
    asd.set_previous_directory(previous_directory);

    if(asd.exec() == QDialog::Accepted){
        previous_directory = asd.get_previous_directory();
        append_archive(asd.get_archive(), asd.get_sha1());
    }
}

void MainWindow::append_archive(const QString &archive, const QString &sha1)
{
    QVBoxLayout * w_layout = qobject_cast<QVBoxLayout *>(ui->w_archive_base->layout());
    if( ! w_layout){
        w_layout = new QVBoxLayout;
        w_layout->setMargin(1);
        w_layout->setSpacing(1);
        ui->w_archive_base->setLayout(w_layout);
    }
    ArchiveWidget * a_w = new ArchiveWidget(
                ui->w_archive_base,
                archive,
                sha1,
                PisiSource::get_archive_type_list(),
                PisiSource::get_archive_type(archive)
                );
    connect(a_w, SIGNAL(delete_me(ArchiveWidget*)), this, SLOT(delete_archive(ArchiveWidget*)));
    archive_widgets.append(a_w);
    w_layout->addWidget(a_w);
}

void MainWindow::delete_archive(ArchiveWidget * a_w)
{
    disconnect(a_w, SIGNAL(delete_me(ArchiveWidget*)), this, SLOT(delete_archive(ArchiveWidget*)));
    archive_widgets.removeAt(archive_widgets.indexOf(a_w));
    delete a_w;
}

void MainWindow::clear_archive_widgets()
{
    foreach (ArchiveWidget * a_w, archive_widgets) {
        delete_archive(a_w);
    }
}

void MainWindow::on_tb_add_translation_clicked()
{
    QString language = ui->combo_translation->currentText();
    foreach (TranslationWidget * t_w, translation_widgets) {
        if(t_w->get_language() == language)
            return;
    }
    QVBoxLayout * w_layout = qobject_cast<QVBoxLayout *>(ui->w_translation_base->layout());
    if( ! w_layout){
        w_layout = new QVBoxLayout;
        w_layout->setMargin(2);
        w_layout->setSpacing(2);
        ui->w_translation_base->setLayout(w_layout);
    }
    TranslationWidget * t_w = new TranslationWidget(
                ui->w_translation_base, language);
    connect(t_w, SIGNAL(delete_me(TranslationWidget*)), this, SLOT(delete_translation(TranslationWidget*)));
    translation_widgets.append(t_w);
    w_layout->addWidget(t_w);

}

void MainWindow::delete_translation(TranslationWidget * t_w)
{
    disconnect(t_w, SIGNAL(delete_me(TranslationWidget*)), this, SLOT(delete_translation(TranslationWidget*)));
    translation_widgets.removeAt(translation_widgets.indexOf(t_w));
    delete t_w;
}

void MainWindow::clear_translation_widgets()
{
    foreach (TranslationWidget * t_w, translation_widgets) {
        delete_translation(t_w);
    }
}

void MainWindow::on_tb_import_package_clicked()
{

    QString pspec_file = package_dir.absoluteFilePath("pspec.xml");

    if(QFile::exists(pspec_file)){
        QFile file(pspec_file);
        if( ! file.open(QFile::ReadOnly)){
            QMessageBox::critical(this, tr("Error"), tr("Can not open file for reading !"));
            return;
        }

        clear_archive_widgets();
        clear_translation_widgets();
        clear_tableW_files();

        QString errorMsg;
        int errorLine, errorColumn;
        if( ! dom_pspec.setContent(&file, &errorMsg, &errorLine, &errorColumn)){
            file.close();
            dom_pspec.clear();
            QMessageBox::critical(this, tr("Parse Error"),
                                  tr("XML Parse Error : \n%1\nLine:%2, Column:%3")
                                    .arg(errorMsg).arg(errorLine).arg(errorColumn)
                                  );
            return;
        }
        file.close();
        pisi.clear();
        ui->lw_packages->clear();
        ui->lw_packages->addItem("Package name");

        try{
            pisi.load_from_dom(dom_pspec);
        }
        catch (QString e){
            QMessageBox::critical(this, tr("Error"), tr("An error occured while parsing xml file : %1").arg(e));
            return;
        }
        catch(...){
            QMessageBox::critical(this, tr("Error"), tr("Unknownt exception !"));
            return;
        }

        try{
            pisi_to_gui();
        }
        catch (QString e){
            QMessageBox::critical(this, tr("Error"), tr("An error occured while filling fields: %1").arg(e));
            return;
        }
        catch (...){
            QMessageBox::critical(this, tr("Error"), tr("Unknownt exception !"));
            return;
        }

        QString actions_py = package_dir.absoluteFilePath("actions.py");
        if(QFile::exists(actions_py)){
            int current_index = ui->combo_actions_template->currentIndex();
            actions_templates[COMBO_ACTIONS_IMPORT_INDEX] = get_file_contents(actions_py);
            ui->combo_actions_template->setCurrentIndex(COMBO_ACTIONS_IMPORT_INDEX);
            if(current_index == COMBO_ACTIONS_IMPORT_INDEX)
                on_combo_actions_template_currentIndexChanged(COMBO_ACTIONS_IMPORT_INDEX);
        }

        // sipa getti kod :)
        QString translations_xml = package_dir.absoluteFilePath("translations.xml");
        QFile translations_file(translations_xml);
        if(translations_file.exists()){
            if(translations_file.open(QIODevice::ReadOnly)){
                QDomDocument dom;
                QString errorMsg;
                int errorLine, errorColumn;
                if(dom.setContent(&translations_file, &errorMsg, &errorLine, &errorColumn)){
                    QDomElement root = dom.firstChildElement("PISI");
                    if( ! root.isNull()){
                        QDomElement elm = root.firstChildElement("Source");
                        if( ! elm.isNull()){
                            elm = elm.firstChildElement("Name");
                            if( ! elm.isNull() && elm.text() == package_name){
                                QDomElement elm_sum = elm.nextSiblingElement("Summary");
                                while( ! elm_sum.isNull()){
                                    QString lang = elm_sum.attribute("xml:lang");
                                    QString language = QLocale::languageToString(
                                                QLocale(lang).language());
                                    translations[elm.text()][language] = elm_sum.text();
                                    bool added_before = false;
                                    foreach(TranslationWidget*t_w, translation_widgets){
                                        if(t_w->get_language() == language){
                                            t_w->set_summary(elm_sum.text());
                                            added_before = true;
                                        }
                                    }
                                    if( ! added_before){
                                        if( ! lang.isEmpty() && language_hash.contains(language)){
                                            ui->combo_translation->setCurrentIndex(
                                                        ui->combo_translation->findText(language));
                                            on_tb_add_translation_clicked();
                                            foreach(TranslationWidget*t_w, translation_widgets){
                                                if(t_w->get_language() == language){
                                                    t_w->set_summary(elm_sum.text());
                                                }
                                            }
                                        }
                                    }
                                    elm_sum = elm_sum.nextSiblingElement("Summary");
                                }
                                QDomElement elm_desc = elm.nextSiblingElement("Description");
                                while( ! elm_desc.isNull()){
                                    QString lang = elm_desc.attribute("xml:lang");
                                    QString language = QLocale::languageToString(
                                                QLocale(lang).language());
                                    bool added_before = false;
                                    foreach(TranslationWidget*t_w, translation_widgets){
                                        if(t_w->get_language() == language){
                                            t_w->set_description(elm_desc.text());
                                            added_before = true;
                                        }
                                    }
                                    if( ! added_before){
                                        if( ! lang.isEmpty() && language_hash.contains(language)){
                                            ui->combo_translation->setCurrentIndex(
                                                        ui->combo_translation->findText(language));
                                            on_tb_add_translation_clicked();
                                            foreach(TranslationWidget*t_w, translation_widgets){
                                                if(t_w->get_language() == language){
                                                    t_w->set_description(elm_sum.text());
                                                }
                                            }
                                        }
                                    }
                                    elm_desc = elm_desc.nextSiblingElement("Description");
                                }
                            }
                            else {
                                // no name or not equal
                                QMessageBox::critical(this,
                                                      tr("Translation File Error"),
                                                      tr("No Name tag in Source tag in PISI tag in translation.xml "
                                                         "or name is not same with package name !"));
                            }
                        }
                        else{
                            // no source
                            QMessageBox::critical(this,
                                                  tr("Translation File Error"),
                                                  tr("No Source tag in PISI tag in translation.xml !"));
                        }

                        // translation.xml package information
                        QDomElement elm_package = root.firstChildElement("Package");
                        if(!elm_package.isNull()){
                            bool ispackage=true;
                            do{
                                if( ! elm.isNull()){
                                    elm = elm_package.firstChildElement("Name");
                                    if( ! elm.isNull() /*&& elm.text() == package_name*/){
                                        QDomElement elm_sum = elm.nextSiblingElement("Summary");
                                        QString lang ;
                                        while( ! elm_sum.isNull()){
                                            lang = elm_sum.attribute("xml:lang");
                                            QString language = QLocale::languageToString(QLocale(lang).language());
                                            translations[elm.text()][language] = elm_sum.text();
    //                                        qDebug() << elm.text() << " -> "
    //                                                 << lang << " -> "
    //                                                 << translations[elm.text()][language];
    /*                                        bool added_before = false;
    //                                        foreach(TranslationWidget*t_w, translation_widgets){
    //                                            if(t_w->get_language() == language){
    //                                                t_w->set_summary(elm_sum.text());
    //                                                added_before = true;
    //                                            }
    //                                        }
    //                                        if( ! added_before){
    //                                            if( ! lang.isEmpty() && language_hash.contains(language)){
    //                                                ui->combo_translation->setCurrentIndex(
    //                                                            ui->combo_translation->findText(language));
    //                                                on_tb_add_translation_clicked();
    //                                                foreach(TranslationWidget*t_w, translation_widgets){
    //                                                    if(t_w->get_language() == language){
    //                                                        t_w->set_summary(elm_sum.text());
    //                                                    }
    //                                                }
    //                                            }
    //                                        }*/
                                            elm_sum = elm_sum.nextSiblingElement("Summary");
                                        }
     /*                                   QDomElement elm_desc = elm.nextSiblingElement("Description");
                                        while( ! elm_desc.isNull()){
                                            lang = elm_desc.attribute("xml:lang");
                                            QString language = QLocale::languageToString(
                                                        QLocale(lang).language());
                                            translations[elm.text()][lang] << elm_desc.text();
                                            bool added_before = false;
                                            foreach(TranslationWidget*t_w, translation_widgets){
                                                if(t_w->get_language() == language){
                                                    t_w->set_description(elm_desc.text());
                                                    added_before = true;
                                                }
                                            }
                                            if( ! added_before){
                                                if( ! lang.isEmpty() && language_hash.contains(language)){
                                                    ui->combo_translation->setCurrentIndex(
                                                                ui->combo_translation->findText(language));
                                                    on_tb_add_translation_clicked();
                                                    foreach(TranslationWidget*t_w, translation_widgets){
                                                        if(t_w->get_language() == language){
                                                            t_w->set_description(elm_sum.text());
                                                        }
                                                    }
                                                }
                                            }

                                            elm_desc = elm_desc.nextSiblingElement("Description");
                                        }*/

                                    }
                                    else {
                                        // no name or not equal
                                        QMessageBox::critical(this,
                                                              tr("Translation File Error"),
                                                              tr("No Name tag in Package tag in PISI tag in translation.xml "
                                                                 "or name is not same with package name !"));
                                    }
                                }
                                elm_package=elm_package.nextSiblingElement("Package");
                                if (elm_package.isNull())
                                    ispackage=false;
                            }while(ispackage);
                        }
                    }
                    else{
                        // no PISI
                        QMessageBox::critical(this,
                                              tr("Translation File Error"),
                                              tr("No PISI tag in translation.xml !"));
                    }
                    // translation.xml package information
                }
                else {
                    QMessageBox::critical(this, tr("Translation File Error"),
                                          tr("XML Parse Error : \n%1\nLine:%2, Column:%3")
                                            .arg(errorMsg).arg(errorLine).arg(errorColumn)
                                          );
                }
                translations_file.close();
            }
        }


        on_le_package_name_textChanged(ui->le_package_name->text());  // to fill package install directory tree
        on_tb_refresh_treeV_files_clicked();

        statusBar()->showMessage(tr("Package build information successfully imported."));
    }

    if(first_run){
        first_run=false;
        on_tb_import_package_clicked();
        }
}

//@qtslot(int)
void MainWindow::pisi_to_gui() throw (QString)
{
    if(pisi.is_empty())
        throw tr("Empty pisi file, import pspec.xml before use !");


    // history section
    clear_tableW_history();
    QMap<int, PisiUpdate> updates = pisi.get_updates();
    QList<int> releases = updates.keys();   // keys are asc ordered
    foreach (int r, releases){
        set_history_update(updates[r]);
    }


    ui->lw_packages->clear();

    // source section
    PisiSource source = pisi.get_source();
    package_name = source.get_name();
    homepage = source.get_home_page();
    QString licenses = source.get_licenses().join(", ");
    QString is_a_s = source.get_is_a_s().join(", ");
    part_of = source.get_part_of();
    summary = source.get_summary();
    if(ui->lw_packages->currentRow()==0){
        ui->le_summary->setText(summary);
    }
    description = source.get_description();
    build_dependency = source.get_build_dependencies_as_stringlist().join(", ");
    QMap<QString, QMap<PisiSource::ArchiveAttr,QString> > archives = source.get_archives();
    QMap<QString, QMap<PisiSource::PatchAttr,QString> > patches = source.get_patches();
    // assign to gui
    ui->le_homepage->setText(homepage);
    ui->le_license->setText(licenses);
    ui->le_is_a->setText(is_a_s);
    ui->combo_part_of->setCurrentIndex(ui->combo_part_of->findText(part_of));
    ui->le_summary->setText(summary);
    ui->pte_description->setPlainText(description);
    ui->le_build_dependency->setText(build_dependency);
    QList<QString> archive_list = archives.keys();
    foreach (QString a, archive_list) {
        append_archive(a, archives[a][PisiSource::SHA1SUM]);
    }
    this->patches.clear();
    QList<QString> patch_list = patches.keys();
    foreach (QString patch, patch_list) {
        QMap<PisiSource::PatchAttr,QString> attr = patches.value(patch);
        this->patches.insert(attr.value(PisiSource::LEVEL).toInt(), patch);
    }
    fill_tableW_patches();

    // package section
    PisiPackage package;

    int count=pisi.packages_count();
    int i=1;
    do{
//        if (pisi.get_package(i).get_name()=="")
//            break;
        package = pisi.get_package(i);

        btn_add_package_clicked(package.get_name());
        ++i;
    }while(i<=count);

    ui->lw_packages->setCurrentRow(0);
    connect(ui->lw_packages,SIGNAL(currentRowChanged(int)),this,SLOT(package_to_gui(int)));

    package_to_gui(0);
}

void MainWindow::package_to_gui(int key) throw (QString)
{
    clear_tableW_files();
    files.clear();
    PisiPackage package;

    package = pisi.get_package(key+1);

    if(key!=0 || package.get_psummary()!=NULL){
        ui->le_summary->setText(package.get_psummary());}
    else{
        ui->le_summary->setText(this->summary);}


    runtime_dependency = package.get_runtime_dependencies_as_stringlist().join(", ");
    ui->le_runtime_dependency->setText(runtime_dependency);
    files = package.get_files_as_string_type();
    aditional_files.clear();
    QList<QString> files_keys = files.keys();
    foreach (QString path, files_keys) {
        QMap<QString, bool> attr = files.value(path);
        append_file(path, attr.keys().first(), attr.value(attr.keys().first()));
    }

    aditional_files = package.get_aditional_files();
    update_package_files();

    foreach(TranslationWidget*t_w, translation_widgets){
        t_w->set_summary(translations[package.get_name()][t_w->get_language()]);
    }

}

void MainWindow::pisi_from_gui() throw (QString)
{

    // history section
    QMap<int, PisiUpdate> updates;
    int history_row_count = ui->tableW_history->rowCount();
    if(history_row_count == 0)
        throw tr("Please define an update in history !");
    for(int i=0; i<history_row_count; ++i)
    {
        bool ok = false;
        int release = ui->tableW_history->item(i,0)->text().toInt(&ok);
        if(ok)
            updates[release] = get_history_update(i);
        else
            throw tr("Error at conversion release string to integer !");
    }
    pisi.set_updates(updates);


    // source section
    PisiSource source;
    source.set_name(package_name);
    source.set_home_page(homepage);
    source.set_packager(pisi.get_last_update().get_packager_name(), pisi.get_last_update().get_packager_email());
    QStringList licenses;
    foreach (QString license, this->licenses.split(",", QString::SkipEmptyParts)) {
        licenses << license.trimmed();
    }
    source.set_licenses(licenses);
    QStringList is_a_s;
    foreach (QString is_a, this->is_a_s.split(",", QString::SkipEmptyParts)) {
        is_a_s << is_a.trimmed();
    }
    source.set_is_a_s(is_a_s);
    source.set_part_of(part_of);
    if(summary==NULL)
        summary=ui->le_summary->text();
    source.set_summary(summary);
    source.set_description(description);
    source.set_build_dependencies(build_dependency);
    QMap<QString, QMap<PisiSource::ArchiveAttr,QString> > archives;
    foreach (ArchiveWidget * a_w, archive_widgets) {
        QMap<PisiSource::ArchiveAttr,QString> attr;
        attr[PisiSource::SHA1SUM] = a_w->get_sha1();
        attr[PisiSource::TYPE] = a_w->get_type();
        archives[a_w->get_archive()] = attr;
    }
    source.set_archives(archives);
    QMap<QString, QMap<PisiSource::PatchAttr, QString> > s_p;
    QMultiMap<int, QString>::const_iterator s_p_it = patches.constBegin();
    while (s_p_it != patches.constEnd()) {
        QMap<PisiSource::PatchAttr, QString> s_p_attr;
        s_p_attr[PisiSource::LEVEL] = QString::number(s_p_it.key());
        s_p[s_p_it.value()] = s_p_attr;
        ++s_p_it;
    }
    source.set_patches(s_p);
    pisi.set_source(source);


    // package section
    save_package(ui->lw_packages->currentRow()+1);
    // package section

    //translation
    QString name=ui->lw_packages->currentItem()->text();
    foreach(TranslationWidget*t_w, translation_widgets){
        translations[name][t_w->get_language()]=t_w->get_summary();
    }
    //translation
}

void MainWindow::save_package(int key){
    PisiPackage package;
    package.set_name(ui->lw_packages->item(key-1)->text());
    package.set_runtime_dependencies(runtime_dependency);
    //if(package.get_files_as_string_type().isEmpty())
        package.set_files(files);
    package.set_psummary(ui->le_summary->text());
    temp_aditional_files.clear();
    QList<QString> a_f_list = aditional_files.keys();
    foreach (QString a_f, a_f_list) {
        QMap<PisiSPBase::AFileAttr,QString> a_f_attr = aditional_files.value(a_f);
        QList<PisiSPBase::AFileAttr> a_f_attr_keys = a_f_attr.keys();
        foreach (PisiSPBase::AFileAttr a, a_f_attr_keys) {
            if(a == PisiSPBase::TARGET){
                a_f_attr[a] = a_f_attr[a].replace("__package_name__", package_name);
                a_f_attr[a] = a_f_attr[a].replace("__version__", pisi.get_last_update().get_version());
                a_f_attr[a] = a_f_attr[a].replace("__summary__", summary);
            }
        }
        temp_aditional_files[a_f] = a_f_attr;
    }
    package.set_aditional_files(temp_aditional_files);
    temp_aditional_files.clear();
    pisi.set_package(key,package);
}

bool MainWindow::create_build_files()
{
    // fill pisi
    dom_pspec.clear();
    try{
        pisi_from_gui();
        pisi.save_to_dom(dom_pspec);
    }
    catch (QString e){
        QMessageBox::critical(this, tr("Error"), tr("An error occured while filling pisi from fields :\n %1").arg(e));
        return false;
    }
    catch (...){
        QMessageBox::critical(this, tr("Error"), tr("Unknownt exception while filling pisi from fields !"));
        return false;
    }

    if( ! package_dir.exists()){
        QString package_dir_name = package_dir.dirName();
        if(package_dir_name.isEmpty()){
            QMessageBox::critical(this, tr("Error"), tr("Empty package name !"));
            return false;
        }
        if( ! workspace_dir.mkdir(package_dir_name)){
            QMessageBox::critical(this, tr("Error"), tr("Can not create package directory in workspace !"));
            return false;
        }
        if( ! package_dir.mkdir("files")){
            QMessageBox::critical(this, tr("Error"), tr("Can not create files directory in package directory !"));
            return false;
        }
    }

    /* bunlar silinecek */
    //QString brand_string = QString("By %1 %2").arg(qApp->applicationName()).arg(qApp->applicationVersion());
    //QString python_branding = QString("\n# %1\n").arg(brand_string);
    //QString xml_branding = QString("\n<!-- %1 -->\n").arg(brand_string);

    QString pspec_file_name = package_dir.absoluteFilePath("pspec.xml");
    QString pspec_content = dom_pspec.toString(4) /*+ xml_branding*/;
    save_text_file( pspec_file_name, pspec_content );

    // create translation
    if(translation_widgets.count()){
        QDomDocument dom;
        QDomElement pisi = dom.createElement("PISI");
        dom.appendChild(pisi);
        QDomElement source = dom.createElement("Source");
        pisi.appendChild(source);

        QDomElement source_name = dom.createElement("Name");
        source_name.appendChild(dom.createTextNode(package_name));
        source.appendChild(source_name);

        foreach (TranslationWidget * t_w, translation_widgets) {
            int l = language_hash.value(t_w->get_language());
            QLocale loc((QLocale::Language) l);
            if(translations[package_name][t_w->get_language()].isEmpty())
                continue;
            QDomElement source_summary = dom.createElement("Summary");
            source_summary.appendChild(dom.createTextNode(translations[package_name][t_w->get_language()]));
            source_summary.setAttribute("xml:lang", loc.name().split('_').first());
            source.appendChild(source_summary);
        }
        foreach (TranslationWidget * t_w, translation_widgets) {
            int l = language_hash.value(t_w->get_language());
            QLocale loc((QLocale::Language) l);
            if(t_w->get_description().isEmpty())
                continue;
            QDomElement source_description = dom.createElement("Description");
            source_description.appendChild(dom.createTextNode(t_w->get_description()));
            source_description.setAttribute("xml:lang", loc.name().split('_').first());
            source.appendChild(source_description);
        }

        for(int i=1; i<ui->lw_packages->count(); i++){
            QString name = ui->lw_packages->item(i)->text();
            QDomElement package = dom.createElement("Package");
            pisi.appendChild(package);

            QDomElement package_name = dom.createElement("Name");
            package_name.appendChild(dom.createTextNode(name));
            package.appendChild(package_name);

            foreach (TranslationWidget * t_w, translation_widgets) {
                int l = language_hash.value(t_w->get_language());
                QLocale loc((QLocale::Language) l);
                if(translations[name][t_w->get_language()] != ""){
                    QDomElement package_summary = dom.createElement("Summary");
                    package_summary.appendChild(dom.createTextNode(translations[name][t_w->get_language()]));
                    package_summary.setAttribute("xml:lang", loc.name().split('_').first());
                    package.appendChild(package_summary);
                }
            }
        }

        QString content = dom.toString(4) /*+ xml_branding*/;
        QString file_name = package_dir.absoluteFilePath("translations.xml");
        save_text_file(file_name, content);
    }


    // create actions
    QString actions_file_name = package_dir.absoluteFilePath("actions.py");
    QString action_py_contents = actions_editor->text();
//    if( ! action_py_contents.contains(python_branding)){
//        action_py_contents += python_branding;
//    }
    if(action_py_contents.isEmpty())
        QMessageBox::information(this, tr("Actions API File"), tr("Actions.py is empty !"));
    action_py_contents.replace(QString("__package_name__"), package_name);
    action_py_contents.replace(QString("__version__"), pisi.get_last_update().get_version());
    action_py_contents.replace(QString("__summary__"), summary);
    save_text_file( actions_file_name, action_py_contents );

    on_le_package_name_textChanged(ui->le_package_name->text());

    return true;
}

void MainWindow::on_tb_build_up_to_clicked()
{
    if( ! create_build_files())
        return;

    QStringList build_steps;
    build_steps.append("--fetch");
    build_steps.append("--unpack");
    build_steps.append("--setup");
    build_steps.append("--build");
    build_steps.append("--install");
    build_steps.append("--check");
    call_pisi_build_command(build_steps.at(ui->combo_build_up_to->currentIndex()));
}

void MainWindow::on_tb_build_only_clicked()
{
    if(ui->combo_build_only->currentIndex() == 0){
        if(create_build_files()){
            QMessageBox::information(this,
                                     tr("Build Successful"),
                                     tr("Build files created successfully."));
        }
    }
    else if(ui->combo_build_only->currentIndex() == 1){
        if(create_build_files()){
            call_pisi_build_command("--package");
        }
    }
}

void MainWindow::on_tb_build_all_clicked()
{
    if(create_build_files())
        call_pisi_build_command();
}

void MainWindow::call_pisi_build_command(const QString &build_step)
{
    QString pspec_file = package_dir.absoluteFilePath("pspec.xml");
    if( ! QFile::exists(pspec_file)){
        QMessageBox::critical(this, tr("Error"), tr("There is no PSPEC file : %1 ").arg(pspec_file));
        return;
    }
    DirectoryModel * model = qobject_cast<DirectoryModel*>(ui->treeV_files->model());
    if(model){
        model->clear();
    }

    QString command;
    if(docker_is_running){
        command = QString("pisi build '%1' %2 -d --ignore-safety\n") //--output-dir %3
                .arg("/git/"+pspec_file.split(workspace_dir.absolutePath()+"/").last())
                .arg(build_step)
                //.arg(workspace_dir.absolutePath())
                ;
    }else{
        command = QString("pisi build '%1' %2 --output-dir %3 \n")
            .arg(pspec_file)
            .arg(build_step)
            .arg(output_dir.absolutePath())
            ;
        if (!is_root)
            command="pkexec -u root "+command;
    }
    w_terminal->sendText(command);

    /*derleme sayacı*/
    timer=new QTimer(0);
    timer->start(1000);
    time.start();
    start_process=false;
    connect(timer,SIGNAL(timeout()),this,SLOT(time_elapsed()));
}


void MainWindow::btn_add_package_clicked(QString name)
{
    lw_item=new QListWidgetItem(name);
    lw_item->setFlags(lw_item->flags() | Qt :: ItemIsEditable);

    ui->lw_packages->addItem(lw_item);


    ui->le_summary->setText("");
    ui->le_runtime_dependency->setText("");

    ui->lw_packages->setCurrentRow(ui->lw_packages->count()-1);
    save_package(ui->lw_packages->count());
}


void MainWindow::on_btn_remove_package_clicked(){
    int i=ui->lw_packages->currentRow();
    pisi.remove_package(i+1);
    QString name=ui->lw_packages->currentItem()->text();
    translations.remove(name);
    ui->lw_packages->takeItem(i);
    ui->lw_packages->setCurrentRow(0);
}

void MainWindow::time_elapsed(){
    int elapsed_time=time.elapsed()/1000;

    QProcess *p=new QProcess(0);
    QStringList argmnt;
    argmnt << "-u"
           << "root"
           << "pisi";
    //qDebug()<<argmnt.last();
    p->start("pgrep",argmnt);
    if(!p->isOpen())
        qDebug() << "process is not open";
    p->waitForBytesWritten();
    p->waitForFinished();
    QString out=QString(p->readAll());
    //qDebug() << QString("Out : %1").arg(out);

    setWindowTitle("PisiDo - "+QString("%1:%2:%3")
                   .arg(elapsed_time/3600, 2, 10, QChar('0'))
                   .arg((elapsed_time/60)%60, 2, 10, QChar('0'))
                   .arg(elapsed_time%60, 2, 10, QChar('0')));

    if (start_process==false) {
        QProcess *pkexec=new QProcess(0);
        pkexec->start("pgrep",QStringList("pkexec"));
        pkexec->waitForBytesWritten();
        pkexec->waitForFinished();

        QString pk_out=pkexec->readAll();
        //qDebug()<< QString("pkexec output : %1").arg(pk_out);

        delete pkexec;

        if(out !=""){
            start_process=true;
        }
        else if (out=="" && pk_out=="") {
            timer->stop();
            start_process=false;
            disconnect(timer,SIGNAL(timeout()),this,SLOT(time_elapsed()));
            timer->disconnect(timer,SIGNAL(timeout()),this,SLOT(time_elapsed()));
            setWindowTitle("PisiDo");
        }
    }
    else {
        if (out=="") {
            timer->stop();
            start_process=false;
            QMessageBox::information(0,tr("Elapsed Time"),tr("   %1:%2:%3   ")
                                     .arg(elapsed_time/3600, 2, 10, QChar('0'))
                                     .arg((elapsed_time/60)%60, 2, 10, QChar('0'))
                                     .arg(elapsed_time%60, 2, 10, QChar('0')));
            disconnect(timer,SIGNAL(timeout()),this,SLOT(time_elapsed()));
            timer->disconnect(timer,SIGNAL(timeout()),this,SLOT(time_elapsed()));
        }
    }

    p->terminate();
    delete p;
}

void MainWindow::save_act_editor_conf(){
    int range;

    if(Zoom==true)
        range=1;
    else
        range=-1;

    settings.beginGroup("actions_editor");
    int zoom=settings.value("Zoom", QString()).toInt();
    settings.setValue("Zoom",zoom+range);
    settings.endGroup();
}

void MainWindow::get_act_editor_conf(){
    settings.beginGroup("actions_editor");
    int zoom=settings.value("Zoom", QString()).toInt();
    settings.endGroup();

    actions_editor->zoomTo(zoom);
}

void MainWindow::on_actionOpen_Log_triggered(){
    settings.beginGroup("configuration");
    if (ui->le_package_name->text().isEmpty() || pisi.is_empty()) {
        QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Please enter package data."));
        return;
    }

    QString log_file_dir = settings.value("pisi_packaging_dir").toString()
            +ui->le_package_name->text()+"-"
            +pisi.get_updates().values().last().get_version()+"-"
            +QString::number(pisi.get_updates().values().last().get_release())
            +"/work/";

    //qDebug()<<log_file_dir;

    if( ! log_file_dir.isEmpty()){
        QDir dir(log_file_dir);
        dir.setCurrent(log_file_dir);
        if(dir.exists()){
            QDirIterator it(log_file_dir, QStringList() << "*.log", QDir::Files, QDirIterator::Subdirectories);
            QStringList files;

            QPoint globalPos = cursor().pos();
            QMenu menu;

            while (it.hasNext()) {
                files << it.next();
                menu.addAction(files.last());
                //qDebug() << "log file : " << files.last();
            }
            //qDebug()<<menu.actions().count();

            if(menu.actions().count()>0){
                if (menu.actions().count()==1) {
                    if(!QDesktopServices::openUrl(QUrl::fromLocalFile(files.last())))
                        QMessageBox::warning(0,trUtf8("Warning"), trUtf8("File is not opened!"));
                }
                else {
                    QAction* selectedItem = (menu.exec(globalPos));

                    if(!QDesktopServices::openUrl(QUrl::fromLocalFile(selectedItem->text())))
                        QMessageBox::warning(0,trUtf8("Warning"), trUtf8("File is not opened!"));
                    }
            }
            else{
                QMessageBox::warning(0,trUtf8("Warning"), trUtf8("Any log file is not found!"));
            }
        }
        else {
            QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Please try again after build process."));
        }
    }
    settings.endGroup();
}

void MainWindow::on_action_Run_Docker_triggered(){
    docker_is_running=true;

    workspace_dir_timer->blockSignals(true);
    ui->action_Checkelf->setEnabled(true);
    ui->action_Update_Docker->setEnabled(true);
    ui->action_Clear_Docker->setEnabled(true);
    ui->tb_docker_container->setEnabled(true);

    QString command = QString(/*"docker pull ertugerata/pisi-chroot-farm && " +*/
                              QString("docker run -v %1:/git ").arg(workspace_dir.absolutePath()) +
                              QString("-v %1:/root ").arg(output_dir.absolutePath()) +
                              QString("-v %1:/var/cache/pisi/archives ").arg(archives_dir.absolutePath()) +
                              QString("-v %1:/var/cache/pisi/packages ").arg(packages_dir.absolutePath()) +
                              "-itd ertugerata/pisi-chroot-farm bash \n");

    w_terminal->sendText(command);


    foreach (QAction *a, ui->tb_docker_container->actions()) {
        if(a->text()=="Find Containers")
            return;
    }
    action_Find_docker_containers=new QAction("Find Containers",0);
    ui->tb_docker_container->addAction(action_Find_docker_containers);
    ui->tb_docker_container->setDefaultAction(action_Find_docker_containers);
    connect(action_Find_docker_containers,SIGNAL(triggered()),this,SLOT(action_Find_docker_containers_triggered()));
}

void MainWindow::on_action_Update_Docker_triggered(){
    ui->tb_docker_container->setEnabled(true);
    foreach (QAction *a, ui->tb_docker_container->actions()) {
        if(a->text()=="Find Containers")
            continue;
        ui->tb_docker_container->removeAction(a);
    }
    ui->tb_docker_container->setText(trUtf8("Containers"));

    QString command = QString("docker pull ertugerata/pisi-chroot-farm \n");
    w_terminal->sendText(command);

    //find_docker_containers();
}

void MainWindow::on_action_Clear_Docker_triggered(){
    QString command = QString("rm -r /var/pisi \n");
    w_terminal->sendText(command);
}

void MainWindow::on_tb_import_from_folder_clicked(){
    QString import_folder=QFileDialog::getExistingDirectory(0,trUtf8("Import From Folder"), workspace_dir.absolutePath());
    import_folder=import_folder.split(workspace_dir.absolutePath()+"/").last();
    ui->le_package_name->setText(import_folder);

    on_tb_import_package_clicked();
}

void MainWindow::action_Find_docker_containers_triggered(){
    foreach (QAction *a, ui->tb_docker_container->actions()) {
        if(a->text()=="Find Containers")
            continue;
        ui->tb_docker_container->removeAction(a);
    }

    Containers_name.clear();
    Containers.clear();

    QProcess *p=new QProcess(0);

    QStringList argmnt;
    argmnt << "--user"
           << "root"
           << "docker"
           << "ps"
           << "-a";
    p->start("pkexec",argmnt);
    if(!p->isOpen())
        qDebug() << "process is not open";

    p->waitForBytesWritten();
    p->waitForFinished();
    QString out=QString(p->readAll());
    QStringList lines=out.split("\n");
    qDebug()<< lines;
    p->close();

    ui->actionRemove_Containers->setEnabled(true);

    delete p;

    for (int i=1;i<lines.length()-1;i++){
        bool iscontain=false;
        qDebug() << lines[i].contains("Exited");//trimmed().split(" ");//.last();
        foreach (QAction *a, ui->tb_docker_container->actions()) {
            if(a->text()==lines[i].trimmed().split(" ").last())
                iscontain=true;
        }
        if(iscontain)
            continue;

        Containers_name << lines[i].trimmed().split(" ").last();
        if(lines[i].contains("Exited"))
            Containers[Containers_name.last()]=true;
        else
            Containers[Containers_name.last()]=false;
        QAction *container=new QAction(lines[i].trimmed().split(" ").last(),0);
        ui->tb_docker_container->addAction(container);
    }
}


void MainWindow::container_triggered(QAction *action){
    if(action->text()=="Find Containers"){
//        action_Find_docker_containers_triggered();
        return;
    }

    ui->action_Update_Docker->setEnabled(false);
    ui->tb_docker_container->setEnabled(false);
    ui->action_Exit_Container->setEnabled(true);
    ui->action_Refresh_Container->setEnabled(true);

    if(Containers[ui->tb_docker_container->text()]){
        QString command = QString("docker start %1 \n").arg(action->text());
        w_terminal->sendText(command);
    }

    QString command = QString("docker attach %1 \n").arg(action->text());
    w_terminal->sendText(command);
    command=QString("service dbus start && pisi ur \n");
    w_terminal->sendText(command);
}

void MainWindow::on_action_Checkelf_triggered(){
    QString command = QString("checkelf -s -x *.pisi\n");
    w_terminal->sendText(command);
}

void MainWindow::on_action_Exit_Container_triggered(){
    ui->action_Checkelf->setEnabled(false);
    ui->action_Update_Docker->setEnabled(true);
    ui->action_Clear_Docker->setEnabled(false);
    ui->action_Exit_Container->setEnabled(false);
    ui->action_Refresh_Container->setEnabled(false);

    ui->tb_docker_container->setEnabled(true);
    foreach (QAction *a, ui->tb_docker_container->actions()) {
        ui->tb_docker_container->removeAction(a);
    }
    ui->tb_docker_container->setText(trUtf8("Containers"));

    QString command = QString("exit\n");
    w_terminal->sendText(command);
}

void MainWindow::on_action_Root_triggered(){
    ui->action_Root->setEnabled(false);
    ui->action_Run_Docker->setEnabled(true);
    is_root=true;

    QString command=QString("pkexec -u root bash \n");
    w_terminal->sendText(command);
}

void MainWindow::on_action_Start_Daemon_triggered(){
    pid_t pid = fork();

    if (pid == 0)
    {
        system("pkexec -u root docker -d -s overlay");
    }
    else if (pid > 0)
    {

    }
    else
    {
        // fork failed
        QMessageBox::warning(0,"Warning","Daemon is not start!\n");
        return;
    }
}

void MainWindow::on_action_Refresh_Container_triggered(){
    QString command=QString("pisi hs -t 80\n");
    w_terminal->sendText(command);
}

void MainWindow::on_actionRemove_Containers_triggered(){
    RemoveContainersDialog rmd(this);
    rmd.set_containers(Containers_name);
    if(rmd.exec() == QDialog::Accepted){
        Containers_name.clear();
        Containers.clear();
        QString command=QString("docker rm -f %1 \n").arg(rmd.get_selected_containers().join(" "));
        w_terminal->sendText(command);
    }
}
