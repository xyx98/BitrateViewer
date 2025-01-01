#include "plot.h"
#include "backend.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>

plot::plot(){
    basehtml="";
}

plot::plot(std::string basehtml){
    this->basehtml=basehtml;
}

void plot::setBaseHtml(std::string basehtml){
    this->basehtml=basehtml;
}

std::string plot::loadhtml(std::string path){
    std::ifstream in(path);
    std::istreambuf_iterator<char> beg(in), end;
    std::string html(beg, end);
    return html;
}

std::string plot::applydata(CalcResult result,std::string filename){
    std::stringstream ss;
    ss <<"x = "             << Backend::ArrayToStringNumber(result.frames,result.length)    << ";"      << std::endl;
    ss <<"  y = "           << Backend::ArrayToStringNumber(result.bitrates,result.length)  << ";"      << std::endl;
    ss <<"  pts = "         << Backend::ArrayToStringNumber(result.pts,result.length)       << ";"      << std::endl;
    ss <<"  pict_type = "   << Backend::ArrayToStringChar(result.pict_type,result.length)   << ";"      << std::endl;
    ss <<"  avg = "         << result.avg                                                   << ";"      << std::endl;
    ss <<"  name = \""      << filename                                                     << "\";"    << std::endl;
    std::regex expr("\\{\\{\\{datas\\}\\}\\}");
    return std::regex_replace(basehtml, expr, ss.str());;
}

void plot::savehtml(std::string path,std::string html){
    std::ofstream file;
    file.open(path);
    file << html;
    file.close();
};
