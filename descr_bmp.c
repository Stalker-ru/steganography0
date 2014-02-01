#include <stdio.h>
#include <stdlib.h>

// BMPheader
#pragma pack(push,2)
struct {
// 14 B
short signature;
int fileSize; 
short reserved1;
short reserved2;
int offsetToData; //=54
// 40 B	
int thisHeaderSize; //=40
int width; 
int height;
short colorDimensions;
short density;
int zipType;
int dataSize;
int horResolution;
int verResolution;
int colorsUsed;
int colorsImportant;
}BMPheader; //далее data: ColorTable (1024) + Image
#pragma pack(pop)

void info_pic () {
printf(">picture info:\n");
// 12 B: short signature; int fileSize; short unused1; short unused2; int offsetToData=54;
// 40 B: int thisHeaderSize=40; int width; int height; short colorDimensions; short density; int zipType;
// int massiveLength; int horResolution; int verResolution; int colorsQuantity; int shadesQuantity;
printf(">sizes: %i! %i! %i! %i!\n", BMPheader.fileSize, BMPheader.thisHeaderSize,
	BMPheader.offsetToData, BMPheader.dataSize);
printf(">signature: %i!\n", BMPheader.signature);
}

void pic_io(FILE* input, FILE* output, char* picData, short mode) {
if (fread(&BMPheader, sizeof(BMPheader), 1, input)<1) {
	errx(EXIT_FAILURE,"Pic could not be read");
}
if(BMPheader.signature!=19778) {
	errx(EXIT_FAILURE,"Incompatible pic format");
}

info_pic();

picData = (char*) malloc(BMPheader.dataSize);
fread(picData, BMPheader.dataSize, 1, input);
fwrite(&BMPheader, sizeof(BMPheader), 1, output);
fwrite(picData, BMPheader.dataSize, 1, output);

free(picData);
if(mode) {
printf("Picture written\n");
}
else {
printf("Picture read\n");
}
}
