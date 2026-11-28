/* world_scenery.h - Background scenery pieces: nature, towers, the Earth, the moon and the picture maps. */
#ifndef NCS_WORLD_SCENERY_H
#define NCS_WORLD_SCENERY_H

typedef struct { short x, y; unsigned char layer; float phase; } Beacon;
typedef struct { short x, y; unsigned on, off; unsigned char lit; } Window;
typedef struct { short x, y, w, h, vertical; unsigned col; char txt[8]; } Sign;

/* the picture maps (DAVID AND LUCY, THE MOON, LAUNCH NIGHT, FREEFALL): the platform hands over a 480x270 picture
   for each (game_picture); it is squeezed into 2x2 pixel blocks, snapped to a small palette picked from the
   picture, with dark edges where a bright area meets a darker one */
enum { PIC_ORBIT, PIC_MOON, PIC_LAUNCH, PIC_FREEFALL, PIC_X /* then 7 that are not maps */, PIC_N = PIC_X + 7 };
#define PBW (FW / 2)
#define PBH (FH / 2)

extern Beacon beacons[40];
extern int nBeacons;
extern Window windows[260];
extern int nWindows;
extern Sign signs[24];
extern int nSigns;
extern short starX[60], starY[60];
extern int nStars;
extern unsigned lrng;
extern unsigned *pics[PIC_N];
extern int picsOk[PIC_N];
extern unsigned *pic; /* the DAVID AND LUCY picture, also a MENU BACKGROUND choice, once picOk */
extern int picOk;
extern unsigned char (*picIdx)[PBW * PBH]; /* [PIC_N], in the workspace */
extern short (*smp)[3]; /* [PBW * PBH] block colours while a picture is built, in the workspace */
extern int picBuilt[PIC_N];

int lr(int n);
void addBeacon(int x, int y, int layer);
void drawSignTextTo(const Sign *s, int on);
void natureFar(void);
void natureMid(void);
/* the Arasaka tower: a dark slab with a stepped crown, red light strips and the red emblem near the top */
void arasakaTower(int cx, int top, int bottom, int w, unsigned col, int lights);
/* the Earth seen from the moon: oceans, continents, clouds, a night side dotted with city lights */
void earth(int cx, int cy, int r);
void moonFar(void);
/* the Earth from low orbit, bright and close, wrapped in a cyan glow; drawn only inside the clip box */
void orbitEarth(int cx, int cy, int r, int x0, int y0, int x1, int y1);
int mapPic(int m);
int hasPic(int m);
unsigned picCol(int k, int gx, int gy) /* one block, outlined, dimmed a little so the fight stays readable */;
void buildPicture(int k) /* fills picIdx[k] once */;

#endif
