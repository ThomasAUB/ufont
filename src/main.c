
#include <stdio.h>

#include "header.h"

const unsigned char font[] = {
    #include "../generator/build/raster-h32_8.txt"
};


char getPixel(const unsigned char* inBuffer, char inChar, char charWidth, char charHeight, char resolution, int x, int y) {

    const unsigned int offset = ((inChar - ' ') * charWidth * charHeight * resolution) / 8;
    const unsigned char* fontBuff = inBuffer + offset;

    const int pix = (x + y * charWidth) * resolution;

    const int shift = pix % 8;

    const unsigned char mask = (1 << resolution) - 1;

    unsigned char val = (fontBuff[pix >> 3] >> shift) & mask;

    //val = (val * 255) / ((1 << resolution) - 1);

    val = (val << 8) / (1 << resolution);

    return val;
}

int main(int argc, const char* argv[]) {

    binheader* header = (binheader*) font;

    printf("version : %d\n", header->version);
    printf("width : %d\n", header->char_width);
    printf("height : %d\n", header->char_height);
    printf("alpha_full : %d\n", header->alpha_full);

    for (int y = 0; y < header->char_height; y++) {

        for (int x = 0; x < header->char_width; x++) {

            const unsigned char pixel = getPixel(font + 16, 'G', header->char_width, header->char_height, 1 << header->alpha_full, x, y);

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
                printf("X");
            }

        }
        printf("\n");
    }

}