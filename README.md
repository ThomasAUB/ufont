# ufont


call the program with the character width and height and a bitmap
-> generates a ufnt file that's able to regenerate a new font size at compile time



for example a bitmap containing 95 characters of width 8 and height 16 will be

95 * 8 = 760 pixel width and 16 pixels height



ASCII

 !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~


Generate a raster image


Generate 4 binary files with a resolution of 1, 2, 4 and 8 bits of a custom font file (.ufnt ?)


Could be cool if the binary files where generated from the bitmap, that would allow to draw fonts and then generate them.




## Steps

- Generate a bitmap file by specifying the font file, the output height,

- Generate the binary files by specifying the bitmap file, the char  (should the reduced char set should be there ? or option below)

- Try to generate at compile-time a binary that only includes specified characters
