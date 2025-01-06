#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject>
#include <QFileDialog>
#include "plot.h"
#include <filesystem>
namespace fs = std::filesystem;


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
    fs::path workpath;

    void setup_polt(std::string path,bool useCache,std::string filename);
    void setup_polt(std::string path,std::string filename,std::string savecache);
    std::string checksum(QString path,int maxCalcSize);

private slots:

    void on_actionopen_triggered();

    void on_actionsave_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
