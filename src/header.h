#pragma once

struct { // 16 bytes

    unsigned char version;
    unsigned char char_width;
    unsigned char char_height;

    unsigned char alpha_full : 3; // [3 bits for alpha channel, 1 bit to tell is it has a table or not
    unsigned char reserved : 5;

    unsigned char field[(95 + 7) / 8]; // 95 chars max

} typedef binheader;

binheader h;