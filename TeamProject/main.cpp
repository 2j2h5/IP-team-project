#include <windows.h>
#include <tchar.h>
#include <direct.h>
#include <iostream>
#include <string>
#include "ximage.h"

void load(CxImage& image, std::string filename) {
    std::wstring wFilename(filename.begin(), filename.end());
    const TCHAR* tFilename = wFilename.c_str();
    if (image.Load(tFilename, CXIMAGE_FORMAT_JPG)) {
        std::cout << "Loading " << filename <<" is success!" << std::endl;
    }
    else {
        std::cout << "Loading " << filename << " is failed." << std::endl;
    }
}

void save(CxImage& image, std::string filename) {
    std::wstring wFilename(filename.begin(), filename.end());
    const TCHAR* tFilename = wFilename.c_str();
    if (image.Save(tFilename, CXIMAGE_FORMAT_JPG)) {
        std::cout << "Saving " << filename << " is success!" << std::endl;
    }
    else {
        std::cout << "Saving " << filename << " is failed." << std::endl;
    }
}

int*** getZeroBuffer(DWORD height, DWORD width) {
    int*** buffer = new int** [height]; //buffer[height][width][rgb]
    for (DWORD y = 0; y < height; y++) {
        buffer[y] = new int* [width];
        for (DWORD x = 0; x < width; x++) {
            buffer[y][x] = new int[3];
        }
    }

    return buffer;
}

CxImage getCxImage(int*** buffer, DWORD height, DWORD width) {
    CxImage image;
    image.Create(width, height, 24);

    for (DWORD y = 0; y < height; y++) {
        for (DWORD x = 0; x < width; x++) {
            RGBQUAD color;
            
            color.rgbRed = buffer[y][x][0];
            color.rgbGreen = buffer[y][x][1];
            color.rgbBlue = buffer[y][x][2];
            
            image.SetPixelColor(x, y, color);
        }
    }
    return image;
}

void writeImage(int*** buffer, CxImage image) {
    DWORD height = image.GetHeight();
    DWORD width = image.GetWidth();

    for (DWORD y = 0;y < height;y++) {
        for (DWORD x = 0; x < width; x++) {
            RGBQUAD color = image.GetPixelColor(x, y);
            buffer[y][x][0] = color.rgbRed;
            buffer[y][x][1] = color.rgbGreen;
            buffer[y][x][2] = color.rgbBlue;
        }
    }
}

void addImage(int*** buffer, CxImage image) {
    DWORD height = image.GetHeight();
    DWORD width = image.GetWidth();

    for (DWORD y = 0;y < height;y++) {
        for (DWORD x = 0; x < width; x++) {
            RGBQUAD color = image.GetPixelColor(x, y);
            buffer[y][x][0] = (buffer[y][x][0] + color.rgbRed) / 2;
            buffer[y][x][1] = (buffer[y][x][1] + color.rgbGreen) / 2;
            buffer[y][x][2] = (buffer[y][x][2] + color.rgbBlue) / 2;
        }
    }
}

void subtractImage(int*** buffer, CxImage image) {
    DWORD height = image.GetHeight();
    DWORD width = image.GetWidth();

    RGBQUAD backgroundColor = image.GetPixelColor(0, 0);
    int backgroundColorRed = backgroundColor.rgbRed;
    int backgroundColorGreen = backgroundColor.rgbGreen;
    int backgroundColorBlue = backgroundColor.rgbBlue;

    int bufferBackgroundColorRed = buffer[0][0][0];
    int bufferBackgroundColorGreen = buffer[0][0][1];
    int bufferBackgroundColorBlue = buffer[0][0][2];

    for (DWORD y = 0;y < height;y++) {
        for (DWORD x = 0; x < width; x++) {
            RGBQUAD color = image.GetPixelColor(x, y);

            if (!(color.rgbRed == backgroundColorRed && color.rgbGreen == backgroundColorGreen && color.rgbBlue == backgroundColorBlue)) {
                buffer[y][x][0] = bufferBackgroundColorRed;
                buffer[y][x][1] = bufferBackgroundColorGreen;
                buffer[y][x][2] = bufferBackgroundColorBlue;
            }
        }
    }
}

int main() {

    CxImage squidHead;
    CxImage squidBody;
    CxImage squidPoints;
    
    //Loading squid image files
    load(squidHead, "squid_head.jpg");
    load(squidBody, "squid_body.jpg");
    load(squidPoints, "squid_points.jpg");

    //Modify buffer
    DWORD height = squidHead.GetHeight();
    DWORD width = squidHead.GetWidth();

    int*** buffer = getZeroBuffer(height, width);
    writeImage(buffer, squidHead);
    addImage(buffer, squidBody);
    subtractImage(buffer, squidPoints);

    //Saving result file
    CxImage result = getCxImage(buffer, height, width);
    save(result, "result.jpg");

    return 0;
}