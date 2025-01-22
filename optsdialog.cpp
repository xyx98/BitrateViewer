#include "optsdialog.h"
#include "ui_optsdialog.h"
#include "backend.h"

optsDialog::optsDialog(QWidget *parent,INI *ini)
    : QDialog(parent)
    , ui(new Ui::optsDialog)
{
    ui->setupUi(this);
    this->ini=ini;
    fs::path workpath=QApplication::applicationDirPath().toLocal8Bit().toStdString();
    std::vector<std::string> tmpls = Backend::findTemplate(workpath / "template");
    for(std::string tmpl:tmpls){
        QListWidgetItem *listItem = new QListWidgetItem(QString::fromStdString(tmpl), ui->listWidget);
        ui->listWidget->addItem(listItem);
        if (tmpl == ini->getTmpl()){
            ui->listWidget->setCurrentItem(listItem);
        }
    }
}

optsDialog::~optsDialog()
{
    delete ui;
}

void optsDialog::closeEvent(QCloseEvent *event)
{
    emit closed(false);
    QWidget::closeEvent(event);
}



void optsDialog::on_buttonBox_accepted()
{
    ini->setTmpl(ui->listWidget->currentItem()->text());
    emit closed(true);
}


void optsDialog::on_buttonBox_rejected()
{
    emit closed(false);
}

