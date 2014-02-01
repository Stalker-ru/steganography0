#include <stdio.h>
#include <stdlib.h>

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


void info_audio () {
printf(">audio info:\n");
// char chunkId[4]; unsigned int chunkSize; char format[4]; char subchunk1Id[4]; unsigned int subchunk1Size;
// unsigned short audioFormat; unsigned short numChannels; unsigned int sampleRate; unsigned int byteRate;
// unsigned short blockAlign; unsigned short bitsPerSample; char subchunk2Id[4]; unsigned int subchunk2Size;
printf(">names: %s! %s! %s! %s!\n", WAVheader.chunkId, WAVheader.format,
	WAVheader.subchunk1Id, WAVheader.subchunk2Id);
printf(">sizes: %u! %u! %u!\n", WAVheader.chunkSize, WAVheader.subchunk1Size, WAVheader.subchunk2Size);
printf(">numChan; sampRate; byteRate; bps: %u! %u! %u! %u!\n", WAVheader.numChannels,
	WAVheader.sampleRate, WAVheader.byteRate, WAVheader.bitsPerSample);
printf(">audioFormat; blockAlign: %u! %u!\n", WAVheader.audioFormat, WAVheader.blockAlign);
}

void audio_io(FILE* input, FILE* output, char* audioData) {
if (fread(&WAVheader, sizeof(WAVheader), 1, input)<1) {
	errx(EXIT_FAILURE,"Audio could not be read");
}
if(*WAVheader.chunkId!=*"RIFF" || *WAVheader.format!=*"WAVE" ||
	*WAVheader.subchunk1Id!=*"fmt " || *WAVheader.subchunk2Id!=*"data") {
	errx(EXIT_FAILURE,"Incompatible audio format");
}

info_audio();

audioData = (char*) malloc(WAVheader.subchunk2Size);
fread(audioData, WAVheader.subchunk2Size, 1, input);

fwrite(&WAVheader, sizeof(WAVheader), 1, output);
fwrite(audioData, WAVheader.subchunk2Size, 1, output);
free(audioData);
printf("Audio read and written\n\n");
}
