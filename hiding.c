// hiding.c
// steganography: hiding/finding a BMP (picture) file in a WAV (audio) file
// Bugakov G., 2014

#include <stdio.h>
#include <stdlib.h>
#include <err.h>

// WAVheader
struct {
// WAV-формат начинается с RIFF-заголовка:

// Содержит символы "RIFF" в ASCII кодировке
char chunkId[4];
// Это оставшийся размер цепочки, начиная с этой позиции =
// =(размер файла - 8) (исключены поля chunkId и chunkSize)
unsigned int chunkSize;

// Содержит символы "WAVE"
char format[4];
// Формат "WAVE" состоит из двух подцепочек: "fmt " и "data":
// Подцепочка "fmt " описывает формат звуковых данных:

// Содержит символы "fmt "
char subchunk1Id[4];
// Оставшийся размер подцепочки, начиная с этой позиции. 16 для формата PCM.
unsigned int subchunk1Size;
// Аудио формат, для PCM = 1.
// Значения, отличающиеся от 1, обозначают некоторый формат сжатия.
unsigned short audioFormat;
// Количество каналов. Моно = 1, Стерео = 2 и т.д.
unsigned short numChannels;
// Частота дискретизации. 8000 Гц, 44100 Гц и т.д.
unsigned int sampleRate;
// sampleRate * numChannels * bitsPerSample/8
unsigned int byteRate;
// Количество байт для одного сэмпла, включая все каналы = numChannels * bitsPerSample/8
unsigned short blockAlign;
// "Глубина" или точность звучания. 8 бит, 16 бит и т.д.
unsigned short bitsPerSample;

// Подцепочка "data" содержит аудио-данные и их размер.

// Содержит символы "data"
char subchunk2Id[4];
// Количество байт в области данных = numSamples * numChannels * bitsPerSample/8
unsigned int subchunk2Size;
}WAVheader; //далее data

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

void info_audio () {
printf("audio info:\n");
// char chunkId[4]; unsigned int chunkSize; char format[4]; char subchunk1Id[4]; unsigned int subchunk1Size;
// unsigned short audioFormat; unsigned short numChannels; unsigned int sampleRate; unsigned int byteRate;
// unsigned short blockAlign; unsigned short bitsPerSample; char subchunk2Id[4]; unsigned int subchunk2Size;
printf("names: %s! %s! %s! %s!\n", WAVheader.chunkId, WAVheader.format, WAVheader.subchunk1Id, WAVheader.subchunk2Id);
printf("sizes: %u! %u! %u!\n", WAVheader.chunkSize, WAVheader.subchunk1Size, WAVheader.subchunk2Size);
printf("numChan; sampRate; byteRate; bps: %u! %u! %u! %u!\n", WAVheader.numChannels,
	WAVheader.sampleRate, WAVheader.byteRate, WAVheader.bitsPerSample);
printf("audioFormat; blockAlign: %u! %u!\n", WAVheader.audioFormat, WAVheader.blockAlign);
}

void info_pic () {
printf("picture info:\n");
// 12 B: short signature; int fileSize; short unused1; short unused2; int offsetToData=54;
// 40 B: int thisHeaderSize=40; int width; int height; short colorDimensions; short density; int zipType;
// int massiveLength; int horResolution; int verResolution; int colorsQuantity; int shadesQuantity;
printf("sizes: %i! %i! %i! %i!\n", BMPheader.fileSize, BMPheader.thisHeaderSize, BMPheader.offsetToData, BMPheader.dataSize);
printf("signature: %i!\n", BMPheader.signature);
}

int main (int argc, char *argv[])
{
FILE *audio_file, *output_file, *picture;
char *audioData, *picData;

if (argc < 3) {
	errx(EXIT_FAILURE,"USAGE: %s <audio_file_path> <output_file_path> <pic_path>", argv[0]); //add argv[4]=mode (hide/find)
}
if ((audio_file = fopen (argv[1],"r")) == NULL) {
	errx(EXIT_FAILURE,"Input file cannot be opened");
}
if ((output_file = fopen (argv[2],"w")) == NULL) {
	errx(EXIT_FAILURE,"Output file cannot be opened");
}
if ((picture = fopen (argv[3],"r")) == NULL) {
	errx(EXIT_FAILURE,"Pic file cannot be opened");
}
if (fread(&WAVheader, sizeof(WAVheader), 1, audio_file)<1) {
	errx(EXIT_FAILURE,"Audio could not be read");
}
if (fread(&BMPheader, sizeof(BMPheader), 1, picture)<1) {
	errx(EXIT_FAILURE,"Pic could not be read");
}

info_audio();
audioData = (char*) malloc(WAVheader.subchunk2Size);
fread(audioData, WAVheader.subchunk2Size, 1, audio_file);
fwrite(&WAVheader, sizeof(WAVheader), 1, output_file);
fwrite(audioData, WAVheader.subchunk2Size, 1, output_file);
free(audioData);
fclose(audio_file);
printf("audio written\n");

info_pic();
picData = (char*) malloc(BMPheader.dataSize);
fread(picData, BMPheader.dataSize, 1, picture);
fwrite(&BMPheader, sizeof(BMPheader), 1, output_file);
fwrite(picData, BMPheader.dataSize, 1, output_file);
free(picData);
fclose(picture);
printf("picture written\n");

fclose(output_file);
printf("finished. press any key to exit");
getchar();
return 0;
}
