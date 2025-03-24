#include <iostream>
#include <stdint.h>
#include "ufont.hpp"

#include "ufont_scaler.hpp"

// 45152
static constexpr uint8_t font[] = {
    #include "../generator/build/Inconsolata-Regular-h32_r8.ufnt"
};


int main() {

    using scaler_t = ufont::FontScaler<128, 1, font>;

    static constexpr auto fnt = ufont::Reader(scaler_t::getBuffer());


    /*static constexpr*/ //ufont::Reader fnt(font);

    //static_assert(fnt.raster('A').getPixel(0, 0) == 0, "okdjf");

    //static constexpr CharRaster raster(font, '4');

    //static_assert(fnt.width() == 16, "ERR");
    //static_assert(fnt.height() == 16, "ERR");
    //static_assert(fnt.resolution() == 4, "ERR");

    char c = 'a';
    for (int i = 0; i < 5; i++) {

        auto raster = fnt.raster(c);

        for (int y = 0; y < fnt.height(); y++) {

            for (int x = 0; x < fnt.width(); x++) {

                //static constexpr char kOpacity[] = " `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";

                const unsigned char pixel = raster.getPixel(x, y);

                //uint16_t sq = ((pixel * pixel) * (sizeof(kOpacity) - 1)) / (0xFF * 0xFF);
                //std::cout << kOpacity[sq];

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
        c++;
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