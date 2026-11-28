/* world_scenery.c - Background scenery pieces: nature, towers, the Earth, the moon and the picture maps. */
#include "core.h"

Beacon beacons[40];
int nBeacons;
Window windows[260];
int nWindows;
Sign signs[24];
int nSigns;
short starX[60], starY[60];
int nStars;

unsigned lrng;
int lr(int n) { lrng ^= lrng << 13; lrng ^= lrng >> 17; lrng ^= lrng << 5; return (int)(lrng % (unsigned)n); }

void addBeacon(int x, int y, int layer)
{
    if (nBeacons < 40) { beacons[nBeacons].x = (short)x; beacons[nBeacons].y = (short)y; beacons[nBeacons].layer = (unsigned char)layer; beacons[nBeacons].phase = lr(100) / 70.f; nBeacons++; }
}

void drawSignTextTo(const Sign *s, int on)
{
    unsigned c = on ? mixc(s->col, C_WHITE, 0.3f) : scalec(s->col, 60);
    if (s->vertical) {
        int n = (int)strlen(s->txt);
        for (int i = 0; i < n; i++) { char ch[2] = { s->txt[i], 0 }; drawText(s->x + 2, s->y + 2 + i * 6, ch, c, 1); }
    } else {
        drawText(s->x + 2, s->y + 2, s->txt, c, 1);
    }
}

/* ---- outdoor maps: ridges, peaks, clouds and trees ---- */
static int ridgeY(int x, float base, float amp, unsigned seed)
{
    float p1 = (seed % 100) / 16.f, p2 = (seed / 100 % 100) / 16.f;
    return (int)(base - amp * (0.55f + 0.25f * sinf(x * 0.021f + p1) + 0.15f * sinf(x * 0.057f + p2) + 0.05f * sinf(x * 0.13f + p1 * 2)));
}

/* a mountain range filled down to the bottom; optional single big peak and snow above snowLine */
static void ridgeLayer(int w, float base, float amp, unsigned seed, unsigned col, int snowLine, int peakX, int peakTop)
{
    for (int x = 0; x < w; x++) {
        int y = ridgeY(x, base, amp, seed);
        if (peakX >= 0) {
            int jag = (int)(hash3(x / 3, (int)seed, 11) % 5);
            int py = peakTop + (int)(fabsf((float)(x - peakX)) * 1.1f) + jag;
            if (py < y) y = py;
        }
        int right = peakX >= 0 ? x > peakX : ridgeY(x + 2, base, amp, seed) > y; /* slope facing away from the light */
        for (int yy = y; yy < FH; yy++) {
            unsigned c = right ? scalec(col, 175) : col;
            if (y < snowLine && yy - y < (snowLine - y) * 0.55f + 2 + (int)(hash3(x, 3, (int)seed) % 4))
                c = right ? C(176, 186, 214) : C(236, 240, 252);
            pset(x, yy, c);
        }
    }
}

static void cloud(int cx, int cy, int w, unsigned top, unsigned under)
{
    for (int k = 0; k < w / 6; k++) {
        int r = 5 + lr(w / 5 + 1), x = cx - w / 2 + lr(w), y = cy - lr(r / 2 + 1);
        fillCircle(x, y + 2, r, under);
        fillCircle(x, y, r - 1, top);
    }
}

static void natureHaze(int from, float strength)
{
    for (int y = from; y < FH; y++) {
        float a = clampf((y - from) / (float)(FH - from), 0, 1) * strength;
        for (int x = 0; x < FARW; x++)
            if (a * 16 > bayer[y & 3][x & 3] * 0.5f) pmix(x, y, D->haze, a);
    }
}

void natureFar(void)
{
    switch (curMap) {
    case D_FOREST:
        ridgeLayer(FARW, 170, 70, 1234, D->farCol, 0, -1, 0);
        natureHaze(150, 0.35f);
        ridgeLayer(FARW, 215, 40, 777, scalec(D->farCol, 70), 0, -1, 0);
        break;
    case D_MEADOW:
        for (int k = 0; k < 7; k++) cloud(lr(FARW), 20 + lr(70), 30 + lr(40), C(255, 255, 255), C(200, 214, 236));
        ridgeLayer(FARW, 200, 60, 4321, D->farCol, 0, -1, 0);
        natureHaze(160, 0.3f);
        ridgeLayer(FARW, 228, 34, 99, C(96, 170, 90), 0, -1, 0);
        break;
    case D_STORM:
        for (int k = 0; k < 14; k++) cloud(lr(FARW), 5 + lr(50), 50 + lr(60), C(52, 58, 68), C(30, 34, 42));
        ridgeLayer(FARW, 185, 80, 2468, D->farCol, 0, -1, 0);
        natureHaze(140, 0.3f);
        ridgeLayer(FARW, 218, 40, 1357, scalec(D->farCol, 70), 0, -1, 0);
        break;
    default: /* the epic mountain: a far snowy range, then one huge peak right behind the action */
        ridgeLayer(FARW, 165, 90, 8642, mixc(D->farCol, D->haze, 0.35f), 125, -1, 0);
        natureHaze(120, 0.25f);
        ridgeLayer(FARW, 225, 45, 5555, D->farCol, 150, 300, 22);
        natureHaze(200, 0.3f);
        break;
    }
}

static void pine(int x, int ground, int h, unsigned col, int snow)
{
    int top = ground - h;
    fillRect(x - 1, ground - 12, 3, 12, C(40, 28, 22));
    for (int y = top; y < ground - 8; y++) {
        int t = y - top, half = t * 3 / 10 + ((t % 9) < 4 ? 2 : 0);
        unsigned c = (t % 9) < 2 ? scalec(col, 135) : col;
        if (snow && (t % 9) < 2) c = C(232, 238, 250);
        fillRect(x - half, y, half * 2 + 1, 1, c);
        pset(x + half, y, scalec(c, 160)); /* shaded right edge */
    }
}

void natureMid(void)
{
    switch (curMap) {
    case D_FOREST:
        for (int x = -10 + lr(10); x < MIDW; x += 10 + lr(16)) pine(x, GROUND_Y + 10, 70 + lr(120), scalec(D->midCol, 170 + lr(120)), 0);
        break;
    case D_MEADOW:
        for (int x = 10 + lr(30); x < MIDW; x += 60 + lr(70)) { /* leafy trees */
            int th = 28 + lr(24), cy = GROUND_Y - th;
            fillRect(x - 2, cy, 4, th + 10, C(96, 64, 40));
            for (int k = 0; k < 7; k++) {
                int r = 9 + lr(8), ox_ = lr(28) - 14, oy_ = lr(16) - 10;
                fillCircle(x + ox_, cy + oy_ + 2, r, C(40, 110, 50));
                fillCircle(x + ox_ - 1, cy + oy_, r - 2, lr(3) ? C(70, 160, 70) : C(100, 190, 80));
            }
        }
        for (int x = 0; x < MIDW; x += 12) fillRect(x, GROUND_Y - 9, 2, 12, C(200, 170, 120)); /* fence */
        fillRect(0, GROUND_Y - 7, MIDW, 1, C(220, 190, 140));
        fillRect(0, GROUND_Y - 3, MIDW, 1, C(220, 190, 140));
        break;
    case D_STORM:
        for (int x = lr(40); x < MIDW; x += 40 + lr(60)) { /* dead trees and boulders */
            int h = 50 + lr(80), top = GROUND_Y - h;
            unsigned c = C(30, 30, 34);
            fillRect(x - 1, top, 3, h + 10, c);
            for (int k = 0; k < 4; k++) {
                int by = top + 6 + lr(h / 2), dir = lr(2) ? 1 : -1, len = 8 + lr(14);
                line(x, by, x + dir * len, by - len / 2, c);
                line(x + dir * len, by - len / 2, x + dir * (len + 4), by - len / 2 - 5, c);
            }
            int bx = x + 20 + lr(20), br = 6 + lr(8);
            fillCircle(bx, GROUND_Y + 4, br, C(44, 46, 52));
            fillCircle(bx - 2, GROUND_Y + 2, br - 3, C(60, 62, 70));
        }
        break;
    default: /* snowy pines on the slopes, a couple of big rocks */
        for (int x = lr(20); x < MIDW; x += 22 + lr(40)) pine(x, GROUND_Y + 10, 50 + lr(80), scalec(D->midCol, 200 + lr(80)), 1);
        for (int k = 0; k < 3; k++) {
            int bx = lr(MIDW), br = 10 + lr(10);
            fillCircle(bx, GROUND_Y + 6, br, C(70, 66, 90));
            fillRect(bx - br + 2, GROUND_Y + 6 - br, br * 2 - 4, 3, C(236, 240, 252));
        }
        break;
    }
}

/* the Arasaka tower: a dark slab with a stepped crown, red light strips and the red emblem near the top */
void arasakaTower(int cx, int top, int bottom, int w, unsigned col, int lights)
{
    int crown = top + w, x0 = cx - w / 2;
    fillRect(cx - 1, top - w / 2, 3, w / 2, col);                                 /* spire */
    fillRect(cx - w / 3, top, w * 2 / 3, crown - top, col);                      /* crown */
    fillRect(cx - w * 2 / 5, top + w / 3, w * 4 / 5, crown - top - w / 3, col);
    fillRect(x0, crown, w, bottom - crown, col);                                 /* shaft */
    fillRect(x0 + w - 3, crown, 3, bottom - crown, scalec(col, 70));             /* shaded side */
    for (int y = crown + 2; y < bottom; y++) {                                   /* red light strips down both edges */
        unsigned r = (y / 3) & 1 ? C(255, 40, 60) : C(170, 20, 36);
        pset(x0 + 1, y, r); pset(x0 + w - 2, y, r);
        if (w >= 24) { pset(cx - w / 4, y, scalec(r, 120)); pset(cx + w / 4, y, scalec(r, 120)); }
    }
    for (int y = crown + 6; lights && y < bottom; y += 4)                        /* a few lit office floors */
        for (int x = x0 + 3; x < x0 + w - 3; x += 2)
            if (hash3(x, y, cx) % 9 == 0) pset(x, y, C(255, 190, 180));
    int er = w / 5 < 2 ? 2 : w / 5, ey = top + w / 2 + 2;                          /* the emblem */
    fillCircle(cx, ey, er + 1, C(30, 6, 10));
    fillCircle(cx, ey, er, C(230, 30, 46));
    if (er >= 3) { fillRect(cx - er / 2, ey - 1, er, 2, C(30, 6, 10)); fillRect(cx - 1, ey - er / 2, 2, er, C(30, 6, 10)); }
}

/* the Earth seen from the moon: oceans, continents, clouds, a night side dotted with city lights */
void earth(int cx, int cy, int r)
{
    int halo = r / 14 + 4;
    for (int j = -r - halo; j <= r + halo; j++)
        for (int i = -r - halo; i <= r + halo; i++) {
            float d = sqrtf((float)(i * i + j * j));
            float lit = clampf((0.35f * i + 0.6f * j) / (d + 1) + 0.3f, 0, 1); /* the sun is low and behind us, lower right */
            if (d > r) { /* glowing atmosphere, brightest on the lit side */
                float g = (1 - (d - r) / halo) * (0.25f + 0.5f * lit);
                if (g > 0 && g * 16 > bayer[(cy + j) & 3][(cx + i) & 3] * 0.5f) pmix(cx + i, cy + j, mixc(C(60, 120, 255), C(200, 240, 255), lit), g);
                continue;
            }
            float nx = i / (float)r, ny = j / (float)r, nz = sqrtf(fmaxf(0, 1 - nx * nx - ny * ny));
            float sx = nx * (1.6f - 0.6f * nz), sy = ny * (1.6f - 0.6f * nz); /* squeeze features toward the rim */
            float shade = 0.5f * nx + 0.7f * ny + 0.45f * nz - 0.12f;
            float v = sinf(sx * 3.1f + 0.7f) + sinf(sy * 3.7f - sx * 1.9f) + 0.6f * sinf((sx - sy) * 6.3f);
            /* swirling storm clouds: warped bands plus finer streaks */
            float wx = sx * 9 + 1.8f * sinf(sy * 6 + sx * 2), wy = sy * 9 + 1.8f * sinf(sx * 5 - sy * 3);
            float cl = sinf(wx * 1.3f + sinf(wy * 1.7f) * 2.2f) * sinf(wy * 1.5f + sinf(wx * 1.3f) * 1.8f)
                     + 0.45f * sinf(wx * 3.7f + wy * 2.9f + sinf(wy * 2) * 1.5f);
            float cloud = clampf((cl - 0.15f) * 1.8f, 0, 1);
            unsigned c = v > 1.0f ? (v > 1.7f ? C(196, 164, 104) : C(140, 118, 70)) : C(12, 40, 96);
            if (cloud * 16 > bayer[(cy + j) & 3][(cx + i) & 3]) c = mixc(C(200, 205, 215), C(250, 250, 252), cloud);
            if (shade < 0.05f) {
                c = scalec(c, 30);
                if (v > 1.0f && cloud < 0.3f && hash3(i, j, 77) % 7 == 0) c = C(255, 200, 100); /* cities at night */
            } else c = scalec(c, (int)(70 + 186 * fminf(1, shade)));
            if (d > r - 2) c = mixc(c, C(140, 200, 255), 0.5f * (0.4f + lit)); /* bright rim */
            pset(cx + i, cy + j, c);
        }
}

static void crater(int cx, int cy, int rx, unsigned ground)
{
    int ry = rx / 3 + 1;
    for (int j = -ry; j <= ry; j++)
        for (int i = -rx; i <= rx; i++) {
            float e = (i * i) / (float)(rx * rx) + (j * j) / (float)(ry * ry);
            if (e > 1) continue;
            pset(cx + i, cy + j, e > 0.6f ? (j < 0 ? scalec(ground, 150) : scalec(ground, 70)) : scalec(ground, j < 0 ? 60 : 85));
        }
}

void moonFar(void)
{
    earth(FARW / 2, 92, 150); /* the whole sky is the Earth, like the last scene of Edgerunners */
    ridgeLayer(FARW, 222, 10, 4242, C(150, 150, 162), 0, -1, 0); /* a flat white horizon */
    ridgeLayer(FARW, 236, 8, 919, C(180, 180, 192), 0, -1, 0);
    for (int k = 0; k < 18; k++) {
        int x = lr(FARW);
        crater(x, ridgeY(x, 236, 8, 919) + 4 + lr(10), 3 + lr(8), C(180, 180, 192));
    }
}

/* the Earth from low orbit, bright and close, wrapped in a cyan glow; drawn only inside the clip box */
void orbitEarth(int cx, int cy, int r, int x0, int y0, int x1, int y1)
{
    int halo = r / 12 + 6;
    for (int y = cy - r - halo; y <= cy + r + halo; y++) {
        if (y < y0 || y >= y1) continue;
        for (int x = cx - r - halo; x <= cx + r + halo; x++) {
            if (x < x0 || x >= x1) continue;
            int i = x - cx, j = y - cy;
            float d = sqrtf((float)(i * i + j * j));
            if (d > r) { /* the atmosphere glows cyan into space */
                float g = 1 - (d - r) / halo;
                if (g > 0 && g * g * 16 > bayer[y & 3][x & 3] * 0.7f) pmix(x, y, C(90, 230, 255), 0.25f + 0.6f * g);
                continue;
            }
            float nx = i / (float)r, ny = j / (float)r, nz = sqrtf(fmaxf(0, 1 - nx * nx - ny * ny));
            float sx = nx * (1.6f - 0.6f * nz) * 1.3f, sy = ny * (1.6f - 0.6f * nz) * 1.3f;
            float v = sinf(sx * 3.1f + 0.7f) + sinf(sy * 3.7f - sx * 1.9f) + 0.6f * sinf((sx - sy) * 6.3f);
            float wx = sx * 4 + 1.4f * sinf(sy * 5 + sx * 2), wy = sy * 4 + 1.4f * sinf(sx * 4 - sy * 3);
            float cl = sinf(wx * 1.3f + sinf(wy * 1.7f) * 2.2f) * sinf(wy * 1.5f + sinf(wx * 1.3f) * 1.8f)
                     + 0.35f * sinf(wx * 3.7f + wy * 2.9f + sinf(wy * 2) * 1.5f);
            float cloud = clampf((cl - 0.1f) * 1.6f, 0, 1);
            unsigned c = v > 1.2f ? (v > 1.9f ? C(150, 160, 130) : C(60, 100, 96)) : (v > 0.4f ? C(22, 92, 150) : C(10, 52, 104));
            if (cloud * 16 > bayer[y & 3][x & 3]) c = mixc(C(170, 220, 245), C(245, 252, 255), cloud);
            c = scalec(c, (int)(150 + 106 * nz));
            float rim = (1 - nz) * (1 - nz);
            if (rim * 16 > bayer[y & 3][x & 3] * 0.6f) c = mixc(c, C(150, 245, 255), 0.25f + 0.5f * rim); /* bright limb */
            pset(x, y, c);
        }
    }
}
unsigned *pics[PIC_N];
int picsOk[PIC_N];
unsigned *pic; /* the DAVID AND LUCY picture, also a MENU BACKGROUND choice, once picOk */
int picOk;
int mapPic(int m) { return m == D_ORBIT ? PIC_ORBIT : m == D_MOON ? PIC_MOON : m == D_LAUNCH ? PIC_LAUNCH : m == D_FREEFALL ? PIC_FREEFALL : -1; }
int hasPic(int m) { int k = mapPic(m); return k >= 0 && picsOk[k]; }

/* the DAVID AND LUCY palette was picked by hand; the others are worked out from their picture */
static const unsigned char ORBIT_PAL[][3] = {
    { 2, 2, 3 }, { 3, 4, 13 }, { 2, 6, 28 }, { 3, 13, 48 }, { 10, 24, 73 }, { 14, 52, 14 }, { 23, 46, 102 }, { 34, 79, 32 },
    { 60, 151, 74 }, { 32, 94, 80 }, { 35, 82, 128 }, { 50, 109, 141 }, { 34, 114, 177 }, { 81, 142, 166 }, { 52, 182, 202 },
    { 118, 174, 186 }, { 163, 93, 228 }, { 103, 232, 131 }, { 107, 203, 245 }, { 141, 231, 237 }, { 159, 204, 206 },
    { 185, 234, 232 }, { 211, 246, 244 }, { 235, 252, 249 }, { 214, 200, 52 }, { 160, 152, 40 }, { 108, 108, 34 },
    { 74, 64, 120 }, { 200, 120, 255 } };
#define ORBIT_PAL_N ((int)(sizeof ORBIT_PAL / sizeof ORBIT_PAL[0]))
#define PPAL_MAX 32
static unsigned char picPal[PIC_N][PPAL_MAX][3];
static int picPalN[PIC_N];
unsigned char (*picIdx)[PBW * PBH]; /* [PIC_N], in the workspace */
short (*smp)[3];                     /* [PBW * PBH] block colours while a picture is built, in the workspace */
int picBuilt[PIC_N];

static int picNearest(int k, int r, int g, int b)
{
    int best = 0, bd = 1 << 30;
    for (int i = 0; i < picPalN[k]; i++) {
        int dr = r - picPal[k][i][0], dg = g - picPal[k][i][1], db = b - picPal[k][i][2];
        int d = dr * dr * 4 + dg * dg * 5 + db * db * 3;
        if (d < bd) { bd = d; best = i; }
    }
    return best;
}
static int picLum(int k, int i) { return picPal[k][i][0] * 3 + picPal[k][i][1] * 6 + picPal[k][i][2]; }
unsigned picCol(int k, int gx, int gy) /* one block, outlined, dimmed a little so the fight stays readable */
{
    gx = clampi(gx, 0, PBW - 1); gy = clampi(gy, 0, PBH - 1);
    const unsigned char *ix = picIdx[k];
    int i = ix[gy * PBW + gx], l = picLum(k, i);
    const unsigned char *q = picPal[k][i];
    int dark = (gx + 1 < PBW && picLum(k, ix[gy * PBW + gx + 1]) - l > 800) || (gy + 1 < PBH && picLum(k, ix[(gy + 1) * PBW + gx]) - l > 800);
    return scalec(C(q[0], q[1], q[2]), dark ? 110 : 238);
}
void buildPicture(int k) /* fills picIdx[k] once */
{
    static int sum[PPAL_MAX][4];
    if (picBuilt[k]) return;
    picBuilt[k] = 1;
    const unsigned *src = pics[k];
    for (int y = 0; y < PBH; y++)
        for (int x = 0; x < PBW; x++) {
            int r = 0, g = 0, b = 0;
            for (int j = 0; j < 2; j++)
                for (int i = 0; i < 2; i++) { unsigned c = src[(y * 2 + j) * FW + x * 2 + i]; r += c & 255; g += (c >> 8) & 255; b += (c >> 16) & 255; }
            smp[y * PBW + x][0] = (short)(r / 4); smp[y * PBW + x][1] = (short)(g / 4); smp[y * PBW + x][2] = (short)(b / 4);
        }
    if (k == PIC_ORBIT) {
        picPalN[k] = ORBIT_PAL_N;
        memcpy(picPal[k], ORBIT_PAL, sizeof ORBIT_PAL);
    } else { /* farthest-point seeds (so small bright details like neon edges get a colour), then a few k-means rounds */
        int n = PBW * PBH, np = 1;
        long tr = 0, tg = 0, tb = 0;
        for (int s = 0; s < n; s++) { tr += smp[s][0]; tg += smp[s][1]; tb += smp[s][2]; }
        picPal[k][0][0] = (unsigned char)(tr / n); picPal[k][0][1] = (unsigned char)(tg / n); picPal[k][0][2] = (unsigned char)(tb / n);
        picPalN[k] = 1;
        while (np < 28) {
            int pick = 0, fd = -1;
            for (int s = 0; s < n; s += 3) {
                int i = picNearest(k, smp[s][0], smp[s][1], smp[s][2]);
                int dr = smp[s][0] - picPal[k][i][0], dg = smp[s][1] - picPal[k][i][1], db = smp[s][2] - picPal[k][i][2];
                int d = dr * dr * 4 + dg * dg * 5 + db * db * 3;
                if (d > fd) { fd = d; pick = s; }
            }
            for (int c = 0; c < 3; c++) picPal[k][np][c] = (unsigned char)smp[pick][c];
            picPalN[k] = ++np;
        }
        for (int it = 0; it < 6; it++) {
            memset(sum, 0, sizeof sum);
            for (int s = 0; s < n; s++) {
                int i = picNearest(k, smp[s][0], smp[s][1], smp[s][2]);
                sum[i][0] += smp[s][0]; sum[i][1] += smp[s][1]; sum[i][2] += smp[s][2]; sum[i][3]++;
            }
            for (int i = 0; i < np; i++)
                if (sum[i][3]) for (int c = 0; c < 3; c++) picPal[k][i][c] = (unsigned char)(sum[i][c] / sum[i][3]);
        }
    }
    for (int s = 0; s < PBW * PBH; s++) picIdx[k][s] = (unsigned char)picNearest(k, smp[s][0], smp[s][1], smp[s][2]);
}
