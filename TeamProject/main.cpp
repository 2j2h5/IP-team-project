#include <windows.h>
#include <tchar.h>
#include <direct.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include "ximage.h"

const int SQUID_BACKGROUND[3] = { 40, 40 ,128 };
const int HEAD_COLOR[3] = { 197, 46, 39 };
const int BODY_COLOR[3] = { 46, 176, 48 };
const int BLACK_BACKGROUND[3] = { 0, 0, 0 };
const int POINT_COLOR[3] = { 7, 65, 102 };

/*
COLOR TABLE
used identify each pixels color in buffer, if you want to how it use see checkColor().

0 unknown
1 squid background
2 haad color
3 body color
4 black background
5 point color
*/

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

double colorDistance(const int color1[3], const int color2[3]) {
    return sqrt(pow(color1[0] - color2[0], 2) +
        pow(color1[1] - color2[1], 2) +
        pow(color1[2] - color2[2], 2));
}

void sharpenHead(CxImage& image) {
    DWORD height = image.GetHeight();
    DWORD width = image.GetWidth();

    for (DWORD y = 0; y < height; y++) {
        for (DWORD x = 0; x < width; x++) {
            RGBQUAD color = image.GetPixelColor(x, y);
            int color1[3] = { color.rgbRed, color.rgbGreen, color.rgbBlue };
            double backgroundDist = colorDistance(color1, SQUID_BACKGROUND);
            double headDist = colorDistance(color1, HEAD_COLOR);

            if (backgroundDist < headDist) {
                RGBQUAD newColor;
                newColor.rgbRed = SQUID_BACKGROUND[0];
                newColor.rgbGreen = SQUID_BACKGROUND[1];
                newColor.rgbBlue = SQUID_BACKGROUND[2];
                image.SetPixelColor(x, y, newColor);
            }
            else {
                RGBQUAD newColor;
                newColor.rgbRed = HEAD_COLOR[0];
                newColor.rgbGreen = HEAD_COLOR[1];
                newColor.rgbBlue = HEAD_COLOR[2];
                image.SetPixelColor(x, y, newColor);
            }
        }
    }
}

void sharpenBody(CxImage& image) {
    DWORD height = image.GetHeight();
    DWORD width = image.GetWidth();

    for (DWORD y = 0; y < height; y++) {
        for (DWORD x = 0; x < width; x++) {
            RGBQUAD color = image.GetPixelColor(x, y);
            int color1[3] = { color.rgbRed, color.rgbGreen, color.rgbBlue };
            double backgroundDist = colorDistance(color1, SQUID_BACKGROUND);
            double bodyDist = colorDistance(color1, BODY_COLOR);

            if (backgroundDist < bodyDist) {
                RGBQUAD newColor;
                newColor.rgbRed = SQUID_BACKGROUND[0];
                newColor.rgbGreen = SQUID_BACKGROUND[1];
                newColor.rgbBlue = SQUID_BACKGROUND[2];
                image.SetPixelColor(x, y, newColor);
            }
            else {
                RGBQUAD newColor;
                newColor.rgbRed = BODY_COLOR[0];
                newColor.rgbGreen = BODY_COLOR[1];
                newColor.rgbBlue = BODY_COLOR[2];
                image.SetPixelColor(x, y, newColor);
            }
        }
    }
}

void sharpenPoints(CxImage& image) {
    DWORD height = image.GetHeight();
    DWORD width = image.GetWidth();

    for (DWORD y = 0; y < height; y++) {
        for (DWORD x = 0; x < width; x++) {
            RGBQUAD color = image.GetPixelColor(x, y);
            int color1[3] = { color.rgbRed, color.rgbGreen, color.rgbBlue };
            double backgroundDist = colorDistance(color1, BLACK_BACKGROUND);
            double pointDist = colorDistance(color1, POINT_COLOR);

            if (backgroundDist < pointDist) {
                RGBQUAD newColor;
                newColor.rgbRed = BLACK_BACKGROUND[0];
                newColor.rgbGreen = BLACK_BACKGROUND[1];
                newColor.rgbBlue = BLACK_BACKGROUND[2];
                image.SetPixelColor(x, y, newColor);
            }
            else {
                RGBQUAD newColor;
                newColor.rgbRed = POINT_COLOR[0];
                newColor.rgbGreen = POINT_COLOR[1];
                newColor.rgbBlue = POINT_COLOR[2];
                image.SetPixelColor(x, y, newColor);
            }
        }
    }
}

void antialiasing(CxImage& image) {

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

int checkColor(int colorRed, int colorGreen, int colorBlue) {
    int const NOISE = 10;

    if (abs(colorRed - SQUID_BACKGROUND[0]) < NOISE &&
        abs(colorGreen - SQUID_BACKGROUND[1]) < NOISE &&
        abs(colorBlue - SQUID_BACKGROUND[2]) < NOISE) {
        return 1;
    }
    else if (abs(colorRed - HEAD_COLOR[0]) < NOISE &&
        abs(colorGreen - HEAD_COLOR[1]) < NOISE &&
        abs(colorBlue - HEAD_COLOR[2]) < NOISE) {
        return 2;
    }
    else if (abs(colorRed - BODY_COLOR[0]) < NOISE &&
        abs(colorGreen - BODY_COLOR[1]) < NOISE &&
        abs(colorBlue - BODY_COLOR[2]) < NOISE) {
        return 3; 
    }
    else if (abs(colorRed - BLACK_BACKGROUND[0]) < NOISE &&
        abs(colorGreen - BLACK_BACKGROUND[1]) < NOISE &&
        abs(colorBlue - BLACK_BACKGROUND[2]) < NOISE) {
        return 4;
    }
    else if (abs(colorRed - POINT_COLOR[0]) < NOISE &&
        abs(colorGreen - POINT_COLOR[1]) < NOISE &&
        abs(colorBlue - POINT_COLOR[2]) < NOISE) {
        return 5;
    }
    else {
        return 0;
    }
}

void addHead(int*** buffer, CxImage image) {
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

void addBody(int*** buffer, CxImage image) {
    DWORD height = image.GetHeight();
    DWORD width = image.GetWidth();

    for (DWORD y = 0;y < height;y++) {
        for (DWORD x = 0; x < width; x++) {
            RGBQUAD color = image.GetPixelColor(x, y);
            int buffer_color = checkColor(buffer[y][x][0], buffer[y][x][1], buffer[y][x][2]);
            int image_color = checkColor(color.rgbRed, color.rgbGreen, color.rgbBlue);

            if (buffer_color == 2 && image_color == 3) {
                buffer[y][x][0] = (buffer[y][x][0] + color.rgbRed) / 2;
                buffer[y][x][1] = (buffer[y][x][1] + color.rgbGreen) / 2;
                buffer[y][x][2] = (buffer[y][x][2] + color.rgbBlue) / 2;
            }
            else if (buffer_color == 1 && image_color == 3) {
                buffer[y][x][0] = color.rgbRed;
                buffer[y][x][1] = color.rgbGreen;
                buffer[y][x][2] = color.rgbBlue;
            }
        }
    }
}

void subPoints(int*** buffer, CxImage image) {
    DWORD height = image.GetHeight();
    DWORD width = image.GetWidth();

    for (DWORD y = 0;y < height;y++) {
        for (DWORD x = 0; x < width; x++) {
            RGBQUAD color = image.GetPixelColor(x, y);
            int buffer_color = checkColor(buffer[y][x][0], buffer[y][x][1], buffer[y][x][2]);
            int image_color = checkColor(color.rgbRed, color.rgbGreen, color.rgbBlue);

            if (image_color == 5) {
                buffer[y][x][0] = SQUID_BACKGROUND[0];
                buffer[y][x][1] = SQUID_BACKGROUND[1];
                buffer[y][x][2] = SQUID_BACKGROUND[2];
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

    sharpenHead(squidHead);
    sharpenBody(squidBody);
    sharpenPoints(squidPoints);

    //Modify buffer
    DWORD height = squidHead.GetHeight();
    DWORD width = squidHead.GetWidth();

    int*** buffer = getZeroBuffer(height, width);
    addHead(buffer, squidHead);
    addBody(buffer, squidBody);
    subPoints(buffer, squidPoints);

    //Saving result file
    CxImage result = getCxImage(buffer, height, width);
    save(result, "result.jpg");

    return 0;
}