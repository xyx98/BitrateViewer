#ifndef BACKEND_H
#define BACKEND_H

#include <vector>
#include <cstdint>
#include <string>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/cpu.h>
}


typedef struct FrameInfo{
    char        pict_type;
    int         pkt_size;
    int64_t     pts;
    int         keyframe; 
    int64_t     duration;
    std::string codec;
    int         fpsNum;
    int         fpsDen;
} FrameInfo;

typedef struct CalcResult{
    int* frames;
    float* bitrates;
    int64_t* pts; 
    char* pict_type;
    int* key;
    int* pkt_size;
    int length;
    float avg;
    std::string codec;
    int fpsNum;
    int fpsDen;
} CalcResult;

typedef struct FFmpegVersion{
    std::string license;
    std::string libavutil;
    std::string libavcodec;
    std::string libavformat;
} FFmpegVersion;


namespace Backend{
    std::vector<FrameInfo> loadvideo(std::string path);
    std::vector<FrameInfo> loadcsv(std::string path);
    void savecsv(std::vector<FrameInfo> FrameInfoArray,std::string path);
    CalcResult calc(std::vector<FrameInfo> FrameInfoArray);
    void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c);
    template<typename T> std::string ArrayToStringNumber(T array[],int length);
    std::string ArrayToStringChar(char array[],int length);
    FFmpegVersion getFFmpegVersion();
    std::string FFverInt2String(int version);
};

#endif
