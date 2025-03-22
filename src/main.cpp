#include <iostream>
#include <stdint.h>
#include "header.h"

static constexpr uint8_t font[] = {
    #include "../generator/build/raster-h32_8.txt"
};

struct CharRaster {

    CharRaster(const uint8_t* inBuffer, char inChar) :
        kCharWidth(((binheader*) inBuffer)->char_width),
        kCharHeight(((binheader*) inBuffer)->char_height),
        kBitResolution(1 << ((binheader*) inBuffer)->alpha_full),
        kBuffer(
            inBuffer + 16 +
            (
                ((inChar - ' ') * kCharWidth * kCharHeight * kBitResolution) >> 3
            )
        )
    {}

    uint8_t getPixel(uint8_t inX, uint8_t inY) {
        const int pix = (inX + inY * kCharWidth) * kBitResolution;
        const int shift = pix & 7;
        const unsigned char mask = (1 << kBitResolution) - 1;
        unsigned char val = (kBuffer[pix >> 3] >> shift) & mask;
        return (val << 8) >> kBitResolution;
    }
    
    uint8_t width() const {
        return kCharWidth;
    }

    uint8_t height() const {
        return kCharHeight;
    }

private:
    const uint8_t kCharWidth;
    const uint8_t kCharHeight;
    const uint8_t kBitResolution;
    const uint8_t* kBuffer;
};

int main() {
/*
    binheader* header = (binheader*) font;

    printf("version : %d\n", header->version);
    printf("width : %d\n", header->char_width);
    printf("height : %d\n", header->char_height);
    printf("alpha_full : %d\n", header->alpha_full);
*/
    //CharRaster raster(font + 16, 'r', header->char_width, header->char_height, 1 << header->alpha_full);

    CharRaster raster(font, 'r');

    for (int y = 0; y < raster.height(); y++) {

        for (int x = 0; x < raster.width(); x++) {

            const unsigned char pixel = raster.getPixel(x, y);

            if (pixel == 0) {
                printf(" ");
            }
            else if (pixel < 255 / 3) {
                printf(".");
            }
            else if (pixel < (255 * 2) / 3) {
                printf("*");
            }
            else {
                printf("@");
            }

        }
        printf("\n");
    }

}


char getPixel(const unsigned char* inBuffer, char inChar, char charWidth, char charHeight, char resolution, int x, int y) {

    const unsigned int offset = ((inChar - ' ') * charWidth * charHeight * resolution) >> 3;
    const unsigned char* fontBuff = inBuffer + offset;

    const int pix = (x + y * charWidth) * resolution;

    const int shift = pix & 7;

    const unsigned char mask = (1 << resolution) - 1;

    unsigned char val = (fontBuff[pix >> 3] >> shift) & mask;

    //val = (val * 255) / ((1 << resolution) - 1);
    //val = (val << 8) / (1 << resolution);

    return (val << 8) >> resolution;
}