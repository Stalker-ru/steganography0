// hiding.c
// Steganography: hiding/finding a BMP (picture) file in a WAV (audio) file.
// v1: picture is written to the end of audio, while audio.header.size stays the same
// USAGE: PROGRAM <audio_in_path> <audio_out_path> <pic_path> <"r"/"w" pic>
// 	  "r" reads picture from audio; "w" writes picture to audio
// Bugakov G., 2014

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "descr_wav.h"
#include "descr_bmp.h"

int main (int argc, char *argv[])
{
FILE *audio_in, *audio_out, *picture;
char *audioData, *picData;
short mode=1;
//if (w)riting pic TO audio, mode==1; if (r)eading pic FROM audio, mode==0

//checking if input is correct
if (argc < 5) {
	errx(EXIT_FAILURE,"USAGE: %s <audio_in_path> <audio_out_path> <pic_path> <\"r\"/\"w\" pic>",argv[0]);	
}

if (argv[4][0] == 'r') mode = 0;
else if (argv[4][0] == 'w') mode = 1;
else errx(EXIT_FAILURE,"Wrong mode\n\"r\" reads picture from audio; \"w\" writes picture to audio");

//checking files for opening
if ((audio_in = fopen (argv[1],"r")) == NULL) {
	errx(EXIT_FAILURE,"Input file cannot be opened");
}
if ((audio_out = fopen (argv[2],"w")) == NULL) {
	errx(EXIT_FAILURE,"Output file cannot be opened");
}

if (mode && ((picture = fopen (argv[3],"r")) == NULL)) {
	errx(EXIT_FAILURE,"Pic file cannot be opened");
}
if (!mode && ((picture = fopen (argv[3],"w")) == NULL)) {
	errx(EXIT_FAILURE,"Pic file cannot be opened");
}

//processing data
audio_io(audio_in, audio_out, audioData);
if(mode) {
pic_io(picture, audio_out, picData, mode);
}
else {
pic_io(audio_in, picture, picData, mode);
}

//finishing
fclose(picture);
fclose(audio_in);
fclose(audio_out);
printf("Finished. Press a key to exit");
getchar();
return 0;
}
