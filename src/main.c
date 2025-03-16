
#include <stdio.h>

const char font[] = {
    #include "../generator/build/raster-h32_8.txt"
};

int main(int argc, const char* argv[]) {

    struct {

        unsigned char version;
        unsigned char width;
        unsigned char height;

        // | b | b | b | b | f | a | a | a |

        unsigned char alpha_full; // [3 bits for alpha channel, 1 bit to tell is it has a table or not

        unsigned char field[12];
        //unsigned char table[193];

    } typedef binheader;

    binheader* header = (binheader*) font;


    printf("header :\n");
    printf("version : %d\n", header->version);
    printf("width : %d\n", header->width);
    printf("height : %d\n", header->height);
    printf("alpha_full : %d\n", header->alpha_full);

    unsigned char* fontBuff = font + 16;

    unsigned char buff[160];

    for (int i = 0; i < header->width; i++) {
        if (fontBuff[i]) {
            printf("*");
        }
        else {
            printf(" ");
        }
    }

    /*
        for (int y = 16; y < header->height * header->width * 95; y++) {

            if (font[y]) {
                printf("*");
            }
            else {
                printf(" ");
            }


            if (y % (160 + 16) == 0) {
                printf("\n");
            }

        }
    */
    /*
    int i = 16;
    int i2 = 0;
    for (int y = 0; y < header->height; y++) {

        i2 = 0;

        for (; i < header->width; i++) {
            buff[i2] = '0' + font[y * i];
            i2++;
        }

        printf(buff);

    }
*/


}