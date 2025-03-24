#pragma once

#include <stdint.h>
#include <array>
#include "ufont.hpp"

#define UFONT_STATIC_CONSTEXPR static constexpr

//#define UFONT_STATIC_CONSTEXPR static inline

namespace ufont {

    template<
        uint8_t char_height,
        uint8_t resolution,
        const uint8_t* matrix_buffer
    >
    struct FontScaler {

        static constexpr auto* getBuffer() {
            return buffer.data();
        }

    private:

        static constexpr uint8_t char_state_offset = 4;
        static constexpr uint8_t sum_state_offset = 16;
        static constexpr uint8_t header_size = 32;
        static constexpr uint8_t char_count = matrix_buffer[sum_state_offset + 11];

        static constexpr auto treader = Reader(matrix_buffer);

        static constexpr uint8_t kW = (treader.width() * char_height + treader.height() / 2) / treader.height();
        static constexpr uint8_t kH = char_height;

        static constexpr uint8_t getReducedResolution() {
            switch (resolution) {
                case 0:
                case 1:
                    return 0;
                case 2:
                case 3:
                    return 1;
                case 4:
                case 5:
                case 6:
                case 7:
                    return 2;
                default:
                    return 3;
            }
        }

        using buffer_t = std::array<
            uint8_t,
            header_size + ((treader.count() * kW * kH) << getReducedResolution())
        >;

        static constexpr void computeCharBuffer(uint8_t* ioBuffer) {

            uint32_t buffIndex = 0;

            constexpr uint8_t resoShift = 1 << getReducedResolution();

            uint8_t byte = 0;
            uint8_t shift = 0;

            for (char c = ' '; c < 127; c++) {

                if (!treader.contains(c)) {
                    continue;
                }

                const uint8_t* char_raster = matrix_buffer + header_size + (c * treader.width() * treader.height() >> (3 - getReducedResolution()));

                for (uint8_t y = 0; y < kH; y++) {

                    for (uint8_t x = 0; x < kW; x++) {

                        const uint32_t pix = ((x * treader.width()) / kW + ((y * treader.height()) / kH) * treader.width()) << getReducedResolution();
                        const uint8_t val = (char_raster[pix >> 3] >> (pix & 7));
                        (val << 8) >> (1 << getReducedResolution());

                        byte |= c << shift;
                        shift += resoShift;

                        if (shift == 8) {
                            ioBuffer[buffIndex++] = byte;
                            shift = 0;
                            byte = 0;
                        }
                    }

                }


                /*
                                    auto raster = treader.raster(c);

                                for (uint8_t y = 0; y < kH; y++) {

                                    for (uint8_t x = 0; x < kW; x++) {

                                        uint8_t c = raster.getPixel(
                                            (x * raster.width()) / kW,
                                            (y * raster.height()) / kH) >> (8 - resoShift);

                                        byte |= c << shift;
                                        shift += resoShift;

                                        if (shift == 8) {
                                            ioBuffer[buffIndex++] = byte;
                                            shift = 0;
                                            byte = 0;
                                        }

                                    }

                                }
                */
            }

        }

        static constexpr buffer_t computeBuffer() {

            buffer_t buffer {

                0, // version

                kW, kH,

                getReducedResolution(),

                matrix_buffer[char_state_offset],
                matrix_buffer[char_state_offset + 1],
                matrix_buffer[char_state_offset + 2],
                matrix_buffer[char_state_offset + 3],
                matrix_buffer[char_state_offset + 4],
                matrix_buffer[char_state_offset + 5],
                matrix_buffer[char_state_offset + 6],
                matrix_buffer[char_state_offset + 7],
                matrix_buffer[char_state_offset + 8],
                matrix_buffer[char_state_offset + 9],
                matrix_buffer[char_state_offset + 10],
                matrix_buffer[char_state_offset + 11],

                matrix_buffer[sum_state_offset],
                matrix_buffer[sum_state_offset + 1],
                matrix_buffer[sum_state_offset + 2],
                matrix_buffer[sum_state_offset + 3],
                matrix_buffer[sum_state_offset + 4],
                matrix_buffer[sum_state_offset + 5],
                matrix_buffer[sum_state_offset + 6],
                matrix_buffer[sum_state_offset + 7],
                matrix_buffer[sum_state_offset + 8],
                matrix_buffer[sum_state_offset + 9],
                matrix_buffer[sum_state_offset + 10],
                matrix_buffer[sum_state_offset + 11],

                0, 0, 0, 0
            };

            computeCharBuffer(buffer.data() + header_size);
            return buffer;
        }

        static constexpr auto buffer = computeBuffer();
    };

}
