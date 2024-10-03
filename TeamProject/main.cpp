#include <windows.h>
#include <tchar.h>
#include <direct.h>
#include <iostream>
#include "ximage.h"

int main() {

    CxImage image;

    if (image.Load(_T("squid_head.jpg"), CXIMAGE_FORMAT_JPG)) {
        std::cout << "Image loaded" << std::endl;
        
        image.GrayScale();

        if (image.Save(_T("output.jpg"), CXIMAGE_FORMAT_JPG)) {
            std::cout << "Image saved" << std::endl;
        }
        else {
            std::cout << "Image save failed" << std::endl;
        }
    }
    else {
        std::cout << "Image load failed" << std::endl;
    }

    return 0;
}