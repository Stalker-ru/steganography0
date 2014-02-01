#ifndef DESCR_WAV
#define DESCR_WAV
#include <stdio.h>
#include <stdlib.h>
struct WAVheader;
void info_audio();
void audio_io(FILE* input, FILE* output, char* audioData);
#endif
