#include <iostream>
#include <stdint.h>

static constexpr uint8_t font[] = {
    #include "../generator/build/raster-h32_8.txt"
};

struct CharRaster {

    constexpr CharRaster(const uint8_t* inBuffer, char inChar) :
        kCharWidth(*(inBuffer + 1)),
        kCharHeight(*(inBuffer + 2)),
        kBitResolution(*(inBuffer + 3) & 3),
        kBuffer(
            inBuffer +
            16 +
            (
                ((inChar - ' ') * kCharWidth * kCharHeight * (1 << kBitResolution)) >> 3
            )
        )
    {}

    constexpr uint8_t getPixel(uint8_t inX, uint8_t inY) const {
        const uint32_t pix = (inX + inY * kCharWidth) << kBitResolution;
        const uint8_t val = (kBuffer[pix >> 3] >> (pix & 7));
        return (val << 8) >> (1 << kBitResolution);
    }

    constexpr uint8_t width() const { return kCharWidth; }

    constexpr uint8_t height() const { return kCharHeight; }

    constexpr uint8_t resolution() const { return 1 << kBitResolution; }

private:
    const uint8_t kCharWidth;
    const uint8_t kCharHeight;
    const uint8_t kBitResolution;
    const uint8_t* kBuffer;
};


int main() {

    static constexpr CharRaster raster(font, '4');

    static_assert(raster.width() == 15, "ERR");
    static_assert(raster.height() == 32, "ERR");
    //static_assert(raster.resolution() == 2, "ERR");


    static constexpr char kOpChar[] = {" `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@"};

    for (int y = 0; y < raster.height(); y++) {

        for (int x = 0; x < raster.width(); x++) {

            const unsigned char pixel = raster.getPixel(x, y);

            //std::cout << kOpChar[(pixel * pixel * (sizeof(kOpChar) - 1)) / (0xFF * 0xFF)];

            if (pixel == 0) {
                std::cout << " ";
            }
            else if (pixel < 255 / 3) {
                std::cout << ".";
            }
            else if (pixel < (255 * 2) / 3) {
                std::cout << "*";
            }
            else {
                std::cout << "@";
            }

        }
        std::cout << std::endl;
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