/* render_giants.c - Enemy sprites, arm guns and the six giant bosses, painted in their own buffer. */
#include "core.h"

void drawArmGun(float sx, float sy, float a, int len, int thick, unsigned col, int muzzle, unsigned armCol)
{
    int x0 = (int)sx + ox, y0 = (int)sy + oy;
    int hx = (int)roundf(sx + cosf(a) * 5) + ox, hy = (int)roundf(sy + sinf(a) * 5) + oy;
    int gx = (int)roundf(sx + cosf(a) * (5 + len)) + ox, gy = (int)roundf(sy + sinf(a) * (5 + len)) + oy;
    thickLine(x0, y0, hx, hy, C_OUT, 3);
    thickLine(x0, y0, hx, hy, armCol, 2);
    thickLine(hx, hy + 1, gx, gy + 1, C_OUT, thick);
    thickLine(hx, hy, gx, gy, col, thick);
    pset(hx, hy, C(232, 180, 140));
    if (muzzle) { fillCircle(gx, gy, 2, C(255, 240, 160)); pset(gx, gy, C_WHITE); }
}

const Spr *enemySprite(const Enemy *e, int *flip)
{
    int f = ((int)(e->anim * 7)) & 1, moving = fabsf(e->vx) > 8 && e->onGround;
    *flip = e->facing < 0;
    switch (e->type) {
    case EN_THUG: return moving && f ? enemyBodySpr(EB_THUG, 1, &sThug1) : enemyBodySpr(EB_THUG, 0, &sThug0);
    case EN_DRONE: *flip = 0; return ((int)(e->anim * 20) & 1) ? enemyBodySpr(EB_DRONE, 1, &sDrone1) : enemyBodySpr(EB_DRONE, 0, &sDrone0);
    case EN_GUNNER: return moving && f ? enemyBodySpr(EB_GUNNER, 1, &sGunner1) : enemyBodySpr(EB_GUNNER, 0, &sGunner0);
    case EN_BOMBER: return f ? &sBomber1 : &sBomber0;
    case EN_BRUTE: return moving && (((int)(e->anim * 5)) & 1) ? enemyBodySpr(EB_BRUTE, 1, &sBrute1) : enemyBodySpr(EB_BRUTE, 0, &sBrute0);
    case EN_SHIELD: return (moving || e->state == 2) && f ? &sShield1 : &sShield0;
    case EN_NINJA: return (moving || e->state == 2) && f ? &sCyNinja1 : &sCyNinja0;
    case EN_JET: return ((int)(e->anim * 14) & 1) ? &sJet1 : &sJet0;
    case EN_MECH: return moving && (((int)(e->anim * 4)) & 1) ? &sMech1 : &sMech0;
    default: *flip = e->facing < 0; return &sGunship;
    }
}

/* the big boss: a giant character painted into its own small buffer (always facing right, feet at GOY),
   then copied onto the screen mirrored to its facing, with one dark outline around the whole figure */
#define GW 128
#define GH 112
#define GOX 64
#define GOY 104
static unsigned gBuf[GW * GH];
typedef struct { unsigned d, m, l; } Mat; /* a material: shadow, base and highlight */
static Mat MT(unsigned d, unsigned m, unsigned l) { Mat t; t.d = d; t.m = m; t.l = l; return t; }

/* the shape being filled: 0 ellipse (cx cy rx ry), 1 tapered limb (x0 y0 x1 y1 r0 r1), 2 polygon (points) */
static int gKind, gN;
static const float *gP;
static float gBx0, gBy0, gBx1, gBy1;
static int gInside(float x, float y, float *nx, float *ny)
{
    const float *p = gP;
    if (gKind == 0) {
        float u = (x - p[0]) / p[2], v = (y - p[1]) / p[3];
        if (u * u + v * v > 1) return 0;
        *nx = u; *ny = v;
        return 1;
    }
    if (gKind == 1) {
        float vx = p[2] - p[0], vy = p[3] - p[1], l2 = vx * vx + vy * vy;
        float t = l2 > 0 ? ((x - p[0]) * vx + (y - p[1]) * vy) / l2 : 0;
        t = t < 0 ? 0 : t > 1 ? 1 : t;
        float r = p[4] + (p[5] - p[4]) * t, qx = x - (p[0] + vx * t), qy = y - (p[1] + vy * t);
        if (qx * qx + qy * qy > r * r) return 0;
        *nx = qx / r; *ny = qy / r;
        return 1;
    }
    int in = 0;
    for (int i = 0, j = gN - 1; i < gN; j = i++) {
        float xi = p[i * 2], yi = p[i * 2 + 1], xj = p[j * 2], yj = p[j * 2 + 1];
        if ((yi > y) != (yj > y) && x < (xj - xi) * (y - yi) / (yj - yi) + xi) in = !in;
    }
    if (!in) return 0;
    *nx = (x - (gBx0 + gBx1) / 2) / ((gBx1 - gBx0) / 2 + 0.5f);
    *ny = (y - (gBy0 + gBy1) / 2) / ((gBy1 - gBy0) / 2 + 0.5f);
    return 1;
}

/* fill the current shape: lit from the upper front, dithered between tones, with a dark rim on its shadow
   side and a bright rim on its lit side so overlapping parts stay readable */
static void gFill(Mat t)
{
    float nx, ny, ax, ay;
    for (int iy = (int)floorf(gBy0) - 1; iy <= (int)gBy1 + 1; iy++)
        for (int ix = (int)floorf(gBx0) - 1; ix <= (int)gBx1 + 1; ix++) {
            int bx = ix + GOX, by = iy + GOY;
            if ((unsigned)bx >= GW || (unsigned)by >= GH) continue;
            float x = ix + 0.5f, y = iy + 0.5f;
            if (!gInside(x, y, &nx, &ny)) continue;
            int edge = !gInside(x + 1, y, &ax, &ay) || !gInside(x - 1, y, &ax, &ay) ||
                       !gInside(x, y + 1, &ax, &ay) || !gInside(x, y - 1, &ax, &ay);
            float d = 0.45f * nx - 0.89f * ny;
            int dith = (bx + by) & 1;
            unsigned c;
            if (edge) c = d > 0.25f ? t.l : scalec(t.d, 150);
            else if (d > 0.62f) c = t.l;
            else if (d > 0.38f) c = dith ? t.l : t.m;
            else if (d < -0.55f) c = t.d;
            else if (d < -0.3f) c = dith ? t.d : t.m;
            else c = t.m;
            gBuf[by * GW + bx] = c;
        }
}
static void gE(float cx, float cy, float rx, float ry, Mat t)
{
    const float p[4] = { cx, cy, rx, ry };
    gKind = 0; gP = p; gBx0 = cx - rx; gBx1 = cx + rx; gBy0 = cy - ry; gBy1 = cy + ry;
    gFill(t);
}
static void gC(float x0, float y0, float x1, float y1, float r0, float r1, Mat t)
{
    const float p[6] = { x0, y0, x1, y1, r0, r1 };
    float r = r0 > r1 ? r0 : r1;
    gKind = 1; gP = p;
    gBx0 = fminf(x0, x1) - r; gBx1 = fmaxf(x0, x1) + r; gBy0 = fminf(y0, y1) - r; gBy1 = fmaxf(y0, y1) + r;
    gFill(t);
}
static void gPoly(const float *p, int n, Mat t)
{
    gKind = 2; gP = p; gN = n;
    gBx0 = gBx1 = p[0]; gBy0 = gBy1 = p[1];
    for (int i = 1; i < n; i++) {
        gBx0 = fminf(gBx0, p[i * 2]); gBx1 = fmaxf(gBx1, p[i * 2]);
        gBy0 = fminf(gBy0, p[i * 2 + 1]); gBy1 = fmaxf(gBy1, p[i * 2 + 1]);
    }
    gFill(t);
}
#define GPOLY(t, ...) do { const float p_[] = { __VA_ARGS__ }; gPoly(p_, (int)(sizeof p_ / sizeof p_[0] / 2), t); } while (0)
/* flat details (glows, seams) in the buffer, local coordinates */
static void gPx(float x, float y, unsigned c) { pset((int)floorf(x) + GOX, (int)floorf(y) + GOY, c); }
static void gR(float x, float y, int w, int h, unsigned c) { fillRect((int)floorf(x) + GOX, (int)floorf(y) + GOY, w, h, c); }
static void gL(float x0, float y0, float x1, float y1, unsigned c)
{
    line((int)floorf(x0) + GOX, (int)floorf(y0) + GOY, (int)floorf(x1) + GOX, (int)floorf(y1) + GOY, c);
}
/* hazard / pattern stripes that only paint over parts already drawn */
static void gStripes(float x, float y, int w, int h, unsigned a, unsigned b)
{
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++) {
            int bx = (int)x + i + GOX, by = (int)y + j + GOY;
            if ((unsigned)bx < GW && (unsigned)by < GH && gBuf[by * GW + bx]) gBuf[by * GW + bx] = ((i + j) >> 2) & 1 ? a : b;
        }
}

/* the arm cannon frame: from the shoulder (gSx, gSy) along the aim (gUx, gUy); t along, s across */
static float gSx, gSy, gUx, gUy;
static float gAx(float t, float s) { return gSx + gUx * t - gUy * s; }
static float gAy(float t, float s) { return gSy + gUy * t + gUx * s; }
static void gArm(float t0, float t1, float r0, float r1, Mat m) { gC(gAx(t0, 0), gAy(t0, 0), gAx(t1, 0), gAy(t1, 0), r0, r1, m); }
static void gBand(float t, float r, unsigned c) { gL(gAx(t, -r), gAy(t, -r), gAx(t, r), gAy(t, r), c); }

/* a leg: hip, foot offset (walk), lift; thigh/shin radii; foot length */
static void gLeg(float hx, float hy, float off, float lift, float tr, float sr, Mat th, Mat sh, Mat ft, float flen)
{
    float kx = hx + off * 0.6f + 2, ky = hy * 0.5f + lift * 0.5f, ax = hx + off, ay = -5 + lift;
    gC(hx, hy, kx, ky, tr, tr * 0.85f, th);
    gC(kx, ky, ax, ay, sr * 0.9f, sr, sh);
    GPOLY(ft, ax - 5, ay - 2, ax + flen - 3, ay - 1, ax + flen, lift, ax - 6, lift);
}

/* --- the six big bosses. b = body bob, fl/bl = front/back foot offset, lf/lb = foot lift, hot = about to fire */
static void giantSmasher(float b, float fl, float bl, float lf, float lb, int hot)
{
    Mat ch = MT(C(46, 50, 62), C(118, 124, 140), C(204, 212, 228)), dm = MT(C(18, 18, 24), C(46, 48, 58), C(92, 96, 112));
    unsigned red = C(255, 40, 40), dred = C(150, 20, 24);
    /* power pack and exhausts on the back */
    gC(-20, -68 + b, -22, -78 + b, 2.5f, 2, ch); gC(-14, -70 + b, -15, -80 + b, 2.5f, 2, ch);
    GPOLY(dm, -26, -70 + b, -10, -72 + b, -8, -40 + b, -24, -42 + b);
    gR(-23, -62 + b, 2, 12, dred); gR(-19, -62 + b, 2, 12, dred);
    /* back arm hanging, huge fist */
    gC(-11, -53 + b, -18, -39 + b, 7, 6, ch); gC(-18, -39 + b, -15, -27 + b, 6, 7, dm); gE(-14, -23 + b, 7, 6, ch);
    gLeg(-5, -30 + b, bl, lb, 7, 6, dm, ch, dm, 11);
    gLeg(5, -30 + b, fl, lf, 7, 6, dm, ch, dm, 12);
    /* pelvis and exposed belly machinery with red cables */
    GPOLY(dm, -12, -38 + b, 12, -38 + b, 9, -26 + b, -9, -26 + b);
    GPOLY(ch, -4, -32 + b, 4, -32 + b, 2, -25 + b, -2, -25 + b);
    gL(-7, -39 + b, -5, -30 + b, red); gL(-3, -39 + b, -2, -30 + b, dred); gL(3, -39 + b, 4, -30 + b, red);
    /* the barrel chest, chrome, with a red core */
    gE(-1, -49 + b, 17, 13, ch);
    gE(6, -51 + b, 9, 6, ch);
    gL(-12, -42 + b, 10, -42 + b, ch.d);
    gL(-13, -47 + b, -8, -38 + b, red); gL(-10, -48 + b, -5, -39 + b, dred);
    fillCircle(GOX + 2, GOY + (int)(-46 + b), 2, red); gPx(1, -47 + b, C(255, 200, 200));
    /* small sunk head: dark helmet, metal jaw, one red optic */
    gE(6, -61 + b, 6, 6, dm);
    GPOLY(ch, 5, -60 + b, 13, -59 + b, 12, -54 + b, 5, -54 + b);
    gL(7, -56 + b, 12, -56 + b, ch.d); gPx(9, -55 + b, ch.d); gPx(11, -55 + b, ch.d);
    gR(8, -62 + b, 4, 1, red); gPx(11, -62 + b, C(255, 200, 200));
    /* missile pod on the back shoulder */
    GPOLY(dm, -12, -78 + b, 3, -78 + b, 3, -66 + b, -12, -66 + b);
    gL(-12, -72 + b, 3, -72 + b, dm.l);
    gPx(2, -76 + b, red); gPx(2, -74 + b, red); gPx(2, -70 + b, red); gPx(2, -68 + b, red);
    /* front pauldron */
    gE(13, -56 + b, 10, 7, ch);
    gL(5, -52 + b, 21, -53 + b, dred);
    /* rotary arm cannon */
    gArm(0, 9, 6, 5, dm);
    gArm(8, 24, 6, 5, dm);
    gBand(11, 6, ch.l); gBand(17, 6, ch.l);
    for (int s = -2; s <= 2; s += 2) gL(gAx(23, s), gAy(23, s), gAx(29, s), gAy(29, s), ch.m);
    gPx(gAx(29, 0), gAy(29, 0), hot ? C_WHITE : red);
}

static void giantOgre(float b, float fl, float bl, float lf, float lb, int hot)
{
    Mat sk = MT(C(84, 16, 22), C(160, 40, 40), C(226, 104, 82)), bone = MT(C(140, 118, 86), C(212, 194, 152), C(250, 240, 212));
    Mat lea = MT(C(46, 28, 22), C(94, 60, 40), C(142, 98, 64)), iron = MT(C(26, 26, 30), C(68, 68, 76), C(126, 126, 136));
    unsigned eye = C(255, 220, 60);
    /* back horn and the long back arm dragging its knuckles */
    gC(4, -60 + b, -2, -68 + b, 2.5f, 1.5f, bone);
    gC(-9, -52 + b, -17, -36 + b, 8, 7, sk); gC(-17, -36 + b, -15, -18 + b, 7, 6, sk);
    gC(-16, -30 + b, -15, -25 + b, 7.5f, 7.5f, lea);
    gE(-14, -14 + b, 7, 6, sk);
    gPx(-9, -16 + b, bone.l); gPx(-9, -12 + b, bone.l);
    /* short, thick legs with wrapped shins and clawed toes */
    gLeg(-6, -26 + b, bl, lb, 8, 7, sk, sk, sk, 10);
    gLeg(6, -26 + b, fl, lf, 8, 7, sk, sk, sk, 11);
    gC(6 + fl * 0.8f, -13 + lf, 6 + fl, -7 + lf, 7, 7, lea);
    gPx(6 + fl + 8, -1 + lf, bone.l); gPx(6 + fl + 5, -1 + lf, bone.l);
    /* loincloth and belt with a skull buckle */
    float sw = sinf(gt * 4) * 1.5f;
    GPOLY(lea, -12, -30 + b, 13, -30 + b, 10 + sw, -14 + b, 2 + sw, -16 + b, -9 + sw, -15 + b);
    /* the gut, the chest and the scars */
    gE(4, -37 + b, 15, 11, sk);
    gE(0, -50 + b, 19, 11, sk);
    gL(-2, -52 + b, 4, -46 + b, sk.d); gL(6, -54 + b, 10, -48 + b, sk.d); gL(-10, -44 + b, -4, -40 + b, sk.d);
    gR(-13, -31 + b, 27, 3, iron.m); gL(-13, -31 + b, 13, -31 + b, iron.l);
    gE(3, -30 + b, 3, 3, bone); gPx(2, -31 + b, C_OUT); gPx(4, -31 + b, C_OUT);
    /* leather shoulder pad with bone spikes */
    gE(4, -55 + b, 9, 6, lea);
    GPOLY(bone, -3, -59 + b, 1, -60 + b, -2, -67 + b);
    GPOLY(bone, 3, -60 + b, 7, -60 + b, 5, -68 + b);
    /* the head hunched forward between the shoulders: brow, glowing eyes, tusks, horn */
    gE(15, -60 + b, 8, 7, sk);
    GPOLY(sk, 9, -66 + b, 23, -65 + b, 22, -61 + b, 10, -61 + b);
    gR(16, -63 + b, 3, 1, eye); gPx(20, -63 + b, eye);
    gL(14, -56 + b, 21, -56 + b, sk.d);
    GPOLY(bone, 15, -56 + b, 17, -56 + b, 16, -60 + b);
    GPOLY(bone, 19, -56 + b, 21, -56 + b, 20, -59 + b);
    gC(11, -66 + b, 7, -73 + b, 2.5f, 1.8f, bone); gC(7, -73 + b, 10, -78 + b, 1.8f, 0.8f, bone);
    /* crude iron hand cannon strapped to the forearm */
    gArm(0, 10, 7, 6, sk);
    gArm(8, 26, 6, 5, iron);
    gBand(12, 6, lea.m); gBand(13, 6, lea.d); gBand(20, 6, lea.m);
    GPOLY(bone, gAx(16, -5), gAy(16, -5), gAx(19, -5), gAy(19, -5), gAx(17, -10), gAy(17, -10));
    gArm(25, 28, 6, 6, iron);
    gPx(gAx(28, 0), gAy(28, 0), hot ? C_WHITE : eye);
}

static void giantShogun(float b, float fl, float bl, float lf, float lb, int hot)
{
    Mat laq = MT(C(16, 14, 20), C(44, 40, 56), C(92, 86, 110)), gold = MT(C(116, 78, 22), C(206, 162, 60), C(255, 228, 140));
    Mat cord = MT(C(86, 14, 20), C(166, 34, 40), C(230, 92, 80)), wood = MT(C(60, 30, 18), C(116, 64, 36), C(170, 108, 64));
    unsigned glow = C(190, 90, 255), gl = gold.m;
    float sw = sinf(gt * 4) * 1.2f;
    /* katana on the back, hilt over the shoulder */
    gC(-20, -32 + b, -8, -70 + b, 1.5f, 1.5f, laq);
    gC(-8, -70 + b, -5, -80 + b, 1.5f, 1.5f, cord);
    gL(-11, -71 + b, -5, -69 + b, gl);
    /* back shoulder plate (sode) and arm */
    GPOLY(laq, -23, -58 + b, -8, -58 + b, -8, -40 + b, -23, -42 + b);
    for (int r = 0; r < 3; r++) gL(-22, -53 + r * 5 + b, -9, -53 + r * 5 + b, gl);
    gC(-12, -48 + b, -14, -32 + b, 5, 5, laq); gE(-14, -29 + b, 5, 5, laq);
    /* wide hakama legs with armoured shins */
    GPOLY(laq, -11, -30 + b, -1, -30 + b, -3 + bl, -7 + lb, -15 + bl, -7 + lb);
    GPOLY(laq, 0, -30 + b, 11, -30 + b, 14 + fl, -7 + lf, 2 + fl, -7 + lf);
    gC(-8 + bl, -13 + lb, -8 + bl, -4 + lb, 4, 4, laq); gC(8 + fl, -13 + lf, 8 + fl, -4 + lf, 4, 4, laq);
    gL(-11 + bl, -12 + lb, -11 + bl, -4 + lb, gl); gL(11 + fl, -12 + lf, 11 + fl, -4 + lf, gl);
    GPOLY(laq, -13 + bl, -3 + lb, -2 + bl, -3 + lb, 0 + bl, lb, -14 + bl, lb);
    GPOLY(laq, 3 + fl, -3 + lf, 14 + fl, -3 + lf, 16 + fl, lf, 2 + fl, lf);
    /* armoured skirt: three laced plates that swing */
    for (int i = 0; i < 3; i++) {
        float x = -15 + i * 10, s = sw * (i - 1);
        GPOLY(laq, x, -34 + b, x + 10, -34 + b, x + 11 + s, -19 + b, x - 1 + s, -19 + b);
        gL(x + s, -20 + b, x + 10 + s, -20 + b, gl);
        gL(x + 5, -33 + b, x + 5 + s, -21 + b, cord.m);
    }
    /* cuirass with gold rows and crossed cords */
    GPOLY(laq, -15, -61 + b, 15, -61 + b, 13, -34 + b, -13, -34 + b);
    for (int r = 0; r < 3; r++) gL(-14, -53 + r * 6 + b, 14, -53 + r * 6 + b, gl);
    gL(-6, -58 + b, 6, -38 + b, cord.m); gL(6, -58 + b, -6, -38 + b, cord.m);
    gE(0, -48 + b, 3, 3, gold);
    /* helmet: flared neck guard, dome, gold crescent, red face mask with glowing eyes */
    GPOLY(laq, -10, -63 + b, 9, -63 + b, 13, -56 + b, -14, -56 + b);
    gL(-13, -57 + b, 12, -57 + b, gl);
    gE(0, -66 + b, 8, 6, laq);
    GPOLY(cord, 1, -64 + b, 10, -64 + b, 10, -58 + b, 3, -57 + b);
    gL(4, -60 + b, 10, -59 + b, cord.d);
    gR(5, -63 + b, 4, 1, glow);
    gC(1, -71 + b, -5, -79 + b, 1.5f, 0.8f, gold); gC(1, -71 + b, 8, -79 + b, 1.5f, 0.8f, gold);
    gE(1, -70 + b, 2, 2, gold);
    /* front shoulder plate */
    GPOLY(laq, 7, -56 + b, 23, -55 + b, 23, -38 + b, 8, -39 + b);
    for (int r = 0; r < 3; r++) gL(9, -51 + r * 5 + b, 22, -50 + r * 5 + b, gl);
    gL(15, -55 + b, 15, -39 + b, cord.m);
    /* matchlock cannon: wooden stock, long lacquered barrel, gold bands */
    gArm(0, 10, 5, 5, wood);
    gArm(8, 28, 3, 3, laq);
    gBand(12, 3, gl); gBand(19, 3, gl); gBand(26, 3, gl);
    gPx(gAx(29, 0), gAy(29, 0), hot ? C_WHITE : glow);
}

static void giantToxic(float b, float fl, float bl, float lf, float lb, int hot)
{
    Mat suit = MT(C(36, 58, 26), C(80, 118, 48), C(142, 178, 82)), rub = MT(C(16, 20, 16), C(42, 48, 40), C(84, 92, 76));
    Mat glass = MT(C(10, 40, 20), C(28, 90, 40), C(160, 255, 150)), tank = MT(C(60, 62, 66), C(130, 134, 140), C(206, 210, 216));
    unsigned glow = C(120, 255, 90), dglow = C(40, 140, 40), haz = C(236, 196, 40);
    /* two chemical tanks on the back with bubbling green windows */
    for (int i = 0; i < 2; i++) {
        float x = -22 + i * 8;
        gC(x, -66 + b, x, -38 + b, 5, 5, tank);
        gR(x - 1, -62 + b, 3, 20, dglow);
        for (int k = 0; k < 2; k++) gPx(x + k - 1, -44 + b - fmodf(gt * 14 + i * 7 + k * 9, 18), glow);
        gE(x, -67 + b, 3, 2, rub);
    }
    /* back arm */
    gC(-10, -52 + b, -16, -38 + b, 7, 6, suit); gE(-16, -32 + b, 6, 6, rub);
    /* chunky legs and boots */
    gLeg(-6, -28 + b, bl, lb, 7, 7, suit, suit, rub, 12);
    gLeg(6, -28 + b, fl, lf, 7, 7, suit, suit, rub, 13);
    gC(-6 + bl, -9 + lb, -6 + bl, -5 + lb, 7, 7, rub); gC(6 + fl, -9 + lf, 6 + fl, -5 + lf, 7, 7, rub);
    /* round suit body with a hazard band and a glowing badge */
    gE(0, -43 + b, 18, 18, suit);
    gL(-8, -56 + b, -10, -32 + b, suit.d); gL(8, -56 + b, 10, -32 + b, suit.d);
    gStripes(-17, -33 + b, 35, 4, haz, C(30, 26, 20));
    gE(4, -48 + b, 4, 4, rub); gR(3, -50 + b, 3, 3, glow); gPx(4, -49 + b, C_WHITE);
    /* hose from the tanks round the belly to the sprayer */
    gC(-14, -40 + b, -4, -26 + b, 1.8f, 1.8f, rub); gC(-4, -26 + b, 10, -34 + b, 1.8f, 1.8f, rub);
    gPx(-4, -22 + b + fmodf(gt * 30, 22), glow);
    /* shoulder pad */
    gE(12, -53 + b, 8, 6, rub);
    /* gas-mask helmet: big visor with glowing eyes, filter canisters */
    gE(3, -65 + b, 9, 8, suit);
    gE(7, -65 + b, 6, 5, glass);
    gPx(6, -65 + b, glow); gPx(10, -65 + b, glow); gPx(5, -67 + b, C_WHITE);
    gC(9, -59 + b, 14, -56 + b, 2.5f, 2.5f, rub); gE(-2, -59 + b, 3, 3, rub);
    /* the sprayer: a glass canister of green goo and a long nozzle */
    gArm(0, 8, 6, 6, suit);
    gArm(6, 14, 5, 5, glass);
    gArm(13, 27, 3, 2, rub);
    gBand(16, 3, haz); gBand(22, 3, haz);
    gPx(gAx(28, 0), gAy(28, 0), hot ? C_WHITE : glow);
}

static void giantReaper(float b, float fl, float bl, float lf, float lb, int hot)
{
    Mat clk = MT(C(24, 32, 62), C(58, 84, 134), C(124, 164, 214)), ice = MT(C(110, 150, 190), C(198, 226, 248), C(252, 254, 255));
    Mat bone = MT(C(112, 116, 132), C(200, 204, 216), C(242, 244, 252)), wood = MT(C(20, 24, 36), C(48, 54, 74), C(96, 104, 130));
    unsigned glow = C(80, 230, 255);
    float sw = sinf(gt * 3) * 2;
    (void)lb;
    /* the scythe slung on the back */
    gC(-18, -18 + b, -4, -84 + b, 1.5f, 1.5f, wood);
    GPOLY(ice, -3, -86 + b, -18, -86 + b, -34, -78 + b, -40, -68 + b, -30, -74 + b, -16, -80 + b, -4, -80 + b);
    /* skeletal back hand in a wide sleeve */
    GPOLY(clk, -10, -56 + b, -4, -50 + b, -14, -32 + b, -22, -34 + b);
    gL(-19, -32 + b, -21, -26 + b, bone.m); gL(-17, -32 + b, -18, -25 + b, bone.m); gL(-15, -32 + b, -15, -26 + b, bone.m);
    /* bony feet stepping under the hem */
    gR(-8 + bl, -2 + lb, 5, 2, bone.m); gR(4 + fl, -2 + lf, 6, 2, bone.l);
    /* the long cloak, tattered and swaying */
    GPOLY(clk, -11, -62 + b, 12, -62 + b, 16 + sw, -6, -17 + sw, -6);
    for (int i = 0; i < 6; i++) {
        float x = -17 + sw + i * 5.5f, d = 4 + ((i * 7 + (int)(gt * 6)) % 3);
        GPOLY(clk, x, -7, x + 5.5f, -7, x + 2.5f, -7 + d);
    }
    gL(-4, -52 + b, -7 + sw, -8, clk.d); gL(5, -50 + b, 8 + sw, -9, clk.d); gL(-10, -40 + b, -13 + sw, -8, clk.d);
    /* the open chest: ribs */
    gE(3, -46 + b, 6, 7, MT(C(6, 8, 18), C(10, 12, 26), C(20, 24, 44)));
    for (int r = 0; r < 3; r++) gL(-1, -50 + r * 3 + b, 7, -50 + r * 3 + b, bone.m);
    gL(3, -52 + b, 3, -41 + b, bone.l);
    /* ice spikes on the shoulders */
    GPOLY(ice, -14, -58 + b, -8, -60 + b, -16, -70 + b);
    GPOLY(ice, -10, -60 + b, -5, -61 + b, -8, -67 + b);
    /* hood with a skull inside and a crown of ice */
    GPOLY(clk, -9, -56 + b, 13, -56 + b, 13, -66 + b, 5, -75 + b, -7, -72 + b, -12, -62 + b);
    gE(7, -63 + b, 5, 6, MT(C(4, 6, 14), C(8, 10, 22), C(16, 20, 40)));
    gE(8, -63 + b, 3.5f, 4, bone);
    gPx(7, -64 + b, glow); gPx(10, -64 + b, glow);
    gL(7, -60 + b, 10, -60 + b, bone.d);
    GPOLY(ice, -1, -73 + b, 2, -74 + b, 0, -80 + b);
    GPOLY(ice, 3, -74 + b, 6, -74 + b, 6, -82 + b);
    GPOLY(ice, 7, -72 + b, 10, -71 + b, 11, -77 + b);
    /* front shoulder spike */
    GPOLY(ice, 6, -58 + b, 16, -60 + b, 20, -68 + b);
    /* ice staff: sleeve, bony hand, pole and a glowing crystal */
    gArm(0, 7, 5, 4, clk);
    gArm(6, 9, 2.5f, 2.5f, bone);
    gArm(3, 22, 1.5f, 1.5f, wood);
    GPOLY(ice, gAx(21, 0), gAy(21, 0), gAx(25, -3.5f), gAy(25, -3.5f), gAx(30, 0), gAy(30, 0), gAx(25, 3.5f), gAy(25, 3.5f));
    gPx(gAx(25, 0), gAy(25, 0), hot ? C_WHITE : glow);
}

static void giantDemon(float b, float fl, float bl, float lf, float lb, int hot)
{
    Mat sk = MT(C(36, 10, 46), C(94, 28, 114), C(164, 68, 186)), horn = MT(C(24, 18, 32), C(66, 56, 86), C(144, 132, 168));
    Mat wing = MT(C(22, 6, 30), C(58, 16, 72), C(108, 40, 130));
    unsigned pink = C(255, 84, 200), cyan = C(60, 255, 230);
    float fp = sinf(gt * 3) * 3;
    /* far wing peeking over the front shoulder, near wing spread behind */
    GPOLY(wing, 2, -58 + b, 16, -82 + b + fp, 26, -74 + b + fp, 14, -58 + b);
    GPOLY(wing, -6, -58 + b, -26, -82 + b + fp, -42, -74 + b + fp, -38, -60 + b, -32, -54 + b, -26, -56 + b, -18, -46 + b);
    gL(-6, -58 + b, -26, -82 + b + fp, pink); gL(-26, -82 + b + fp, -42, -74 + b + fp, pink);
    gL(-26, -82 + b + fp, -32, -54 + b, horn.m); gL(-26, -82 + b + fp, -26, -56 + b, horn.m);
    /* tail with a neon arrow tip */
    gC(-8, -30 + b, -24, -22 + b, 3, 2.5f, sk); gC(-24, -22 + b, -32, -12, 2.5f, 1.5f, sk);
    GPOLY(sk, -36, -12, -29, -14, -32, -6);
    gL(-35, -12, -31, -8, pink);
    /* back arm with claws */
    gC(-11, -52 + b, -16, -38 + b, 5, 4, sk); gC(-16, -38 + b, -13, -26 + b, 4, 3, sk);
    gL(-14, -24 + b, -16, -20 + b, horn.l); gL(-12, -24 + b, -12, -20 + b, horn.l);
    /* digitigrade legs on hooves */
    for (int f = 0; f < 2; f++) {
        float o = f ? fl : bl, l = f ? lf : lb, hx = f ? 5 : -5;
        gC(hx, -30 + b, hx + 4 + o * 0.5f, -18 + l * 0.5f, 6, 5, sk);
        gC(hx + 4 + o * 0.5f, -18 + l * 0.5f, hx - 3 + o, -8 + l, 4.5f, 3.5f, sk);
        gC(hx - 3 + o, -8 + l, hx + o, -3 + l, 3, 3, sk);
        GPOLY(horn, hx - 3 + o, -3 + l, hx + 4 + o, -3 + l, hx + 5 + o, l, hx - 3 + o, l);
    }
    /* V-shaped torso, abs, neon circuit tattoos */
    GPOLY(sk, -16, -61 + b, 16, -61 + b, 9, -33 + b, -9, -33 + b);
    gE(0, -38 + b, 8, 6, sk);
    gL(-2, -42 + b, 2, -42 + b, sk.d); gL(0, -45 + b, 0, -34 + b, sk.d);
    gL(-12, -57 + b, -4, -52 + b, pink); gL(-4, -52 + b, -4, -46 + b, pink);
    gL(12, -57 + b, 5, -52 + b, pink); gL(5, -52 + b, 5, -46 + b, pink);
    fillCircle(GOX, GOY + (int)(-50 + b), 2, cyan); gPx(0, -51 + b, C_WHITE);
    /* head: long curved horns, cyan eyes, a neon grin */
    gC(1, -66 + b, -7, -73 + b, 2.5f, 1.8f, horn); gC(-7, -73 + b, -9, -82 + b, 1.8f, 0.6f, horn);
    gE(6, -63 + b, 6, 6, sk);
    GPOLY(sk, 3, -59 + b, 12, -60 + b, 9, -55 + b);
    gR(7, -65 + b, 4, 1, cyan); gPx(10, -66 + b, C_WHITE);
    gL(7, -60 + b, 11, -61 + b, pink);
    gC(8, -67 + b, 15, -74 + b, 2.5f, 1.8f, horn); gC(15, -74 + b, 14, -83 + b, 1.8f, 0.6f, horn);
    gPx(14, -83 + b, pink); gPx(-9, -82 + b, pink);
    /* shoulder muscle with a neon mark */
    gE(12, -55 + b, 7, 6, sk);
    gL(9, -57 + b, 15, -53 + b, pink);
    /* clawed arm holding an energy orb */
    gArm(0, 13, 5, 4, sk);
    gBand(10, 4, pink);
    gArm(13, 18, 4, 4, sk);
    for (int s = -3; s <= 3; s += 3) gL(gAx(17, s), gAy(17, s), gAx(21, s * 1.3f), gAy(21, s * 1.3f), horn.l);
    float ox_ = gAx(25, 0), oy_ = gAy(25, 0);
    fillCircle(GOX + (int)floorf(ox_), GOY + (int)floorf(oy_), 3, hot ? C_WHITE : cyan);
    gPx(ox_, oy_, C_WHITE);
}

void drawGiant(const Enemy *e)
{
    int k = e->mega - 1, f = e->facing < 0 ? -1 : 1;
    unsigned ac = MEGA_COL[k];
    int moving = e->onGround && fabsf(e->vx) > 8, air = !e->onGround;
    float ph = e->anim * 9, step = moving ? sinf(ph) * 3.5f : 0;
    float fl = air ? 4 : step, bl = air ? -3 : -step;
    float lf = air ? -5 : moving ? -fmaxf(0, cosf(ph)) * 3 : 0, lb = air ? -3 : moving ? -fmaxf(0, -cosf(ph)) * 3 : 0;
    float bob = air ? -2 : moving ? fabsf(sinf(ph)) * 1.5f : sinf(gt * 2) * 0.7f;
    int hot = e->timer < 0.5f && ((int)(gt * 16) & 1);
    int cx = (int)e->x + ox, fy = (int)(e->y + e->hh) + oy;
    for (int i = -22; i <= 22; i++) pdark(cx + i, fy, 150); /* shadow */
    /* aim in the buffer's facing-right frame */
    float sx, sy, a = megaAim(e, &sx, &sy);
    gUx = cosf(a) * f; gUy = sinf(a);
    gSx = 13; gSy = -50 + bob;
    unsigned *st = tgt;
    int sw_ = tgtW, sh_ = tgtH;
    memset(gBuf, 0, sizeof gBuf);
    target(gBuf, GW, GH);
    switch (k) {
    case 0: giantSmasher(bob, fl, bl, lf, lb, hot); break;
    case 1: giantOgre(bob, fl, bl, lf, lb, hot); break;
    case 2: giantShogun(bob, fl, bl, lf, lb, hot); break;
    case 3: giantToxic(bob, fl, bl, lf, lb, hot); break;
    case 4: giantReaper(bob, fl, bl, lf, lb, hot); break;
    default: giantDemon(bob, fl, bl, lf, lb, hot); break;
    }
    target(st, sw_, sh_);
    /* copy to the screen, mirrored to the facing, outlining the whole figure */
    unsigned flash = e->flash > 0 && state == ST_PLAY ? C_WHITE : 0;
    for (int y = 0; y < GH; y++)
        for (int x = 0; x < GW; x++) {
            unsigned c = gBuf[y * GW + x];
            int px_ = f > 0 ? cx + x - GOX : cx - (x - GOX) - 1, py_ = fy + y - GOY;
            if (c) pset(px_, py_, flash ? flash : c);
            else if ((x > 0 && gBuf[y * GW + x - 1]) || (x < GW - 1 && gBuf[y * GW + x + 1]) ||
                     (y > 0 && gBuf[(y - 1) * GW + x]) || (y < GH - 1 && gBuf[(y + 1) * GW + x])) pset(px_, py_, C_OUT);
        }
    /* a soft glow round the weapon, and the aim warning */
    float gx, gy;
    megaGun(e, &gx, &gy);
    for (int j = -5; j <= 5; j++)
        for (int i = -5; i <= 5; i++)
            if (i * i + j * j <= 25) pmix((int)gx + ox + i, (int)gy + oy + j, ac, 0.18f);
    if (e->state == 0 && hot) {
        float wa = atan2f(py - gy, px - gx);
        for (int i = 8; i < 160; i += 4) pset((int)(gx + cosf(wa) * i) + ox, (int)(gy + sinf(wa) * i) + oy, ac);
    }
    (void)sx; (void)sy;
}
