#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "backend.h"
#include <QTimer>
#include <fstream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->webEngineView->setContextMenuPolicy(Qt::NoContextMenu);
    std::string html =plot::loadhtml(QApplication::applicationDirPath().toStdString()+"/resource/template.html");
    Plot.setBaseHtml(html);
    CurrentVideo="";
    loaded=false;
    //ui->webEngineView->setHtml(QString::fromStdString(html));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionopen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("open video file"),"",
        tr("video file(*.mkv,*.mp4,*.m2ts,*.ts,*.mov,*.m4v,*.avi,*.3GP,*.flv,*.ogv,*.vob)(*.mkv *.mp4 *.m2ts *.ts *.mov *.m4v *.avi *.3GP *.flv *.ogv *.vob);;all file(*)"));
    if (filename.isEmpty()) return;
    QFileInfo fileinfo = QFileInfo(filename);
    if(fileinfo.suffix().compare("csv",Qt::CaseInsensitive)==0)    setup_polt(filename.toStdString(),true,fileinfo.fileName().toStdString());
    else setup_polt(filename.toStdString(),false,fileinfo.fileName().toStdString());
    CurrentVideo=filename;
    loaded=true;
    return;
}


void MainWindow::setup_polt(std::string path,bool useCache,std::string filename){
    std::vector<FrameInfo> FrameInfoArray;
    //ui->webEngineView->setHtml("Loading...");
    if (useCache){
        FrameInfoArray = Backend::loadcsv(path);
    }
    else {
        FrameInfoArray = Backend::loadvideo(path);
    }
    CurrentResult = Backend::calc(FrameInfoArray);
    std::string html=Plot.applydata(CurrentResult,filename);
    //plot::savehtml("E:\\coding\\test\\test\\tmp.html",html);
    ui->webEngineView->setHtml(QString::fromStdString(html));
    return;
}


void MainWindow::on_actionsave_triggered()
{
    if (!loaded) return;
    QFileInfo fileinfo = QFileInfo(CurrentVideo);
    QString filename = QFileDialog::getSaveFileName(
        this,
        tr("save plot"),
        fileinfo.filePath()+".html",
        tr("html (*.html)"));
    if (filename.isEmpty()){
        //ui->webEngineView->setHtml("fail");
        return;
    }
    std::string html=Plot.applydata(CurrentResult,fileinfo.fileName().toStdString());
    plot::savehtml(filename.toStdString(),html);
    return;
}

