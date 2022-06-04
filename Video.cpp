#include "Video.h"

bool Vid::openFile(const char* path, Img& img){
    avFormatCntx = avformat_alloc_context();
    if ( !avFormatCntx ){
        printf("Couldn't make context!\n");
        return false;
    }

    if ( avformat_open_input(&avFormatCntx,path,NULL,NULL) != 0 ){
        printf("Couldn't open File!\n");
        return false;
    }

    const AVCodec* avCodec;
    for ( int i = 0; i < avFormatCntx->nb_streams; i ++ ){
        auto stream = avFormatCntx->streams[i];
        avCodecParams = stream->codecpar;
        avCodec = avcodec_find_decoder(avCodecParams->codec_id);
        if ( !avCodec )
            continue;
        if ( avCodecParams->codec_type == AVMEDIA_TYPE_VIDEO ){
            videoStreamInd = i;
            totalFrame = stream->nb_frames;
            timeBase = stream->time_base;
            break;
        }
    }

    if ( videoStreamInd == -1 ){
        printf("Couldn't find video stream\n");
        return false;
    }

    avCodecCntx = avcodec_alloc_context3(avCodec);
    if ( !avCodecCntx ){
        printf("Couldn't create codec context\n");
        return false;
    }
    if ( avcodec_parameters_to_context(avCodecCntx,avCodecParams) < 0 ){
        printf("Couldn't codecParam to context\n");
        return false;
    }
    if ( avcodec_open2(avCodecCntx,avCodec,NULL) < 0 ){
        printf("Couldn't open2\n");
        return false;
    }

    avFrame = av_frame_alloc();
    if ( !avFrame ) {
        printf("Couldn't load frame\n");
        return false;
    }
    avPacket = av_packet_alloc();
    if ( !avPacket ){
        printf("Couldn't packet\n");
        return false;
    }

    img.width = avCodecCntx->width;
    img.height = avCodecCntx->height;
    img.channel = 3;
    img.data = new unsigned char[img.width*img.height*img.channel];

    return true;
}

bool Vid::loadFrame(Img& img){
    int result;
    while ( av_read_frame(avFormatCntx,avPacket) >= 0 ){
        if ( avPacket->stream_index != videoStreamInd ){
            av_packet_unref(avPacket);
            continue;
        }
        result = avcodec_send_packet(avCodecCntx,avPacket);
        if ( result < 0 ){
            av_packet_unref(avPacket);
            return false;
        }
        
        result = avcodec_receive_frame(avCodecCntx,avFrame);
        if ( result == AVERROR(EAGAIN) || result == AVERROR_EOF) {
            av_packet_unref(avPacket);
            continue;
        } else if ( result < 0) {
            av_packet_unref(avPacket);
            return false;
        }
        av_packet_unref(avPacket);
        break;
    }

    frameLeft = totalFrame - avCodecCntx->frame_number;

    for ( int i = 0; i < img.width; i++ ){
        for ( int j = 0; j < img.height; j ++ ){
            int imgIndex = (j*img.width + i)*img.channel;
            int dataIndex = j * avFrame->linesize[0] + i;

            img.data[imgIndex]   = avFrame->data[0][dataIndex];
            img.data[imgIndex+1] = avFrame->data[0][dataIndex];
            img.data[imgIndex+2] = avFrame->data[0][dataIndex];
        }
    } 
    return true;
}

void Vid::closeFile(){
    avformat_close_input(&avFormatCntx);
    avformat_free_context(avFormatCntx);
    av_frame_free(&avFrame);
    av_packet_free(&avPacket);
    avcodec_free_context(&avCodecCntx);
}

