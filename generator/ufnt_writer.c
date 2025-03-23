#include "ufnt_writer.h"
#include <stdio.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct { // 4 + 12 + 12 + 4 = 32

    unsigned char version;
    unsigned char char_width;
    unsigned char char_height;

    unsigned char alpha_full : 3;
    unsigned char reserved : 5;

    unsigned char bif_field[12];
    unsigned char sums[12];

    unsigned char padding[4];

} typedef binheader;



void writeHexa(FILE* inFile, unsigned char byte, char addComa) {
    char buff[5];
    sprintf(buff, "0x%02X", byte);
    fwrite(buff, 1, strlen(buff), inFile);
    if (addComa) {
        fwrite(", ", 1, strlen(", "), inFile);
    }
}



int writeUFNTFile(const char* inBMPFileName, const char* inBinFileName, unsigned char inBitResolution, const char* inAuthChars) {

    int bmp_width;
    int bmp_height;
    int channels;

    unsigned char* img = stbi_load(inBMPFileName, &bmp_width, &bmp_height, &channels, 0);

    if (img == 0) {
        return -1;
    }

    FILE* binFile = fopen(inBinFileName, "w");

    if (!binFile) {
        printf("couldn't create binary file");
        return -1;
    }

    const unsigned int kAuthCharsCount = strnlen(inAuthChars, 95);

    binheader h;

    memset(&h, 0, sizeof(binheader));

    h.version = 0;
    h.char_width = bmp_width / kAuthCharsCount;
    h.char_height = bmp_height;

    switch (inBitResolution) {

        case 1:
            h.alpha_full = 0; // 1 bit
            break;

        case 2:
        case 3:
            h.alpha_full = 1; // 2 bits
            break;

        case 4:
        case 5:
        case 6:
        case 7:
            h.alpha_full = 2; // 4 bits
            break;

        default:
            h.alpha_full = 3; // 8 bits
            break;
    }

    inBitResolution = 1 << h.alpha_full;

    { // enable authorized chars

        // compute state bit field
        int tableIndex = 0;
        for (unsigned char c = ' '; c <= 127; c++) {

            int i = c - ' ';
            unsigned char mask = 1 << (i % 8);
            unsigned char* val = &h.bif_field[i / 8];

            if (inAuthChars[tableIndex] == c) {
                *val |= mask;
                if (tableIndex < kAuthCharsCount - 1) {
                    tableIndex++;
                }
            }
            else {
                *val &= ~mask;
            }
        }
    }

    // compute bitfield sums
    for (unsigned char i = 0; i < 12; i++) {
        // sum set bits in byte
        h.sums[i] = 0;
        for (int b = 0; b < 8; b++) {
            if (h.bif_field[i] & (1 << b)) {
                h.sums[i]++;
            }
        }
        if (i > 0) {
            h.sums[i] += h.sums[i - 1];
        }
    }


    // write header
    for (int i = 0; i < 32; i++) {
        writeHexa(binFile, ((unsigned char*) &h)[i], 1);
    }

    fwrite("\n", 1, 1, binFile);

    const int max = (1 << inBitResolution) - 1;

    unsigned char byte = 0;
    unsigned char shift = 0;
    unsigned char lineCounter = 0;

    for (int counter = 0; counter < kAuthCharsCount; counter++) {

        const unsigned int xStart = counter * h.char_width;

        for (unsigned int _y = 0; _y < h.char_height; _y++) {

            unsigned int pixel = (_y * bmp_width + xStart) * 3;

            for (unsigned int _x = 0; _x < h.char_width; _x++) {

                unsigned char c = img[pixel] >> (8 - inBitResolution);

                pixel += 3;
                byte |= c << shift;
                shift += inBitResolution;

                if (shift == 8) {

                    // store byte
                    writeHexa(binFile, byte, pixel < (3 * bmp_width * bmp_height));

                    shift = 0;
                    byte = 0;

                    if (++lineCounter == 16) {
                        fwrite("\n", 1, strlen("\n"), binFile);
                        lineCounter = 0;
                    }

                }

            }

        }
    }

    if (shift != 0) {
        writeHexa(binFile, byte, 0);
    }

    stbi_image_free(img);
    fclose(binFile);

    return 0;
}