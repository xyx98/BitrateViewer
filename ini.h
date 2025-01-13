#ifndef INI_H
#define INI_H

#include <QSettings>

class INI{
public:
    INI(QString path);

    QSettings* config;

    void createINI();

    void setDecoder(QString key,QString value);
    std::string getDecoder(QString key);
    std::string getDecoder(QString key,bool Default);
};

#endif
