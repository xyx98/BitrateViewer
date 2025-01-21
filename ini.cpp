#include "config.h"
#include "ini.h"

#include <QFile>


INI::INI(QString path){
    config = new QSettings(path, QSettings::IniFormat);
    if(!QFile::exists(path)) createINI();
}

void INI::createINI(){
    config->beginGroup("config");
    config->setValue("template",DEFAULTTEMPLATE);
    config->endGroup();
}

QString INI::getTmpl(){
    return config->value("config/template").toString();
}

void INI::setTmpl(QString str){
    config->setValue("config/template",str);
    config->sync();
}
