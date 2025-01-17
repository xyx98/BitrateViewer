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


class Backend{
    public:
    static std::vector<FrameInfo> loadvideo(std::string path);
    static std::vector<FrameInfo> loadcsv(std::string path);
    static void savecsv(std::vector<FrameInfo> FrameInfoArray,std::string path);
    static CalcResult calc(std::vector<FrameInfo> FrameInfoArray);
    static void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c);
    template<typename T> static std::string ArrayToStringNumber(T array[],int length);
    static std::string ArrayToStringChar(char array[],int length);
    static FFmpegVersion getFFmpegVersion();
    static std::string FFverInt2String(int version);
};

#endif
