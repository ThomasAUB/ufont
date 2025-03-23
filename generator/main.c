// thanks to : https://github.com/justinmeiners/stb-truetype-example 

#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_STATIC

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h" // http://nothings.org/stb/stb_image_write.h 

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h" // http://nothings.org/stb/stb_truetype.h 

#include "ufnt_writer.h"

int writeImage(const char* inFontPath, const char* inFileName, int inCharHeight, unsigned char bitResolution, const char* inAuthChars);

void writeBMPFile(const char* inFontPath, int imgWidth, int imgHeight, unsigned char* img, unsigned char resolution);

void createCharTable(const char* inAuthChars, int inAuthCharsCount, char* ioSortedTable);

int main(int argc, const char* argv[]) {

    const char* fontFilePath = "../../font/Inconsolata-Regular.ttf";
    int height = 32;
    int width = 0;
    unsigned char bitResolution = 8;

    char charTable[96] = {
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F
    };

    switch (argc) {

        case 5:
            createCharTable(argv[4], strnlen(argv[4], 95), charTable);
        case 4:
            bitResolution = atoi(argv[3]);
        case 3:
            height = atoi(argv[2]);
        case 2:
            fontFilePath = argv[1];
            break;

        default:
            printf("usage: %s font.ttf height [width]\n", argv[0]);
            printf("\n");
            printf("       rasterizes the provided ttf font into cells with specified height in pixels\n");
            printf("       if width is provided, it overrides the auto detected value\n");
            printf("       the generated raster data is printed on the standard output\n");
            break;
    }

    char filename[128];
    {

        { // extract file name
            char* lastDashPos = strrchr(fontFilePath, '/');

            if (lastDashPos) {
                strcpy(filename, lastDashPos + 1);
            }
        }

        { // remove file extension
            char* lastDotPos = strrchr(filename, '.');

            if (lastDotPos) {
                *lastDotPos = 0;
            }
        }

    }

    char bmpFileName[128];
    snprintf(bmpFileName, 128, "%s-h%d_r%d.bmp", filename, height, bitResolution);

    char binFileName[128];
    snprintf(binFileName, 128, "%s-h%d_r%d.ufnt", filename, height, bitResolution);

    int result = writeImage(fontFilePath, bmpFileName, height, bitResolution, charTable);

    if (result != 0) {
        return result;
    }

    return writeUFNTFile(bmpFileName, binFileName, bitResolution, charTable);
}

int writeImage(const char* inFontPath, const char* inFileName, int inCharHeight, unsigned char bitResolution, const char* inAuthChars) {

    unsigned char* fontBuffer;

    // load font file //
    {
        long size;
        FILE* fontFile = fopen(inFontPath, "rb");
        fseek(fontFile, 0, SEEK_END);
        size = ftell(fontFile);
        fseek(fontFile, 0, SEEK_SET);
        fontBuffer = malloc(size);
        fread(fontBuffer, size, 1, fontFile);
        fclose(fontFile);
    }

    // prepare font
    stbtt_fontinfo info;
    if (!stbtt_InitFont(&info, fontBuffer, 0)) {
        printf("failed to load ttf font\n");
        return -2;
    }

    int b_w = 0; // bitmap width
    int b_h = inCharHeight; // bitmap height
    int c_w = 0; // char width
    int c_h = inCharHeight; // char height

    // calculate font scaling
    float scale = stbtt_ScaleForPixelHeight(&info, c_h);

    // determine max char width
    const int kAuthCharsCount = strnlen(inAuthChars, 95);
    for (unsigned char i = 0; i < kAuthCharsCount; ++i) {

        unsigned char ch = inAuthChars[i];

        int ax;
        int lsb;
        stbtt_GetCodepointHMetrics(&info, ch, &ax, &lsb);
        if (c_w < ax * scale) {
            c_w = ax * scale;
        }
    }

    b_w = kAuthCharsCount * c_w;

    // create a bitmap for the phrase
    unsigned char* bitmap = calloc(b_w * b_h, sizeof(unsigned char));

    int x = 0, maxx = 0;
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);

    ascent *= scale;
    descent *= scale;

    for (unsigned char i = 0; i < kAuthCharsCount; ++i) {

        unsigned char ch = inAuthChars[i];

        // how wide is this character
        int ax;
        int lsb;
        stbtt_GetCodepointHMetrics(&info, ch, &ax, &lsb);

        // get bounding box for character (may be offset to account for chars that dip above or below the line
        int c_x1, c_y1, c_x2, c_y2;
        stbtt_GetCodepointBitmapBox(&info, ch, scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

        // compute y (different characters have different heights
        int y = ascent + c_y1;
        if (y < 0) y = 0;

        // render character (stride and offset is important here)
        int byteOffset = x + (lsb * scale) + (y * b_w);
        stbtt_MakeCodepointBitmap(&info, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, b_w, scale, scale, ch);

        // advance x
        //x += ax*scale; // use true character width
        x += c_w; // force mono-space width

        // add kerning
        if (i < kAuthCharsCount - 1) {
            int kern;
            kern = stbtt_GetCodepointKernAdvance(&info, ch, ch + 1);
            x += kern * scale;
        }
    }

    writeBMPFile(inFileName, b_w, b_h, bitmap, bitResolution);

    free(fontBuffer);
    free(bitmap);

    return 0;
}

void writeBMPFile(const char* inFontPath, int imgWidth, int imgHeight, unsigned char* img, unsigned char bitResolution) {

    for (int i = 0; i < imgWidth * imgHeight; i++) {

        // extract MSBs
        unsigned char csq = img[i] >> (8 - bitResolution);

        // fill byte with MSBs
        for (unsigned char r = bitResolution; r < 8; r *= 2) {
            csq |= csq << r;
        }

        img[i] = csq;
    }

    // save out a 1 channel image
    stbi_write_bmp(inFontPath, imgWidth, imgHeight, 1, img);

}

void createCharTable(const char* inAuthChars, int inAuthCharsCount, char* ioSortedTable) {

    int tableIndex = 0;

    memset(ioSortedTable, 0, strlen(ioSortedTable));

    for (char c = ' '; c < 127; c++) {
        for (int i = 0; i <= inAuthCharsCount; i++) {
            if (inAuthChars[i] == c) {
                ioSortedTable[tableIndex++] = c;
                break;
            }
        }
    }
}