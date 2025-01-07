#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "backend.h"
#include "sha1.hpp"
#include <QTimer>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->webEngineView->setContextMenuPolicy(Qt::NoContextMenu);
    workpath=QApplication::applicationDirPath().toLocal8Bit().toStdString();
    if (!fs::exists(workpath / ".cache")) fs::create_directory(workpath / ".cache");
    std::string html =plot::loadhtml(workpath.string()+"/resource/template.html");
    Plot.setBaseHtml(html);
    CurrentHtml = (workpath / ".cache" / "Curr.html").string();
    CurrentVideo = "";
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
    ui->webEngineView->setHtml("Loading...");
    QFileInfo fileinfo = QFileInfo(filename);
    if(fileinfo.suffix().compare("csv",Qt::CaseInsensitive)==0)    setup_polt(filename.toStdString(),true,fileinfo.fileName().toStdString());
    else{
        std::string cachefileName=checksum(filename,10);
        fs::path cachefile=workpath / ".cache" / cachefileName;
        if (fs::exists(cachefile)){
            setup_polt(cachefile.string(),true,fileinfo.fileName().toStdString());
        }
        else{
            setup_polt(filename.toStdString(),fileinfo.fileName().toStdString(),cachefile.string());
        }
    }
    CurrentVideo=filename;
    loaded=true;
    return;
}


void MainWindow::setup_polt(std::string path,bool useCache,std::string filename){
    std::vector<FrameInfo> FrameInfoArray;
    if (useCache){
        FrameInfoArray = Backend::loadcsv(path);
    }
    else {
        FrameInfoArray = Backend::loadvideo(path);
    }
    CurrentResult = Backend::calc(FrameInfoArray);
    std::string html=Plot.applydata(CurrentResult,filename,false);
    //ui->webEngineView->setHtml(QString::fromStdString(html));
    plot::savehtml(CurrentHtml,html);
    ui->webEngineView->load(QUrl::fromLocalFile(QString::fromLocal8Bit(CurrentHtml.data())));
    return;
}

void MainWindow::setup_polt(std::string path,std::string filename,std::string savecache){
    std::vector<FrameInfo> FrameInfoArray;
    FrameInfoArray = Backend::loadvideo(path);
    Backend::savecsv(FrameInfoArray,savecache);
    CurrentResult = Backend::calc(FrameInfoArray);
    std::string html=Plot.applydata(CurrentResult,filename,false);
    //ui->webEngineView->setHtml(QString::fromStdString(html));
    plot::savehtml(CurrentHtml,html);
    ui->webEngineView->load(QUrl::fromLocalFile(QString::fromLocal8Bit(CurrentHtml.data())));
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
        return;
    }
    std::string html=Plot.applydata(CurrentResult,fileinfo.fileName().toStdString(),true);
    plot::savehtml(filename.toLocal8Bit().toStdString(),html);
    return;
}

inline std::string MainWindow::checksum(QString path,int maxCalcSize){
    uint32_t KB=65536;
    std::string checksum=SHA1::from_file(path.toLocal8Bit().toStdString(),KB*1024*maxCalcSize);
    SHA1 sha1;
    QFileInfo fileinfo = QFileInfo(path);
    sha1.update(fileinfo.fileName().toLocal8Bit().toStdString());
    checksum+=sha1.final();
    return checksum;
}
