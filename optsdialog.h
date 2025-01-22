#ifndef OPTSDIALOG_H
#define OPTSDIALOG_H

#include <QDialog>

#include "ini.h"

namespace Ui {
class optsDialog;
}

class optsDialog : public QDialog
{
    Q_OBJECT

public:
    std::vector<std::string> tmpls;
    INI *ini;
    bool tmplexist=false;

    explicit optsDialog(QWidget *parent = nullptr,INI *ini = nullptr);
    ~optsDialog();

signals:
    void closed(bool accepted);

private:
    Ui::optsDialog *ui;

protected:
    void closeEvent(QCloseEvent *event) override;
private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
};

#endif // OPTSDIALOG_H
