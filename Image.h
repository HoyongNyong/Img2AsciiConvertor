#pragma once

#include "stb_image.h"
#include <string>

class Img {
    public :
    int width, height, channel;
    unsigned char* data = nullptr;

    void freeData( void );
    void loadImgFile(const char* path);
    template<typename T>
    bool processingImg(int pixelSize, std::string& result, T func){
        if ( data != nullptr ){
            for ( int j = 0; j < height; j += pixelSize ){
                for ( int i = 0; i < width; i += pixelSize ){
                    int index = (j*width + i)*channel;
                    func(result,index);
                }
                result += "\n";
            }
        } else { return false; }
        return true;
    }
};


