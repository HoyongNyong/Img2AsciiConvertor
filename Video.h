#pragma once

#include "Image.h"

extern "C"{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
}

class Vid{
    public:

    int totalFrame;
    int frameLeft;

    AVRational timeBase;
    AVFormatContext* avFormatCntx;
    AVCodecParameters* avCodecParams;
    AVCodecContext* avCodecCntx;
    AVFrame* avFrame;
    AVPacket* avPacket;
    int videoStreamInd = -1;

    bool openFile(const char* path, Img& img);
    bool loadFrame(Img& img);
    void closeFile();
};
