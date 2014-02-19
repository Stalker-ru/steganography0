// hiding2.c
// Steganography: hiding/finding a BMP (picture) file in a WAV (audio) file.
// v2: picture is written into the audioData in every (№%4==0) byte
// USAGE: PROGRAM <audio_in_path> <audio_out_path> <pic_path> <"r"/"w" pic>
// 	  "r" reads picture from audio; "w" writes picture to audio
// Bugakov G., 2014

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
//#include "descr_wav.h"
//#include "descr_bmp.h"

// BMPheader
#pragma pack(push,1)
typedef struct {
	// 14 B
	short signature; //=19778
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
}BMPheader_t; //then data: ColorTable (1024) + Image
#pragma pack(pop)

// WAVheader
typedef struct {
	char chunkId[4]; //="RIFF"
	unsigned int chunkSize; //=filesize-2*4

	char format[4]; //="WAVE"

	char subchunk1Id[4]; //="fmt "
	unsigned int subchunk1Size; //=chunkSize-4-2*4
	unsigned short audioFormat; //=1(pcm), !1(zipping)
	unsigned short numChannels; //=1(mono), 2(stereo), ...
	unsigned int sampleRate; //=8000(kHz), 441000, ...
	unsigned int byteRate; //= sampleRate * numChannels * bitsPerSample/
	unsigned short blockAlign; //= numChannels * bitsPerSample/8 =(bytes per sample)
	unsigned short bitsPerSample; //=8(bit), 16, ...

	char subchunk2Id[4]; //="data"
	unsigned int subchunk2Size; //=numSamples * numChannels * bitsPerSample/8
}WAVheader_t; //then data

typedef struct {
	FILE *audio_in;
	FILE *audio_out;
	FILE *picture;
}FILElist_t;

int cover(FILElist_t FILElist, WAVheader_t WAVheader, BMPheader_t BMPheader) {
	char *audioData, *picData;
	char *BMPheader_p=&BMPheader;
	unsigned long iter, iter2;

	//reading & checking BMPheader
	if (fread(BMPheader_p, sizeof(BMPheader), 1, FILElist.picture)<1) {
		errx(EXIT_FAILURE,"Pic could not be read");
	}
	if(BMPheader.signature!=19778) {
		errx(EXIT_FAILURE,"Incompatible pic format");
	}

	//is hiding safe?
	if ((WAVheader.subchunk2Size/4)<BMPheader.fileSize) {
		errx(EXIT_FAILURE,"Pic is bigger than 1/4 of audiodata. Hiding is unsafe.");
	}

	audioData = (char*) malloc(WAVheader.subchunk2Size);
	fread(audioData, WAVheader.subchunk2Size, 1, FILElist.audio_in);
	picData = (char*) malloc(BMPheader.dataSize);
	fread(picData, BMPheader.dataSize, 1, FILElist.picture);

	//writing to output
	fwrite(&WAVheader, sizeof(WAVheader), 1, FILElist.audio_out);

	for(iter=0, iter2=0; iter<WAVheader.subchunk2Size && iter<4*sizeof(BMPheader); iter++) {
		if(iter%4==0 && iter2<sizeof(BMPheader)) {
			//writing BMPheader instead of every 4th byte of audiodata
			fwrite(BMPheader_p+iter2, 1, 1, FILElist.audio_out);
			iter2++;
			}
		else fwrite(audioData+iter, 1, 1, FILElist.audio_out);
	}

	for(iter2=0; iter<WAVheader.subchunk2Size; iter++) {
		if(iter%4==0 && iter2<(BMPheader.fileSize-sizeof(BMPheader))) {
			//writing picData instead of every 4th byte of audiodata
			fwrite(picData+iter2, 1, 1, FILElist.audio_out);
			iter2++;
			}
		else fwrite(audioData+iter, 1, 1, FILElist.audio_out);
	}

	printf("sizes: WAVdata=%u; BMPfile=%u\n",WAVheader.subchunk2Size,BMPheader.fileSize);
	printf("iters %lu %lu\n",iter,iter2+sizeof(BMPheader));
	free(audioData);
	free(picData);
	return 0;
}

int uncover(FILElist_t FILElist, WAVheader_t WAVheader, BMPheader_t BMPheader) {
	char *audioData, *picData;
	char *BMPheader_p=&BMPheader;
	unsigned long iter, iter2;

	audioData = (char*) malloc(WAVheader.subchunk2Size);

	//reading from input
	for(iter=0, iter2=0; iter<WAVheader.subchunk2Size && iter<4*sizeof(BMPheader); iter++) {
		if(iter%4==0 && iter2<sizeof(BMPheader)) {
			//reading BMPheader instead of every 4th byte of audiodata
			fread(BMPheader_p+iter2, 1, 1, FILElist.audio_in);
			iter2++;
			}
		else fread(audioData+iter, 1, 1, FILElist.audio_in);
	}

	//checking BMPheader
	if(BMPheader.signature!=19778) {
		errx(EXIT_FAILURE,"Incompatible pic format");
	}

	picData = (char*) malloc(BMPheader.dataSize);

	for(iter2=0; iter<WAVheader.subchunk2Size; iter++) {
		if(iter%4==0 && iter2<(BMPheader.fileSize-sizeof(BMPheader))) {
			//reading picData instead of every 4th byte of audiodata
			fread(picData+iter2, 1, 1, FILElist.audio_in);
			iter2++;
			}
		else fread(audioData+iter, 1, 1, FILElist.audio_in);
	}

	//writing to output
	fwrite(&WAVheader, sizeof(WAVheader), 1, FILElist.audio_out);
	fwrite(audioData, iter, 1, FILElist.audio_out);
	fwrite(&BMPheader, sizeof(BMPheader), 1, FILElist.picture);
	fwrite(picData, BMPheader.dataSize, 1, FILElist.picture);

	printf("sizes: WAVdata=%u; BMPfile=%u\n",WAVheader.subchunk2Size,BMPheader.fileSize);
	printf("iters %lu %lu\n",iter,iter2+sizeof(BMPheader));
	free(audioData);
	free(picData);
	return 0;
}



int main (int argc, char *argv[]) {
	FILElist_t FILElist;
	WAVheader_t WAVheader;
	BMPheader_t BMPheader;
	short mode = 1; //if (w)riting pic TO audio, mode==1; if (r)eading pic FROM audio, mode==0

	//checking if input is correct
	if (argc < 5) {
		errx(EXIT_FAILURE,
		"USAGE: %s <audio_in_path> <audio_out_path> <pic_path> <\"r\"/\"w\" pic>", argv[0]);
	}

	if (argv[4][0] == 'r') mode = 0;
	else if (argv[4][0] == 'w') mode = 1;
	else errx(EXIT_FAILURE,"Wrong mode\n\"r\" reads picture from audio; \"w\" writes picture to audio");

	//opening files
	if ((FILElist.audio_in = fopen(argv[1],"r")) == NULL) {
		errx(EXIT_FAILURE,"Input file cannot be opened");
	}
	if ((FILElist.audio_out = fopen(argv[2],"w")) == NULL) {
		errx(EXIT_FAILURE,"Output file cannot be opened");
	}

	if (mode && ((FILElist.picture = fopen(argv[3],"r")) == NULL)) {
		errx(EXIT_FAILURE,"Pic file cannot be opened");
	}
	if (!mode && ((FILElist.picture = fopen(argv[3],"w")) == NULL)) {
		errx(EXIT_FAILURE,"Pic file cannot be opened");
	}

	//reading & checking WAVheader
	if (fread(&WAVheader, sizeof(WAVheader), 1, FILElist.audio_in)<1) {
		errx(EXIT_FAILURE,"Audio could not be read");
	}
	if(*WAVheader.chunkId!=*"RIFF" || *WAVheader.format!=*"WAVE" ||
		*WAVheader.subchunk1Id!=*"fmt " || *WAVheader.subchunk2Id!=*"data") {
		errx(EXIT_FAILURE,"Incompatible audio format");
	}

	//processing
	if(mode) {
		cover(FILElist, WAVheader, BMPheader);
	}
	else {
		uncover(FILElist, WAVheader, BMPheader);
	}

	//finishing
	fclose(FILElist.picture);
	fclose(FILElist.audio_in);
	fclose(FILElist.audio_out);
	printf("Finished. Press a key to exit");
	getchar();
	return 0;
}
