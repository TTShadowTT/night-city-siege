/* gameplay_state.h - The running game: player, enemies, shots, particles, waves and the screen state. */
#ifndef NCS_GAMEPLAY_STATE_H
#define NCS_GAMEPLAY_STATE_H

typedef struct { float x, y, vx, vy, life, dmg; int kind, pierce, lastHit, weapon; } Shot; /* kind: 0 bullet, 1 rocket, 2 grenade, 3 gravity orb */
typedef struct { float x, y, vx, vy, life; int kind; } EShot;            /* kind: 0 bullet, 1 bomb, 2 missile */
typedef struct { float x, y, vx, vy, life, max, grav; unsigned c; int sz, bounce, blood; } Part;
typedef struct { float x, y, r, life, max; } Boom;
typedef struct { float x0, y0, x1, y1, life; } Beam;
typedef struct { float x, y, vy, life; int type, landed, chute, plat; } Pickup; /* type 0 = medkit */
typedef struct { float x, y, life; char s[28]; unsigned c; } Floater;
typedef struct { float x, y0, y1, life; } RopeFx;
typedef struct {
    int alive, type, state, elite, hunt, onGround, plat, rope, facing, mega; /* mega: 1 + which big boss */
    float x, y, vx, vy, hw, hh, hp, maxhp, speed, timer, timer2, timer3, flash, phase, anim, dropT, jumpCd, ax, ay, anchorY, targetY, dmgAcc, dmgT; /* dmgAcc / dmgT: hits adding up into one DAMAGE NUMBER */
} Enemy;

/* W_SMG is the minigun and W_LASER the railgun */
enum { W_PISTOL, W_SMG, W_SHOTGUN, W_LASER, W_GRENADE, W_ROCKET, W_GRAVITY, W_COUNT };
typedef struct {
    const char *name;
    float cd, dmg, spread, speed, life, reload; /* cd = seconds between shots */
    int pellets, mag, gunLen, thick, snd;       /* mag = rounds before a reload; reloads are unlimited */
    unsigned col;
    const Spr *icon;
} Weapon;

enum { ST_MENU, ST_PLAY, ST_PAUSE, ST_OVER, ST_MAPSEL, ST_SKINS, ST_QUESTS, ST_KEYS, ST_QUIT, ST_PERK, ST_OPTIONS, ST_MENUBG, ST_UISTYLE, ST_UPGRADES, ST_PETEDIT, ST_TITLE, ST_SONGS, ST_ACH, ST_WIPE, ST_EXTRA };

/* upgrades: after every cleared wave you pick one of three; they last for the rest of the run */
enum { PK_HP, PK_DMG, PK_RELOAD, PK_MAG, PK_RATE, PK_VAMP, PK_JUMP, PK_DASH, PK_LUCK, PK_BLAST, PK_PIERCE, PK_ARMOR, PK_SPEED, PK_COUNT };
typedef struct { const char *name, *line1, *line2; int maxStack; unsigned col; } Perk;
#define PERK_DEFAULT 10 /* upgrade cards stop after this many in one run, unless OPTIONS > UPGRADES says otherwise */
#define PERK_TOTAL 38   /* every upgrade at its max stack: the most one run can ever take */
/* upgrades that keep coming after their max level when upgrades are infinite (the others would break the game) */
#define PERK_ENDLESS (1 << PK_HP | 1 << PK_DMG | 1 << PK_MAG | 1 << PK_RATE | 1 << PK_VAMP | 1 << PK_LUCK | 1 << PK_BLAST | 1 << PK_ARMOR | 1 << PK_SPEED)
enum { MODE_CAMPAIGN, MODE_ENDLESS };
enum { WK_NORMAL, WK_HORDE, WK_SWARM, WK_ELITE, WK_BOSS };
#define MAXHOLE 4
typedef struct { float x, y, life; int kills; } Hole; /* gravity gun black holes */
#define HOLE_TIME 1.7f
#define HOLE_PULL 72.0f
#define BG_PICTURE (-3)
#define BG_RANDOM (-2)
#define BG_LAST (-1)
/* MENU TITLE: bought once in OPTIONS, then you type the two lines of the main menu title and pick a style for them */
#define TITLE_COST 2500
#define TITLE1_MAX 16
#define TITLE2_MAX 24
/* a style = a letter shape (fx flags) + a colour scheme; append only, the save stores the index */
enum { FX_BOLD = 1, FX_ITALIC = 2, FX_TALL = 4, FX_WAVE = 8, FX_BUBBLE = 16, FX_DOTS = 32, FX_SHAKY = 64, FX_STENCIL = 128, FX_WIDE = 256, FX_HOLLOW = 512, FX_SLASH = 1024, FX_LABEL = 2048 }; /* LABEL: line 2 sits in a box */
enum { SC_NEON, SC_CHROME, SC_FIRE, SC_RAINBOW, SC_GOLD, SC_TOXIC, SC_GLITCH, SC_SYNTH, SC_ICE, SC_CLEAN, SC_MATRIX, SC_BLOOD, SC_VAPOR, SC_PINK,
       SC_EDGE, SC_PSYCHO, SC_SANDY, SC_NET, SC_ARASAKA, SC_SAMURAI, SC_HOLO, SC_MAXTAC, SC_BD, SC_KABUKI, SC_AFTER, SC_STREET, SC_CP, SC_CPRED, SC_CPCYAN };
typedef struct { const char *name; int fx, sc; } TitleStyle;
#define TS_COUNT 42 /* entries in TSTY */
#define TS_PAGE 10
#define SEARCH_MAX 18
typedef struct { const char *name; unsigned fill, fillHi, edge, edgeHi, txt, hi, dim, titleA, titleB; int deco; } UiStyle;
#define CURSOR_COUNT 34 /* saved in 6 bits, so at most 64 */
#define CUR_SWORD 19

extern const Weapon WEAP[W_COUNT];
extern const Perk PERKS[PK_COUNT];
extern int perkLimitOn, perkLimit;
extern int perk[PK_COUNT], perkChoice[3], perksTaken, perkPending;
extern float perkPow[PK_COUNT];
extern float perkOpenT; /* when the cards appeared, so a stray click or weapon key doesn't pick one instantly */
extern int focusBtn; /* highlighted button / card on any menu screen */
extern Shot shots[MAXS];
extern EShot eshots[MAXES];
extern Part parts[MAXP];
extern Boom booms[MAXBOOM];
extern Beam beams[MAXBEAM];
extern Pickup pu[MAXPU];
extern Floater fl[MAXF];
extern Enemy en[MAXE];
extern RopeFx ropeFx[MAXRFX];
extern Hole holes[MAXHOLE];
extern int state, mode;
extern float gt;
extern float px, py, pvx, pvy, aim, inv, dashT, dashCd, fireCd, muzzleT, runAnim, dropT, coyote;
extern int hp, facing, curW, mag[W_COUNT], onGround, onPlat, ropeIdx, jumps, airDash, dashDir;
extern float reloadT, reloadMax, hurtFlash;
extern int maxHp, selfHit;
extern int score, newHi, area, startMap, waveIdx, waveKind, toSpawn, combo, bossIdx, runCoins;
extern int runWave; /* waves fought this run, across districts: every 20th brings a big boss */
extern int totalKills, bossKills, deaths, stallKills, runKills, bestCombo, streakN, waveHurt;
extern float spawnCd, bannerT, interT, comboT, shake, overT, stallT, supplyT, streakT, hitStop, coinFrac;
extern char callout[24];
extern float calloutT;
extern unsigned calloutCol;
extern int menuMusMap; /* the district behind the menu, for the soundtrack */
extern int musicOn, perksOn, shakeOn;
extern int sfxOn; /* sound effects on / off (settings bit 26 = OFF) */ /* saved as one bit field: 1 = music, 2 = upgrade cards OFF, 4 = shake OFF */
extern int menuBg; /* the district behind the main menu: one map, the last one played, a random one, or the picture */
extern int picUndone; /* one build made the picture the default; saves from it go back to LAST PLAYED once (settings bit 2048) */
extern const TitleStyle TSTY[];
extern int tsPage; /* the page of style cards on the MENU TITLE screen */
extern int songDel; /* MY SONGS: the song whose X was clicked once */
extern char songSearch[SEARCH_MAX + 1]; /* SOUNDTRACK: only songs with this in their name are listed */
extern int searchEdit; /* 1 while typing in the search box */
extern int sndLblGame, sndLblMine, sndGameAny, sndMineAny; /* where the list put its section labels (for drawing) */
extern int titleOwned, titleStyle, titleEdit, titleFresh; /* titleEdit: the line being typed (1 or 2), 0 = none; titleFresh: the next letter replaces the old text */
extern char title1[TITLE1_MAX + 1], title2[TITLE2_MAX + 1];
extern int uiStyle; /* UI STYLE preset: free to pick, saved in the settings bits from 16 up */
extern UiStyle ui; /* the current UI STYLE's colors, refreshed every frame */
extern int devMode; /* developer mode: everything counts as unlocked while it is on (settings bit 8) */
extern int optFromPause, sliderDrag, skinTab; /* OPTIONS opened from the pause screen; slider being dragged; skins screen tab */
extern int cursorSkin; /* the mouse cursor look, picked on the CURSORS tab of the skins screen (all free) */
extern float musicSent;
extern int areaClear;
extern char banner[48], banner2[48];
extern int mx, my, mdown, dashReq, held[4], jumpPress, upPress, downPress, jumpHeld, wantsQuit;
extern int fullOn, fullReq; /* fullscreen: fullOn is reported by the platform, fullReq asks it to toggle */
extern int bot, god;
extern int dmgNumsOn; /* DAMAGE NUMBERS over the enemies you hit (OPTIONS) */
extern int hardOn, hardUnlocked; /* HARD MODE: unlocked by clearing the last campaign district, switched in OPTIONS */
#define HARD_HP 1.6f /* on HARD enemies have this much more health, hit 50% harder and every coin counts double */
extern float bossCardT; /* the boss intro card that slams in when a boss wave starts */
extern char bossCardName[32], bossCardSub[48], bossCardLine[48];
extern unsigned bossCardCol;
#define BOSS_CARD_T 2.8f
extern float botWeaponT;

int perkCap(void);
float perkWeight(int nTaken);
int level(void);
/* weapon numbers after upgrades */
int magSize(int w);
float dmgMul(void);
float blastMul(void);
float dashMax(void);

#endif
