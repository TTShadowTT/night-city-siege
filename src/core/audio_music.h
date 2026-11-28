/* audio_music.h - The soundtracks (8-bar synth loops, one per kind of district) and MY SONGS. */
#ifndef NCS_AUDIO_MUSIC_H
#define NCS_AUDIO_MUSIC_H

typedef struct { const char *name; int step, drum, arp, lead, bass; float pad; signed char root[4], chord[4][3], mel[8][4]; } Track;

/* MY SONGS: the player's own music files. The platform keeps the files and plays them; the game only knows their
   names and which district each one is tied to (-1 = none), and asks the platform to add, remove or re-tie one. */
#define MY_MAX 8
#define MY_NAME 48

extern const Track TRACKS[MUS_TRACKS];
extern const signed char MAP_TRACK[];
extern int musReady[MUS_TRACKS], musTrack;
extern int myCount;
extern char myNames[MY_MAX][MY_NAME];
extern signed char myMap[MY_MAX];
extern int musicMode, musFor; /* SOUNDTRACK: 0 = BY MAP, 1 = RANDOM, 2 + n = always track n (MY SONGS after the built-in ones) */
extern int sfxVolPct, musicVolPct;

const char *trackName(int t);
void sfx(int id, float vol);

#endif
