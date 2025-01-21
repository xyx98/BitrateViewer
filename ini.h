#ifndef INI_H
#define INI_H

#include <QSettings>

class INI{
public:
    INI(QString path);

    QSettings* config;

    void createINI();

    QString getTmpl();
    void setTmpl(QString str);
};

#endif
