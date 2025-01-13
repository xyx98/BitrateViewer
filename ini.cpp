#include "config.h"
#include "ini.h"

#include <QFile>


INI::INI(QString path){
    config = new QSettings(path, QSettings::IniFormat);
    if(!QFile::exists(path)) createINI();
}

void INI::createINI(){
    config->beginGroup("Decoder");
    config->setValue("H264",DEFAULTDECODERH264);
    config->setValue("H265",DEFAULTDECODERH265);
    config->setValue("VP9", DEFAULTDECODERVP9 );
    config->setValue("AV1", DEFAULTDECODERAV1 );
    config->endGroup();
    config->sync();
}

void INI::setDecoder(QString key,QString value){
    config->setValue("Decoder/"+key,value);
    config->sync();
}

std::string INI::getDecoder(QString key){
    return config->value("Decoder/"+key).toString().toStdString();
}

std::string INI::getDecoder(QString key,bool Default){
    if(Default) return config->value("Decoder/"+key).toString().toStdString();
    else{
        if      (key=="H264") return config->value("Decoder/"+key,DEFAULTDECODERH264).toString().toStdString();
        else if (key=="H265") return config->value("Decoder/"+key,DEFAULTDECODERH265).toString().toStdString();
        else if (key=="VP9 ") return config->value("Decoder/"+key,DEFAULTDECODERVP9 ).toString().toStdString();
        else if (key=="AV1 ") return config->value("Decoder/"+key,DEFAULTDECODERAV1 ).toString().toStdString();
        else                  return "";
    }
}
