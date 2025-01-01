#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject>
#include <QFileDialog>
#include "plot.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QAction *actionopen;

    plot Plot;
    QString CurrentVideo;
    bool loaded;
    CalcResult CurrentResult;

    void setup_polt(std::string path,bool useCache,std::string filename);

private slots:

    void on_actionopen_triggered();

    void on_actionsave_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
