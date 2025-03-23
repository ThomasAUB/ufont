#pragma once

struct { // 4 + 12 + 12 + 4 = 32

    unsigned char version;
    unsigned char char_width;
    unsigned char char_height;

    unsigned char alpha_full : 3;
    unsigned char reserved : 5;

    unsigned char bif_field[12];
    unsigned char sums[12];

    unsigned char reserved2[4];

} typedef binheader;

binheader h;