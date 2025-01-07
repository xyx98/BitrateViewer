#include "plot.h"
#include "backend.h"
#include "config.h"
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

std::string plot::applydata(CalcResult result,std::string filename,bool isSave){
    std::stringstream ss;
    ss <<"x = "             << Backend::ArrayToStringNumber(result.frames,result.length)    << ";"      << std::endl;
    ss <<"  y = "           << Backend::ArrayToStringNumber(result.bitrates,result.length)  << ";"      << std::endl;
    ss <<"  pts = "         << Backend::ArrayToStringNumber(result.pts,result.length)       << ";"      << std::endl;
    ss <<"  pict_type = "   << Backend::ArrayToStringChar(result.pict_type,result.length)   << ";"      << std::endl;
    ss <<"  key = "         << Backend::ArrayToStringNumber(result.key,result.length)       << ";"      << std::endl;
    ss <<"  pkt_size = "    << Backend::ArrayToStringNumber(result.pkt_size,result.length)  << ";"      << std::endl;
    ss <<"  avg = "         << result.avg                                                   << ";"      << std::endl;
    ss <<"  length = "      << result.length                                                << ";"      << std::endl;
    ss <<"  name = \""      << filename                                                     << "\";"    << std::endl;
    std::string res=plot::templateReplace(basehtml,ss.str(),"datas");
    if (isSave){
        res=plot::templateReplace(res,"true","isSave");
        res=plot::templateReplace(res,ONLINE_ECHARTS,"echarts");
    }
    else{
        res=plot::templateReplace(res,"false","isSave");
        res=plot::templateReplace(res,"../"+std::string(OFFLINE_ECHARTS),"echarts");
    }
    return res;
}

void plot::savehtml(std::string path,std::string html){
    std::ofstream file;
    file.open(path);
    file << html;
    file.close();
};

inline std::string plot::templateReplace(std::string base,std::string data,std::string templ){
    std::regex expr("\\{\\{\\{"+templ+"\\}\\}\\}");
    return std::regex_replace(base, expr, data);
}
