#ifndef PLOT_H
#define PLOT_H

#include <string>
#include "backend.h"

class plot{
public:
    std::string basehtml;

    plot();
    plot(std::string basehtml);
    void setBaseHtml(std::string basehtml);
    std::string applydata(CalcResult result,std::string filename);

    static std::string loadhtml(std::string path);
    static std::string showString(std::string);
    static void savehtml(std::string path,std::string html);
};


#endif
