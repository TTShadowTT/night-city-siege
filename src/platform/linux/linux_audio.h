/* linux_audio.h - Sound on Linux: a small software mixer on PulseAudio / PipeWire, plus MY SONGS decoded with ffmpeg. */
#ifndef NCS_LINUX_AUDIO_H
#define NCS_LINUX_AUDIO_H

#define MY_NAME 48
typedef struct { char name[MY_NAME], file[32]; int map, len; short *pcm; volatile int state, dead; } MySong; /* state: 0 new, 1 decoding, 2 ready, 3 failed */

extern char songDir[PATH_MAX + 16], songIndex[PATH_MAX + 32];

void audioInit(void);
void audioQuit(void);
/* once per frame on the main thread: tells the mixer which soundtrack the game wants */
void audioFrame(void);
void *songDecode(void *arg);
/* tell the game the names and districts */
void songsPush(void);
void songsLoad(void);

#endif
