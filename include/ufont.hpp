/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MIT License                                                                     *
 *                                                                                 *
 * Copyright (c) 2025 Thomas AUBERT                                                *
 *                                                                                 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy    *
 * of this software and associated documentation files (the "Software"), to deal   *
 * in the Software without restriction, including without limitation the rights    *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell       *
 * copies of the Software, and to permit persons to whom the Software is           *
 * furnished to do so, subject to the following conditions:                        *
 *                                                                                 *
 * The above copyright notice and this permission notice shall be included in all  *
 * copies or substantial portions of the Software.                                 *
 *                                                                                 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
 * SOFTWARE.                                                                       *
 *                                                                                 *
 * github : https://github.com/ThomasAUB/uFont                                     *
 *                                                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

#include <stdint.h>

namespace ufont {

    struct CharRaster {

        constexpr CharRaster(
            const uint8_t* inBuffer,
            uint8_t inCharWidth,
            uint8_t inCharHeight,
            uint8_t inResolutionShift
        ) :
            kCharWidth(inCharWidth),
            kCharHeight(inCharHeight),
            kResolutionShift(inResolutionShift),
            kBuffer(inBuffer) {}

        constexpr uint8_t getPixel(uint8_t inX, uint8_t inY) const {

            if (inX >= kCharWidth || inY >= kCharHeight) { return 0xFF; }

            const uint8_t kShiftedRes = 1 << kResolutionShift;
            const uint32_t pix = (inX + inY * kCharWidth) << kResolutionShift;

            uint8_t val =
                (kBuffer[pix >> 3] >> (pix & 7)) &
                ((1 << kShiftedRes) - 1);

            if (kResolutionShift == 0) {
                return ~(val - 1);
            }

            return (val << 8) >> kShiftedRes;
        }

        constexpr uint8_t width() const { return kCharWidth; }

        constexpr uint8_t height() const { return kCharHeight; }

        constexpr uint8_t resolution() const { return 1 << kResolutionShift; }

    private:
        const uint8_t kCharWidth;
        const uint8_t kCharHeight;
        const uint8_t kResolutionShift;
        const uint8_t* kBuffer;
    };





    struct Reader {

        constexpr Reader(const uint8_t* inBuffer) :
            kBuffer(inBuffer),
            kResolutionShift(inBuffer[resolution_offset] & 3) {}

        constexpr CharRaster raster(char inChar) const {

            if (
                (inChar >= ' ' && inChar < 127) ||
                version() != current_version
                ) {

                inChar -= ' ';

                const uint8_t mask = 1 << (inChar & 7);
                const uint8_t fieldIndex = inChar >> 3;
                const uint8_t charStates = (kBuffer + char_state_offset)[fieldIndex];

                // check if char is enabled
                if (charStates & mask) {

                    // count the enabled chars before it
                    if (fieldIndex == 0) {
                        // char is in the first 8 ones
                        inChar = 0;
                    }
                    else {
                        inChar = (kBuffer + sum_state_offset)[fieldIndex - 1];
                    }

                    // count the bit set in the byte field
                    constexpr uint8_t nibble_sum[16] = {
                        0, 1, 1, 2, 1, 2, 2, 3,
                        1, 2, 2, 3, 2, 3, 3, 4
                    };
                    const uint8_t rest = charStates & (mask - 1);
                    inChar += (nibble_sum[rest >> 4] + nibble_sum[rest & 0xF]);

                    return CharRaster(
                        kBuffer + header_size + (inChar * width() * height() >> (3 - kResolutionShift)),
                        width(),
                        height(),
                        kResolutionShift
                    );

                }
            }
            // invalid char
            return CharRaster(nullptr, 0, 0, 0);
        }

        constexpr uint8_t version() const { return kBuffer[version_offset]; }

        constexpr uint8_t width() const { return kBuffer[char_width_offset]; }

        constexpr uint8_t height() const { return kBuffer[char_height_offset]; }

        constexpr uint8_t resolution() const { return 1 << kResolutionShift; }

        constexpr bool contains(char c) const {
            if (c < ' ' || c >= 127) { return false; }
            c -= ' ';
            const uint8_t charStates = (kBuffer + char_state_offset)[c >> 3];
            return charStates & (1 << (c & 7));
        }

        constexpr uint8_t count() const {
            return kBuffer[sum_state_offset + 11];
        }

    private:
        static constexpr uint8_t version_offset = 0;
        static constexpr uint8_t char_width_offset = 1;
        static constexpr uint8_t char_height_offset = 2;
        static constexpr uint8_t resolution_offset = 3;
        static constexpr uint8_t char_state_offset = 4;
        static constexpr uint8_t sum_state_offset = 16;
        static constexpr uint8_t header_size = 32;

        static constexpr uint8_t current_version = 0;

        const uint8_t* kBuffer;
        const uint8_t kResolutionShift;
    };

}