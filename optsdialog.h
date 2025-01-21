#ifndef OPTSDIALOG_H
#define OPTSDIALOG_H

#include <QDialog>

namespace Ui {
class optsDialog;
}

class optsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit optsDialog(QWidget *parent = nullptr);
    ~optsDialog();

signals:
    void closed();

private:
    Ui::optsDialog *ui;

protected:
    void closeEvent(QCloseEvent *event) override;
private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
};

#endif // OPTSDIALOG_H
