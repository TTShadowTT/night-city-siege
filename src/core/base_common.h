/* base_common.h - Shared constants, random numbers, colour maths and a tiny string formatter. */
#ifndef NCS_BASE_COMMON_H
#define NCS_BASE_COMMON_H

#define FW GAME_FB_W
#define FH GAME_FB_H
#define FARW (FW + 32) /* parallax layers are wider than the screen */
#define MIDW (FW + 64)
#define GROUND_Y 250
#define GRAV 760.0f
#define DT (1.0f / 60.0f)
#define PI 3.14159265f
#define SND_POOL 150000
#define MUS_TRACKS 10                 /* soundtracks: each an 8-bar loop, made the first time it's needed */
#define MUS_MAXSTEP 4000              /* samples per 16th note in the slowest track (84 BPM) */
#define MUS_MAXLEN (MUS_MAXSTEP * 16 * 8)

#define MAXS 320
#define MAXES 320
#define MAXE 96
#define MAXP 1200
#define MAXPU 16
#define MAXF 64
#define MAXBOOM 24
#define MAXBEAM 8
#define MAXRFX 16
#define PHW 4.0f /* player half width / half height */
#define PHH 9.0f
#define MAX_HP 100
#define MEDKIT_HEAL 35

#define C(r, g, b) (0xFF000000u | (unsigned)(r) | ((unsigned)(g) << 8) | ((unsigned)(b) << 16))
#define C_OUT   C(16, 14, 22)
#define C_WHITE C(255, 255, 255)

extern unsigned rng;
extern const int bayer[4][4];
extern unsigned *fb, *farL, *midL, *foreL;
extern short *sndPool, *musPool;

void sfmt(char *out, int cap, const char *f, ...) __attribute__((format(printf, 3, 4)));

static inline unsigned xrand(void) { rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5; return rng; }

static inline float frand(void) { return (xrand() >> 8) * (1.0f / 16777216.0f); }

static inline int irand(int n) { return (int)(xrand() % (unsigned)n); }

static inline float frange(float a, float b) { return a + (b - a) * frand(); }

static inline float clampf(float v, float a, float b) { return v < a ? a : v > b ? b : v; }

static inline int clampi(int v, int a, int b) { return v < a ? a : v > b ? b : v; }

static inline float lenf(float x, float y) { return sqrtf(x * x + y * y); }

static inline unsigned hash3(int a, int b, int c)
{
    unsigned h = (unsigned)a * 374761393u + (unsigned)b * 668265263u + (unsigned)c * 2246822519u;
    h = (h ^ (h >> 13)) * 1274126177u;
    return h ^ (h >> 16);
}

static inline unsigned mixc(unsigned a, unsigned b, float t)
{
    int ar = a & 255, ag = (a >> 8) & 255, ab = (a >> 16) & 255;
    int br = b & 255, bg_ = (b >> 8) & 255, bb = (b >> 16) & 255;
    return C(ar + (int)((br - ar) * t), ag + (int)((bg_ - ag) * t), ab + (int)((bb - ab) * t));
}

static inline unsigned scalec(unsigned a, int k256)
{
    int r = ((a & 255) * k256) >> 8, g = (((a >> 8) & 255) * k256) >> 8, b = (((a >> 16) & 255) * k256) >> 8;
    return C(r > 255 ? 255 : r, g > 255 ? 255 : g, b > 255 ? 255 : b);
}

#endif
