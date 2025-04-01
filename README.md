![build status](https://github.com/ThomasAUB/ufont/actions/workflows/build.yml/badge.svg)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# ufont

This repo contains a font rasterizer in the generator directory that is able to convert a TTF file into a bmp file and a ufnt file. The include directory contains the file ufont.hpp that allows to read ufnt files at run-time and compile-time.

The header of an ufnt file weights 32 bytes is structured like this :

- version [8]
- char_width [8]
- char_height [8]
- resolution [3]
- reserved [5]
- states[96]
- sums[96]
- padding[32]

The resolution is encoded a follow :

- 0 : 1 bit resolution
- 1 : 2 bits resolution
- 2 : 4 bits resolution
- 3 : 8 bits resolution

The states field is a bitfield that tells if a character is enabled in the raster.

The sums field is the cumulative sum of enabled characters by chunks of 8 characters.


```cpp

static constexpr uint8_t font_8[] = {
    #include "Inconsolata-Regular-h32_r8.ufnt"
};

void foo() {

    static constexpr ufont::Reader fnt(font_8);

    static constexpr auto raster = fnt.raster('A');

    // this library allows to parse the font at compile-time
    static_assert(raster.getPixel(0, 0) == 0, "Invalid pixel value");

    for (int y = 0; y < raster.height(); y++) {

        for (int x = 0; x < raster.width(); x++) {

            uint8_t pixel = raster.getPixel(x, y);

            // draw pixel
            // ...

        }
    }

}

```