#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string>
#include <algorithm>
#include <chrono>
#include <unistd.h>

#include "Image.h"
#include "Video.h"

int main(){
    Img finalImg;
    Vid video;

    bool programRun = true;
    int pixelSize;
    int choose;

    std::string path;

    std::string asciiScale = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;;::,,\"\"^^```'''....     ";
    std::string result;
    std::reverse(asciiScale.begin(), asciiScale.end());

    std::chrono::system_clock::time_point start;
    std::chrono::duration<double> sec;

    while ( programRun ){
        std::cout << "경로 입력 --경로에 한글이 포함되있으면 작동되지 않을 수 있습니다.--" << std::endl;
        std::cin >> path;

        std::cout << "픽셀사이즈 입력:";
        std::cin >> pixelSize;

        std::cout << "이미지 입니까, 동영상 입니까? ( img = 0, vid = 1 ) ";
        std::cin >> choose;

        if ( choose == 0 ){ 
            finalImg.loadImgFile(path.c_str());
        }else if ( choose == 1 ) {
            if ( !video.openFile(path.c_str(), finalImg) ){
                std::cout << "can't open!! Error!!" << std::endl;
                break;
            }
            start = std::chrono::system_clock::now();
        }else { break; }

        do {
            if ( choose == 1 ){
                if ( !video.loadFrame(finalImg) ){
                    programRun = false;
                    std::cout << "can't load frame!! Error!!" << std::endl;
                    break;
                }
            }

            bool success = finalImg.processingImg(pixelSize, result, [=, &finalImg](std::string& result, int index ){
                double grayScale;
                if ( finalImg.channel == 3 ){ 
                    grayScale = finalImg.data[index]+finalImg.data[index+1]+finalImg.data[index+2]; 
                }
                if ( finalImg.channel == 4 ){
                    grayScale = (finalImg.data[index]+finalImg.data[index+1]+finalImg.data[index+2])*finalImg.data[index+3];
                }
                grayScale /= finalImg.channel;
                grayScale /= 255.0*3.0;
                grayScale *= asciiScale.size();
                grayScale = floor(grayScale);

                result += asciiScale[grayScale];
                result += asciiScale[grayScale];
            });
        
            if ( !success ){
                std::cout << "Error occured!!" << std::endl;
                programRun = false;
                break;
            }else{
                sec = std::chrono::system_clock::now() - start;
                double pt_sec = video.avFrame->pts*(double)video.timeBase.num/(double)video.timeBase.den;
                //system("clear");
                std::cout << result;
                
                while( sec.count() < pt_sec ){
                    usleep((pt_sec - sec.count())*1000);
                    sec = std::chrono::system_clock::now() - start;
                }
                result.clear();
            }

        }while(choose == 1 && video.frameLeft > 1);

        if( programRun ){
            printf("더 진행하고 싶나요?(0 or 1) : ");
            scanf("%d", &programRun);
        }
        if ( choose == 0 )
            finalImg.freeData();
        if ( choose == 1 ){
            delete finalImg.data;
            video.closeFile();
            
        }
        
    }
    return 0;
}
