#include "backend.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace Backend{

    std::vector<FrameInfo> loadvideo(std::string path){
        const char* input_file = path.c_str();
        std::vector<FrameInfo> FrameInfoArray;
        FrameInfo tmpFrameInfo;
        // Open input file and allocate format context
        AVFormatContext* format_ctx = nullptr;
        if (avformat_open_input(&format_ctx, input_file, nullptr, nullptr) < 0) {
            std::cerr << "Could not open input file: " << input_file << std::endl;
            return FrameInfoArray;
        }

        // Retrieve stream information
        if (avformat_find_stream_info(format_ctx, nullptr) < 0) {
            std::cerr << "Could not find stream information" << std::endl;
            return FrameInfoArray;
        }

        // Find video stream
        int video_stream_idx = -1;
        for (unsigned int i = 0; i < format_ctx->nb_streams; ++i) {
            if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                video_stream_idx = i;
                break;
            }
        }
        if (video_stream_idx == -1) {
            std::cerr << "Could not find video stream" << std::endl;
            return FrameInfoArray;
        }

        int fpsNum=format_ctx->streams[video_stream_idx]->avg_frame_rate.num;
        int fpsDen=format_ctx->streams[video_stream_idx]->avg_frame_rate.den;
        // Get codec parameters and codec context
        AVCodecParameters* codecpar = format_ctx->streams[video_stream_idx]->codecpar;
        const AVCodec* codec = avcodec_find_decoder(codecpar->codec_id);
        std::string codec_name = avcodec_get_name(codecpar->codec_id);
        AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
        codec_ctx->thread_count = av_cpu_count(); //set decode threads
        avcodec_parameters_to_context(codec_ctx, codecpar);
        if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
            std::cerr << "Could not open codec" << std::endl;
            return FrameInfoArray;
        }

        // Allocate frame and packet
        AVFrame* frame = av_frame_alloc();
        AVPacket packet;
        av_init_packet(&packet);

        // Read frames from the video stream
        while (av_read_frame(format_ctx, &packet) >= 0) {
            if (packet.stream_index == video_stream_idx) {
                // Decode video frame
                int ret = avcodec_send_packet(codec_ctx, &packet);
                if (ret < 0) {
                    std::cerr << "Error sending packet for decoding" << std::endl;
                    break;
                }

                while (ret >= 0) {
                    ret = avcodec_receive_frame(codec_ctx, frame);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                        break;
                    } else if (ret < 0) {
                        std::cerr << "Error during decoding" << std::endl;
                        break;
                    }
                FrameInfo tmpFrameInfo={
                    av_get_picture_type_char(frame->pict_type),
                    frame->pkt_size,
                    frame->pts,
                    frame->key_frame,
                    frame->duration,
                    codec_name,
                    fpsNum,
                    fpsDen
                    };
                FrameInfoArray.push_back(tmpFrameInfo);
                }
            }
            av_packet_unref(&packet);
        }
        std::sort(
            FrameInfoArray.begin(),
            FrameInfoArray.end(),
            [](FrameInfo a,FrameInfo b){
                return a.pts < b.pts;
            }
        );
        // Free allocated resources
        av_frame_free(&frame);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return FrameInfoArray;
    }

    std::vector<FrameInfo> loadcsv(std::string path){
        std::vector<FrameInfo> FrameInfoArray;
        std::ifstream file(path);
        std::string line;
        //int count=0;
        std::getline(file,line); //skip first line
        std::stringstream ss;
        while (std::getline(file,line)){
            FrameInfo tmpFrameInfo;
            std::vector<std::string> lineArray;
            Backend::SplitString(line,lineArray,"\t");

            ss << lineArray[1]; ss >> tmpFrameInfo.pict_type;   ss.clear();
            ss << lineArray[2]; ss >> tmpFrameInfo.pkt_size;    ss.clear();
            ss << lineArray[3]; ss >> tmpFrameInfo.pts;         ss.clear();
            ss << lineArray[4]; ss >> tmpFrameInfo.keyframe;    ss.clear();
            ss << lineArray[5]; ss >> tmpFrameInfo.duration;    ss.clear();
            ss << lineArray[6]; ss >> tmpFrameInfo.codec;       ss.clear();
            ss << lineArray[7]; ss >> tmpFrameInfo.fpsNum;      ss.clear();
            ss << lineArray[8]; ss >> tmpFrameInfo.fpsDen;      ss.clear();
            FrameInfoArray.push_back(tmpFrameInfo);
        }

        file.close();
        return FrameInfoArray;
    }

    CalcResult calc(std::vector<FrameInfo> FrameInfoArray){
        int length=FrameInfoArray.size();
        CalcResult result;
        result.frames = (int*)calloc(length,sizeof(int));
        result.bitrates = (float*)calloc(length,sizeof(float));
        result.pts = (int64_t*)calloc(length,sizeof(int64_t));
        result.pict_type = (char*)calloc(length,sizeof(char));
        result.key = (int*)calloc(length,sizeof(int));
        result.pkt_size = (int*)calloc(length,sizeof(int));
        result.length=length;
        result.codec=FrameInfoArray[0].codec;
        result.fpsNum=FrameInfoArray[0].fpsNum;
        result.fpsDen=FrameInfoArray[0].fpsDen;
        uint64_t totalsize =0 ;

        for(int i=0;i<length;i++){
            result.frames[i]=i;
            result.pict_type[i]=FrameInfoArray[i].pict_type;
            result.key[i]=FrameInfoArray[i].keyframe;
            result.pkt_size[i]=FrameInfoArray[i].pkt_size;
            if (i<length-1) result.bitrates[i]=8.0 * FrameInfoArray[i].pkt_size/(FrameInfoArray[i+1].pts-FrameInfoArray[i].pts);
            else result.bitrates[i]=8.0 * FrameInfoArray[i].pkt_size/FrameInfoArray[i].duration;
            result.pts[i]=FrameInfoArray[i].pts;
            totalsize += FrameInfoArray[i].pkt_size * 8;
        }

        float avg =1.0 * totalsize / (FrameInfoArray[length-1].pts-FrameInfoArray[0].pts);
        result.avg=avg;
        return result;
    }

    void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c){
      std::string::size_type pos1, pos2;
      pos2 = s.find(c);
      pos1 = 0;
      while(std::string::npos != pos2)
      {
        v.push_back(s.substr(pos1, pos2-pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
      }
      if(pos1 != s.length())
        v.push_back(s.substr(pos1));
    }

    void savecsv(std::vector<FrameInfo> FrameInfoArray,std::string path){
        std::ofstream file;
        file.open(path);
        if (file.is_open()){
            file << "n\tpict_type\tpkt_size\tpts\tkeyframe\tduration\tcodec\tfpsnum\tfpsden" <<std::endl;
            for (int i=0;i<FrameInfoArray.size();i++){
                file    << i << "\t"
                        << FrameInfoArray[i].pict_type   << "\t"
                        << FrameInfoArray[i].pkt_size    << "\t"
                        << FrameInfoArray[i].pts         << "\t"
                        << FrameInfoArray[i].keyframe    << "\t"
                        << FrameInfoArray[i].duration    << "\t"
                        << FrameInfoArray[i].codec       << "\t"
                        << FrameInfoArray[i].fpsNum      << "\t"
                        << FrameInfoArray[i].fpsDen      << std::endl;
            }
        }
        else{
            std::cout << "Failed to open the file:" << path << std::endl;
        }

        file.close();
    }

    template<typename T> std::string ArrayToStringNumber(T array[],int length){
        std::stringstream ss;
        ss << "[";
        for(int i=0;i<length;i++){
            if (i==length-1) ss << array[i] << "]";
            else ss << array[i] << ",";
        }
        return ss.str();
    }

    std::string ArrayToStringChar(char array[],int length){
        std::stringstream ss;
        ss << "[";
        for(int i=0;i<length;i++){
            if (i==length-1)    ss << "'" << array[i] << "']";
            else                ss << "'" << array[i] << "',";
        }
        return ss.str();
    }

    FFmpegVersion getFFmpegVersion(){
        FFmpegVersion ffver;
        ffver.license=avcodec_license();
        ffver.libavcodec=Backend::FFverInt2String(avcodec_version());
        ffver.libavutil=Backend::FFverInt2String(avutil_version());
        ffver.libavformat=Backend::FFverInt2String(avformat_version());
        return ffver;
    }

    inline std::string FFverInt2String(int version){
        int a = version / (int) pow(2, 16);
        int b = (int) (version - a * pow(2, 16)) / (int) pow(2, 8);
        int c = version % (int) pow(2, 8);
        std::stringstream ss;
        ss << a << "." << b << "." << c;
        return ss.str();
    }

    std::vector<std::string> findTemplate(fs::path path){
        std::vector<std::string> tmpls;
        if (!exists(path)) return tmpls;
        fs::directory_entry entry(path);
        if (entry.status().type() == fs::file_type::directory){
            fs::directory_iterator list(path);
            for (auto& it:list){
                if (it.path().extension() == ".tmpl") tmpls.push_back(it.path().filename());
            }
        }
        std::sort(tmpls.begin(),tmpls.end());
        return tmpls;
    }

    template std::string Backend::ArrayToStringNumber<int>(int array[],int length);
    template std::string Backend::ArrayToStringNumber<float>(float array[],int length);
    template std::string Backend::ArrayToStringNumber<int64_t>(int64_t array[],int length);

}
