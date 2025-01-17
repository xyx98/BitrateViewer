#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "backend.h"
#include "sha1.hpp"
#include "config.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->webEngineView->setContextMenuPolicy(Qt::NoContextMenu);
    ini= new INI(QApplication::applicationDirPath()+"/"+SETTINGFILE);
    workpath=QApplication::applicationDirPath().toLocal8Bit().toStdString();
    if (!fs::exists(workpath / ".cache")) fs::create_directory(workpath / ".cache");
    std::string html =plot::loadhtml(workpath.string()+"/template/"+ini->getTmpl().toStdString());
    Plot.setBaseHtml(html);
    CurrentHtml = (workpath / ".cache" / "Curr.html").string();
    CurrentVideo = "";
    loadStatus=0; //-1:loading 0:unloaded 1:loaded

    connect(this,SIGNAL(loadhtml(QUrl)),this,SLOT(LoadHTML(QUrl)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionopen_triggered()
{
    if (loadStatus <= -1) return;
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
            synchronizer.addFuture(QtConcurrent::run([=](){setup_polt(filename.toStdString(),fileinfo.fileName().toStdString(),cachefile.string());}));
        }
    }
    return;
}

void MainWindow::setup_polt(std::string path,bool useCache,std::string filename){
    std::vector<FrameInfo> FrameInfoArray;
    loadStatus=-1;
    if (useCache){
        FrameInfoArray = Backend::loadcsv(path);
    }
    else {
        FrameInfoArray = Backend::loadvideo(path);
    }

    CurrentResult = Backend::calc(FrameInfoArray);
    std::string html=Plot.applydata(CurrentResult,filename,false);
    plot::savehtml(CurrentHtml,html);
    emit loadhtml(QUrl::fromLocalFile(QString::fromLocal8Bit(CurrentHtml.data())));
    CurrentVideo=QString::fromStdString(filename);
    loadStatus=1;
    return;
}


void MainWindow::setup_polt(std::string path,std::string filename,std::string savecache){
    std::vector<FrameInfo> FrameInfoArray;
    loadStatus=-1;
    FrameInfoArray = Backend::loadvideo(path);
    Backend::savecsv(FrameInfoArray,savecache);
    CurrentResult = Backend::calc(FrameInfoArray);
    std::string html=Plot.applydata(CurrentResult,filename,false);
    plot::savehtml(CurrentHtml,html);
    emit loadhtml(QUrl::fromLocalFile(QString::fromLocal8Bit(CurrentHtml.data())));
    CurrentVideo=QString::fromStdString(filename);
    loadStatus=1;
    return;
}


void MainWindow::on_actionsave_triggered()
{
    if (loadStatus <= 0) return;
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

void MainWindow::LoadHTML(QUrl url){
    ui->webEngineView->load(url);
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

void MainWindow::on_actionAbout_triggered()
{
    FFmpegVersion ffver=Backend::getFFmpegVersion();
    QString aboutString = "<h2>BitrateViewer v" BITRATEVIEWER_VERSION "</h2>\n<p>by xyx98<br/>https://github.com/xyx98/BitrateViewer";
    aboutString.append("<br/>This software is free and distributed under GPL-3.0 license</p>");

    aboutString.append("<p><pre>Software uses FFmpeg project.<br/>https://ffmpeg.org/<br/>Current using build distributed under "+ffver.license);
    aboutString.append("<br/>libavutil\t\t"+ffver.libavutil);
    aboutString.append("<br/>libavcodec\t\t"+ffver.libavcodec);
    aboutString.append("<br/>libavformat\t"+ffver.libavformat+"</pre></p>");

    aboutString.append("<p>Software uses Qt framework by The Qt Company, distributed under LGPL license.<br/>https://qt.io/");
    aboutString.append(QString("<br/>Built with Qt %1</p>").arg(QT_VERSION_STR));

    aboutString.append("<p>Software uses Apache ECharts by Apache Software Foundation, distributed under Apache License 2.0.<br/>https://echarts.apache.org/<p>");
    QMessageBox::about(this,"About BitrateViewer",aboutString);
}

void MainWindow::on_actiontemplate_triggered()
{
    QMessageBox::information(this,"choose template","Not implemented yet.\nSet it in setting.ini instead.");
}
