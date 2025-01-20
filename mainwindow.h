#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject>
#include <QFileDialog>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>
#include <filesystem>
#include <QFile>
#include "plot.h"
#include "ini.h"

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
    INI *ini;
    QString CurrentVideo;
    int loadStatus;
    CalcResult CurrentResult;
    fs::path workpath;
    std::string CurrentHtml;

    void setup_polt(std::string path,bool useCache,std::string filename);
    void setup_polt(std::string path,std::string filename,std::string savecache);
    std::string checksum(QString path,int maxCalcSize);


signals:
    void loadhtml(QUrl url);

public slots:
    void LoadHTML(QUrl url);
private slots:

    void on_actionopen_triggered();

    void on_actionsave_triggered();

    void on_actionAbout_triggered();


    void on_actionoptions_triggered();

private:
    Ui::MainWindow *ui;
    QFutureSynchronizer<void> synchronizer;
};

#endif // MAINWINDOW_H
