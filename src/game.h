/*
 * NIGHT CITY SIEGE - interface between the game core (src/core) and a platform
 * layer (platform/win32 for the native exe, platform/web for the browser).
 *
 * The core renders everything itself into a 480x270 pixel buffer; a platform
 * only scales that buffer up (nearest neighbour), forwards input and plays
 * the sound effects the core synthesises.
 */
#ifndef SLOP_GAME_H
#define SLOP_GAME_H

#define GAME_FB_W 480
#define GAME_FB_H 270
#define SND_RATE 22050

/* pixels are 32-bit with bytes R,G,B,A in memory (canvas ImageData order) */

enum {
    K_LEFT, K_RIGHT, K_UP, K_DOWN,             /* held: A D W S / arrows */
    K_JUMP, K_DASH, K_PAUSE, K_ENTER, K_ESC, K_RETRY, K_MENU,
    K_PREV_WEAPON, K_NEXT_WEAPON,
    K_WEAPON1, K_WEAPON2, K_WEAPON3, K_WEAPON4, K_WEAPON5, K_WEAPON6, K_WEAPON7,
    K_MUSIC,                                   /* M: music on / off */
    K_X
};

enum {
    STAT_STATE, STAT_WAVE, STAT_SCORE, STAT_HP, STAT_ENEMIES, STAT_KILLS, STAT_BOSSES, STAT_DEATHS,
    STAT_PX, STAT_PY, STAT_SHOTS, STAT_ESHOTS, STAT_AREA, STAT_WEAPON, STAT_MAP, STAT_ON_ROPE, STAT_ON_GROUND,
    STAT_MAG, STAT_RELOADING, STAT_COINS, STAT_MODE, STAT_FOCUS, STAT_MAPS, STAT_SKINS_OWNED, STAT_SKIN, STAT_QUESTS_DONE,
    STAT_PERKS, STAT_MAX_HP, STAT_MUSIC, STAT_DAILY_DONE, STAT_DAILY_POOL
};

/* provided by the platform */
void plat_sound(int id, float vol);
void plat_music(float vol);              /* loop the soundtrack (game_music_data) at this volume; 0 = silent */
int plat_day(void);                      /* today's local date as a day number (days since 1970) */
int plat_daysec(void);                   /* seconds since local midnight */
int plat_load_data(char *buf, int cap);  /* the saved progress text (coins, unlocks, quests); returns its length */
void plat_save_data(const char *text);
int plat_song_add(void);                 /* MY SONGS: let the player pick music files (0 = can't here) */
void plat_song_remove(int i);            /* forget song i */
void plat_song_map(int i, int map);      /* song i now plays on this district (-1 = none) */
void plat_open_url(const char *url);     /* open this web page in the browser */

/* provided by the game */
unsigned game_workspace_size(void);        /* bytes of scratch memory game_init needs */
void game_init(unsigned seed, void *workspace);
void game_tick(void);                      /* one fixed 1/60 s step */
void game_render(void);                    /* draws the frame into the pixel buffer */
const unsigned *game_framebuffer(void);
#define GAME_PICTURES 11                   /* optional pictures: 0 DAVID AND LUCY (also a MENU BACKGROUND), 1 THE MOON, 2 LAUNCH NIGHT, 3 FREEFALL, 4-10 extra ones */
unsigned *game_picture(int k);             /* the platform fills these GAME_FB_W x GAME_FB_H pixels for picture k... */
void game_picture_ready(int k);            /* ...then calls this, and that map is drawn from the picture */
void game_mouse_move(int x, int y);        /* in pixel-buffer coordinates */
void game_mouse_button(int button, int down); /* 0 = left, 1 = right */
void game_wheel(int dir);
void game_key(int key, int down);
void game_char(int ch);                    /* a typed character (8 = backspace), for typing the MENU TITLE */
/* on each key press: the keys down (1 CTRL 2 ALT 4 SHIFT, up to two other keys, lowest first, 0 = none) and the key;
   1 = the developer hotkey (send K_X). plat 1 Windows scan codes, 2 X11 key codes, 3 browser key numbers */
int game_hotkey(int plat, int mods, int held1, int held2, int key);
int game_typing(void);                     /* 1 while the player is typing: send letters to game_char, not game_key */
int game_music_track(void);                /* which soundtrack game_music_data holds; when it changes, start the new one */
                                           /* tracks from 10 on are MY SONGS (the player's own files), which the platform plays itself */
char *game_song_names(void);               /* MY SONGS: the platform writes up to 8 names here (48 bytes each)... */
signed char *game_song_maps(void);         /* ...and the district each is tied to (-1 = none)... */
void game_songs_set(int n);                /* ...then says how many there are */
void game_focus_lost(void);
int game_wants_quit(void);                 /* 1 once after the player picks QUIT GAME */
int game_wants_fullscreen(void);           /* 1 once after the player toggles FULLSCREEN in OPTIONS */
void game_set_fullscreen(int on);          /* the platform reports whether the window is fullscreen */
int game_sound_count(void);
const short *game_sound_data(int id);
int game_sound_len(int id);
const short *game_music_data(void);        /* the looping soundtrack, SND_RATE mono */
int game_music_len(void);
/* a game controller, read by the platform once per frame (Xbox layout: on a PlayStation pad A = cross, B = circle,
   X = square, Y = triangle). Sticks run -1..1 with up negative, triggers 0..1; buttons = 0 when none is connected. */
enum { PAD_A = 1, PAD_B = 2, PAD_X = 4, PAD_Y = 8, PAD_LB = 16, PAD_RB = 32, PAD_BACK = 64, PAD_START = 128,
       PAD_UP = 256, PAD_DOWN = 512, PAD_LEFT = 1024, PAD_RIGHT = 2048 };
void game_pad(unsigned buttons, float lx, float ly, float rx, float ry, float lt, float rt);
void game_set_bot(int bot, int god);       /* autopilot for automated testing */
int game_stat(int which);

#endif
