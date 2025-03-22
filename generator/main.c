// thanks to : https://github.com/justinmeiners/stb-truetype-example 

#include <stdio.h>
#include <stdlib.h>

//#include <math.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h" // http://nothings.org/stb/stb_image_write.h 


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h" // http://nothings.org/stb/stb_truetype.h 

#include "../src/header.h"

static const int startCharIndex = 32;
static const int endCharIndex = 127;
static const int charCount = endCharIndex - startCharIndex;

int renderFont(unsigned char* fontBuffer, int inCharHeight, unsigned char bitResolution);

int writeImage(const char* inFontName, const char* inFileName, int inCharHeight, unsigned char bitResolution);

int writeBinaryFile(const char* inBMPFileName, const char* inBinFileName, int inCharHeight, int inCharWidth, int inBitResolution);

void writeBMPFile(const char* inFileName, int imgWidth, int imgHeight, unsigned char* img, unsigned char resolution);

int main(int argc, const char* argv[]) {

    const char* fonFileName = "../../font/Inconsolata-Regular.ttf";
    int height = 32;
    int width = 0;
    int bitResolution = 8;

    if (argc == 4) {
        /*
                printf("usage: %s font.ttf height [width]\n", argv[0]);
                printf("\n");
                printf("       rasterizes the provided ttf font into cells with specified height in pixels\n");
                printf("       if width is provided, it overrides the auto detected value\n");
                printf("       the generated raster data is printed on the standard output\n");
                return -1;
        */

        //argv[1] = "../../font/Inconsolata-Regular.ttf";
        //argv[2] = "32";
        //argv[3] = "8";

        fonFileName = argv[1];
        height = atoi(argv[2]);
        bitResolution = atoi(argv[3]);
    }

    char bmpFileName[128];
    snprintf(bmpFileName, 128, "raster-h%d_%d.bmp", height, bitResolution);

    char binFileName[128];
    snprintf(binFileName, 128, "raster-h%d_%d.txt", height, bitResolution);


    int result = writeImage(fonFileName, bmpFileName, height, bitResolution);

    if (result != 0) {
        return result;
    }

    writeBinaryFile(bmpFileName, binFileName, height, width, bitResolution);

    return 0;
}

void writeHexa(FILE* inFile, unsigned char byte, char addComa) {
    char buff[5];
    sprintf(buff, "0x%02X", byte);
    fwrite(buff, 1, strlen(buff), inFile);
    if (addComa) {
        fwrite(", ", 1, strlen(", "), inFile);
    }
}

int writeBinaryFile(const char* inBMPFileName, const char* inBinFileName, int inCharHeight, int inCharWidth, int inBitResolution) {

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

    binheader h;

    h.version = 0;
    h.char_width = bmp_width / charCount;
    h.char_height = bmp_height;

    switch (inBitResolution) {

        case 1:
            h.alpha_full = 0; // 1 bit
            break;

        case 2:
        case 3:
            h.alpha_full = 1; // 2 bits
            inBitResolution = 2;
            break;

        case 4:
        case 5:
        case 6:
        case 7:
            h.alpha_full = 2; // 4 bits
            inBitResolution = 4;
            break;

        default:
            h.alpha_full = 3; // 8 bits
            inBitResolution = 8;
            break;
    }

    // enable all chars
    for (int i = 0; i < sizeof(h.field); i++) {
        h.field[i] = 255;
    }

    // write readable file
    const char* c_header = "#pragma once\nconst char* fnt = {";
    //fwrite(c_header, 1, strlen(c_header), binFile);

    // write header
    for (int i = 0; i < 16; i++) {
        writeHexa(binFile, ((uint8_t*) &h)[i], 1);
    }

    fwrite("\n", 1, 1, binFile);

    const int max = (1 << inBitResolution) - 1;

    uint8_t byte = 0;
    uint8_t shift = 0;

    uint8_t lineCounter = 0;

    for (int counter = 0; counter < charCount; counter++) {

        const unsigned int xStart = counter * h.char_width;

        for (unsigned int _y = 0; _y < h.char_height; _y++) {

            unsigned int pixel = (_y * bmp_width + xStart) * 3;

            for (unsigned int _x = 0; _x < h.char_width; _x++) {

                unsigned char c = img[pixel];

                pixel += 3;

                /*
                if (c == 0) {
                    fwrite(" ", 1, strlen("\n"), binFile);
                }
                else if (c < 128) {
                    fwrite(".", 1, strlen("\n"), binFile);
                }
                else {
                    fwrite("X", 1, strlen("\n"), binFile);
                }
                */

                byte |= ((c * max + 127) / 255) << shift;
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

            //fwrite("\n", 1, strlen("\n"), binFile);
        }
    }

    if (shift != 0) {
        writeHexa(binFile, byte, 0);
    }

    stbi_image_free(img);
    fclose(binFile);

    return 0;
}

int writeImage(const char* inFontName, const char* inFileName, int inCharHeight, unsigned char bitResolution) {

    unsigned char* fontBuffer;

    // load font file //
    {
        long size;
        FILE* fontFile = fopen(inFontName, "rb");
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
    for (unsigned char ch = startCharIndex; ch < endCharIndex; ++ch) {
        int ax;
        int lsb;
        stbtt_GetCodepointHMetrics(&info, ch, &ax, &lsb);
        if (c_w < ax * scale) {
            c_w = ax * scale;
        }
    }

    b_w = charCount * c_w;

    // create a bitmap for the phrase
    unsigned char* bitmap = calloc(b_w * b_h, sizeof(unsigned char));

    int x = 0, maxx = 0;
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);

    ascent *= scale;
    descent *= scale;

    // begin C constants output
    //printf("static const int FONT_SIZE_X = %d;\n", c_w);
    //printf("static const int FONT_SIZE_Y = %d;\n\n", c_h);
    //printf("static const unsigned char kFontRaster[128][FONT_SIZE_Y*FONT_SIZE_X] = {\n");

    for (unsigned char ch = startCharIndex; ch < endCharIndex; ++ch) {
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

        // output C array data
        //printf("    {");
        //for (int yy = 0; yy < c_h; ++yy) {
        //    for (int xx = 0; xx < c_w; ++xx) {
        //        int v = bitmap[byteOffset + (yy - y) * b_w + xx];
        //        printf(" %3d,", v);
        //        if (v > 0 && xx > maxx) {
        //            maxx = xx;
        //        }
        //    }
        //}
        //printf(" }, // U+%04d\n", ch);

        // advance x
        //x += ax*scale; // use true character width
        x += c_w; // force mono-space width

        // add kerning
        if (ch < endCharIndex - 1) {
            int kern;
            kern = stbtt_GetCodepointKernAdvance(&info, ch, ch + 1);
            x += kern * scale;
        }
    }

    //printf("};\n");

    //if (maxx + 1 < c_w) {
    //    printf("// warning : max character width was %d. consider lowering the width parameter to this value\n", maxx + 1);
    //}

    //writeBMPFile(inFileName, b_w, b_h, bitmap, 8);
    writeBMPFile(inFileName, b_w, b_h, bitmap, bitResolution);


    free(fontBuffer);
    free(bitmap);

    return 0;
}

void writeBMPFile(const char* inFileName, int imgWidth, int imgHeight, unsigned char* img, unsigned char bitResolution) {

    const unsigned char max = ((1 << bitResolution) - 1);
    const unsigned char halfMax = max >> 1;

    for (int i = 0; i < imgWidth * imgHeight; i++) {
        unsigned char csq = (img[i] * max + 127) / 255;
        csq = (csq * 255 + halfMax) / max;
        img[i] = csq;
    }

    // save out a 1 channel image
    stbi_write_bmp(inFileName, imgWidth, imgHeight, 1, img);

}

int renderFont(unsigned char* fontBuffer, int inCharHeight, unsigned char bitResolution) {

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

    if (c_w == 0) {
        // determine max char width
        for (unsigned char ch = startCharIndex; ch < endCharIndex; ++ch) {
            int ax;
            int lsb;
            stbtt_GetCodepointHMetrics(&info, ch, &ax, &lsb);
            if (c_w < ax * scale) {
                c_w = ax * scale;
            }
        }
    }

    b_w = charCount * c_w;

    // create a bitmap for the phrase
    unsigned char* bitmap = calloc(b_w * b_h, sizeof(unsigned char));

    int x = 0, maxx = 0;
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);

    ascent *= scale;
    descent *= scale;

    // begin C constants output
    //printf("static const int FONT_SIZE_X = %d;\n", c_w);
    //printf("static const int FONT_SIZE_Y = %d;\n\n", c_h);
    //printf("static const unsigned char kFontRaster[128][FONT_SIZE_Y*FONT_SIZE_X] = {\n");

    for (unsigned char ch = startCharIndex; ch < endCharIndex; ++ch) {
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

        // output C array data
        //printf("    {");
        //for (int yy = 0; yy < c_h; ++yy) {
        //    for (int xx = 0; xx < c_w; ++xx) {
        //        int v = bitmap[byteOffset + (yy - y) * b_w + xx];
        //        printf(" %3d,", v);
        //        if (v > 0 && xx > maxx) {
        //            maxx = xx;
        //        }
        //    }
        //}
        //printf(" }, // U+%04d\n", ch);

        // advance x
        //x += ax*scale; // use true character width
        x += c_w; // force mono-space width

        // add kerning
        if (ch < endCharIndex - 1) {
            int kern;
            kern = stbtt_GetCodepointKernAdvance(&info, ch, ch + 1);
            x += kern * scale;
        }
    }

    return b_w;
}