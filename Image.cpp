#include "Image.h"

void Img::freeData( void ){
    stbi_image_free(data);
}

void Img::loadImgFile(const char* path){
    data = stbi_load(path,&width,&height,&channel,0);
}
