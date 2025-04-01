#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "ufont.hpp"
#include "a_char.h"


static constexpr uint8_t font_8[] = {
    #include "Inconsolata-Regular-h32_r8.ufnt"
};


TEST_CASE("basic ufont tests") {

    static constexpr ufont::Reader fnt(font_8);

    static_assert(fnt.width() == 15, "Invalid pixel value");
    static_assert(fnt.height() == 32, "Invalid pixel value");
    static_assert(fnt.raster('A').getPixel(0, 0) == 0, "Invalid pixel value");
    static_assert(fnt.raster('!').getPixel(fnt.width() / 2, fnt.height() / 2) == 255, "Invalid pixel value");

    const auto raster = fnt.raster('A');

    for (int y = 0; y < raster.height(); y++) {

        for (int x = 0; x < raster.width(); x++) {

            const unsigned char pixel = raster.getPixel(x, y);

            char v;

            if (pixel == 0) {
                v = ' ';
            }
            else if (pixel < 255 / 3) {
                v = '.';
            }
            else if (pixel < (255 * 2) / 3) {
                v = '*';
            }
            else {
                v = '@';
            }

            CHECK(a_char[y][x] == v);
        }
    }

}

