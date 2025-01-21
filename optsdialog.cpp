#include "optsdialog.h"
#include "ui_optsdialog.h"

optsDialog::optsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::optsDialog)
{
    ui->setupUi(this);
}

optsDialog::~optsDialog()
{
    delete ui;
}

void optsDialog::closeEvent(QCloseEvent *event)
{
    emit closed();
    QWidget::closeEvent(event);
}



void optsDialog::on_buttonBox_accepted()
{
    emit closed();
}


void optsDialog::on_buttonBox_rejected()
{
    emit closed();
}

