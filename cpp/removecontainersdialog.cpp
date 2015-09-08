#include "h/removecontainersdialog.h"
#include "ui_removecontainersdialog.h"

#ifdef USING_QT5
    #include <QtWidgets/QCheckBox>
#else
    #include <QCheckBox>
#endif

RemoveContainersDialog::RemoveContainersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemoveContainersDialog)
{
    ui->setupUi(this);
}

RemoveContainersDialog::~RemoveContainersDialog()
{
    delete ui;
}

void RemoveContainersDialog::set_containers(QStringList Containers){
    for(int i=0;i<Containers.length();i++){
        QListWidgetItem* item = new QListWidgetItem(Containers[i], ui->listWidget);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable); // set checkable flag
        item->setCheckState(Qt::Unchecked); // AND initialize check state
    }
}

void RemoveContainersDialog::on_pb_Select_All_clicked(){
    for(int i=0;i<ui->listWidget->count();i++){
        ui->listWidget->item(i)->setCheckState(Qt::Checked);
    }
}

QStringList RemoveContainersDialog::get_selected_containers(){
    QStringList selected_containers;

    for (int i=0;i<ui->listWidget->count();i++) {
        if(ui->listWidget->item(i)->checkState()==Qt::Checked)
            selected_containers << ui->listWidget->item(i)->text();
    }
    return selected_containers;
}
