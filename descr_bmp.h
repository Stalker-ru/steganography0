#ifndef DESCR_BMP
#define DESCR_BMP
#include <stdio.h>
#include <stdlib.h>
struct BMPheader;
void info_pic();
void pic_io(FILE* input, FILE* output, char* picData, short mode);
#endif
