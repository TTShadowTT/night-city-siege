/* win32_audio.h - Sound on Windows: a small software mixer on waveOut, plus MY SONGS decoded with Media Foundation. */
#ifndef NCS_WIN32_AUDIO_H
#define NCS_WIN32_AUDIO_H

#define MY_NAME 48
typedef struct { char name[MY_NAME], file[32]; int map, len; short *pcm; volatile LONG state, dead; } MySong; /* state: 0 new, 1 decoding, 2 ready, 3 failed */

extern HWAVEOUT hwo;
extern int audioOk;
extern char songDir[MAX_PATH], songIndex[MAX_PATH + 16];

void audioInit(void);
DWORD WINAPI songDecode(LPVOID arg);
/* tell the game the names and districts */
void songsPush(void);
void songsLoad(void);
void audioPump(void);

#endif
