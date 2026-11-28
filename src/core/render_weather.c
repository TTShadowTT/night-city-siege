/* render_weather.c - Parallax scrolling and weather: rain, snow, lightning, fog, searchlights and traffic. */
#include "core.h"

static void fog(void);

/* rendering */

/* animate the city: twinkling stars, blinking beacons, windows, flickering neon */
void animateLayers(void)
{
    int f8 = (int)(gt * 8);
    target(farL, FARW, FH);
    for (int i = 0; i < nStars; i++)
        pset(starX[i], starY[i], (hash3(i, f8 / 3, 1) % 6 == 0) ? C(90, 90, 120) : C(255, 255, 230));
    for (int i = 0; i < nBeacons; i++) {
        Beacon *b = &beacons[i];
        target(b->layer ? midL : farL, b->layer ? MIDW : FARW, FH);
        int on = fmodf(gt + b->phase, 1.4f) < 0.3f;
        pset(b->x, b->y, on ? C(255, 60, 60) : C(90, 20, 24));
    }
    target(midL, MIDW, FH);
    int slow = (int)(gt * 0.7f);
    for (int i = 0; i < nWindows; i++) {
        Window *w = &windows[i];
        int lit = w->lit ^ (hash3(i, slow, 7) % 31 == 0);
        fillRect(w->x, w->y, 2, 3, lit ? w->on : w->off);
    }
    int f12 = (int)(gt * 12);
    for (int i = 0; i < nSigns; i++) {
        int broken = i % 5 == 0;
        int on = !(hash3(i, f12, 3) % (broken ? 7 : 61) == 0);
        drawSignTextTo(&signs[i], on);
    }
}

void composite(int dimK)
{
    int shift = (int)((FW / 2.f - px) * 0.05f);
    int fs = 16 + clampi(shift / 2, -16, 15), ms = 32 + clampi(shift, -32, 31);
    for (int y = 0; y < FH; y++) {
        int sy = clampi(y - oy, 0, FH - 1);
        const unsigned *fr = farL + sy * FARW, *mr = midL + sy * MIDW, *fo = foreL + sy * FW;
        unsigned *out = fb + y * FW;
        for (int x = 0; x < FW; x++) {
            int sx = clampi(x - ox, 0, FW - 1);
            unsigned c = fo[sx];
            if (!(c >> 24)) { c = mr[sx + ms]; if (!(c >> 24)) c = fr[sx + fs]; }
            out[x] = dimK ? scalec(c, dimK) : c;
        }
    }
}

/* soft beams sweeping the sky behind the platforms */
static void searchlights(unsigned col)
{
    for (int k = 0; k < 2; k++) {
        float bx = k ? FW * 0.78f : FW * 0.22f, a = -PI / 2 + sinf(gt * 0.35f + k * 2.1f) * 0.55f;
        float dx = cosf(a), dy = sinf(a), hw = 0.07f;
        for (int y = 0; y < GROUND_Y; y++) {
            float t = (y - GROUND_Y) / dy;
            if (t <= 0) continue;
            float cx = bx + dx * t, w = t * hw;
            int x0 = (int)(cx - w), x1 = (int)(cx + w);
            for (int x = x0; x <= x1; x++) {
                if ((unsigned)x >= FW || (foreL[y * FW + x] >> 24)) continue;
                float edge = 1 - fabsf(x - cx) / (w + 0.5f);
                pmix(x, y, col, 0.13f * edge * (1 - t / 400));
            }
        }
    }
}

static void rain(int seed)
{
    for (int i = 0; i < 150; i++) {
        unsigned h = hash3(i, 77 + seed, 5);
        float speed = 280 + (h % 80);
        float y = fmodf((h >> 8) % FH + gt * speed, FH + 20) - 10;
        int x = (int)fmodf((float)(h % (FW + 80)) - y * 0.25f + 80, (float)(FW + 80)) - 40;
        for (int k = 0; k < 4; k++) pmix(x + k / 3, (int)y + k, C(150, 170, 230), 0.45f);
        if (y > GROUND_Y - 3 && y < GROUND_Y + 2) { pmix(x - 1, GROUND_Y - 1, C(200, 210, 255), 0.6f); pmix(x + 1, GROUND_Y - 1, C(200, 210, 255), 0.6f); }
    }
}

/* storm lightning: at most one strike every 5.5 s window; returns seconds since this window's strike (<0 = none yet) */
float strikeAge(int *win, int *bx)
{
    int w = (int)(gt / 5.5f);
    unsigned h = hash3(w, 9, 9);
    if (win) *win = w;
    if (bx) *bx = 40 + (int)(h % (FW - 80));
    if (h % 3 == 0) return -1; /* a quiet window */
    return gt - (w * 5.5f + (h >> 4) % 400 / 100.f);
}

static void lightning(void)
{
    int bx;
    float t = strikeAge(0, &bx);
    if (t < 0 || t > 0.35f) return;
    if (t < 0.18f) { /* the bolt: a jagged line from the clouds */
        int x = bx, y = 0;
        while (y < 150) {
            int nx = x + (int)(hash3(bx, y, 3) % 15) - 7, ny = y + 8 + (int)(hash3(y, bx, 4) % 8);
            thickLine(x, y, nx, ny, C(230, 240, 255), 2);
            if (hash3(x, y, 5) % 5 == 0) line(nx, ny, nx + (int)(hash3(y, 1, 6) % 21) - 10, ny + 14, C(180, 200, 255));
            x = nx; y = ny;
        }
    }
    float a = (1 - t / 0.35f) * 0.35f; /* the whole sky flashes */
    for (int i = 0; i < FW * FH; i++) fb[i] = mixc(fb[i], C(220, 230, 255), a);
}

void snow(void)
{
    for (int i = 0; i < 140; i++) {
        unsigned h = hash3(i, 31, 8);
        float sp = 18 + (h % 30), y = fmodf((h >> 8) % FH + gt * sp, FH + 10) - 5;
        int x = (int)fmodf((float)(h % FW) + sinf(gt * 1.3f + i) * 6 + gt * 14, (float)FW);
        pset(x, (int)y, C(245, 248, 255));
        if (h & 16) pset(x + 1, (int)y, C(210, 220, 240));
    }
}

static void leaves(void)
{
    static const unsigned lc[3] = { C(120, 200, 70), C(240, 200, 60), C(230, 130, 50) };
    for (int i = 0; i < 26; i++) {
        unsigned h = hash3(i, 44, 2);
        float y = fmodf((h >> 8) % FH + gt * (16 + h % 14), FH + 10) - 5;
        int x = (int)fmodf((float)(h % FW) + sinf(gt * 2 + i) * 14 + gt * 10, (float)FW);
        unsigned c = lc[i % 3];
        pset(x, (int)y, c);
        pset(x + ((int)(gt * 4 + i) & 1 ? 1 : -1), (int)y + 1, scalec(c, 170));
    }
}

static void fireflies(void)
{
    for (int i = 0; i < 28; i++) {
        unsigned h = hash3(i, 66, 1);
        int x = (int)(h % FW + sinf(gt * 0.6f + i) * 30), y = 60 + (int)((h >> 9) % 180 + sinf(gt * 0.9f + i * 2) * 12);
        float glow = 0.5f + 0.5f * sinf(gt * 3 + i * 1.7f);
        if (glow < 0.25f) continue;
        pmix(x, y, C(220, 255, 120), glow);
        pmix(x - 1, y, C(180, 255, 90), glow * 0.35f); pmix(x + 1, y, C(180, 255, 90), glow * 0.35f);
        pmix(x, y - 1, C(180, 255, 90), glow * 0.35f); pmix(x, y + 1, C(180, 255, 90), glow * 0.35f);
    }
}

/* per-map effects: behind the action, and in front of it */
/* flying cars streaking through the sky lanes, headlights one way, tail lights the other */
static void avTraffic(int lanes, int y0, int gap)
{
    for (int i = 0; i < lanes * 3; i++) {
        unsigned h = hash3(i, 12, 3);
        int lane = i % lanes, dir = lane & 1 ? -1 : 1, y = y0 + lane * gap + (int)(h % 5);
        float sp = 70 + (h >> 4) % 90;
        int x = (int)fmodf((float)((h >> 8) % (FW + 60)) + gt * sp, (float)(FW + 60)) - 30;
        if (dir < 0) x = FW - x;
        if ((unsigned)x >= FW || (unsigned)y >= GROUND_Y || (foreL[y * FW + x] >> 24)) continue;
        unsigned body = C(30, 28, 40), front = C(255, 250, 220), back = C(255, 50, 60);
        fillRect(x - 4, y, 9, 3, body);
        fillRect(x - 2, y - 1, 5, 1, body);
        pset(x + 4 * dir, y + 1, front); pset(x - 4 * dir, y + 1, back);
        for (int k = 1; k < 10; k++) pmix(x - (4 + k) * dir, y + 1, back, 0.5f * (1 - k / 10.f)); /* light trail */
    }
}

/* big flickering hologram ads hanging over the city center */
static void holoAds(void)
{
    static const struct { short x, y, w, h; const char *txt; } ad[4] = {
        { 18, 30, 110, 46, "KIROSHI" }, { 350, 22, 112, 40, "BRAINDANCE" }, { 14, 150, 92, 26, "NIGHT CITY" }, { 360, 132, 96, 30, "SAKE" } };
    static const unsigned col[4] = { C(255, 50, 180), C(60, 255, 230), C(255, 220, 60), C(160, 120, 255) };
    for (int k = 0; k < 4; k++) {
        int f = (int)(gt * 10) + k * 7;
        if (hash3(f, k, 4) % 19 == 0) continue; /* glitch: blinks out for a moment */
        unsigned c = col[(k + (int)(gt / 4)) & 3];
        int x0 = ad[k].x, y0 = ad[k].y, w = ad[k].w, h = ad[k].h, scan = (int)(gt * 30) % h;
        for (int y = y0; y < y0 + h; y++)
            for (int x = x0; x < x0 + w; x++) {
                if ((unsigned)x >= FW || (unsigned)y >= FH || (foreL[y * FW + x] >> 24)) continue;
                int border = y == y0 || y == y0 + h - 1 || x == x0 || x == x0 + w - 1;
                float a = border ? 0.7f : (y & 1) ? 0.10f : 0.18f;
                if (y - y0 == scan) a = 0.35f;
                pmix(x, y, c, a);
            }
        int sc = w > 100 && strlen(ad[k].txt) < 9 ? 2 : 1;
        text(x0 + w / 2 + ((hash3(f, k, 9) % 11 == 0) ? 2 : 0), y0 + h / 2 - 3 * sc, ad[k].txt, mixc(c, C_WHITE, 0.4f), sc, AL_CENTER);
    }
}

void ambienceBack(void)
{
    if (curMap == D_CITYCENTER) { searchlights(C(255, 120, 220)); avTraffic(3, 70, 30); holoAds(); }
    if (curMap == D_DOWNTOWN || curMap == D_SKYLINE) searchlights(C(210, 220, 255));
    if (curMap == D_ARASAKA) { searchlights(C(255, 70, 80)); avTraffic(3, 150, 26); }
    if (curMap == D_NIGHTCITY) { searchlights(C(255, 230, 140)); avTraffic(4, 40, 22); }
    if (curMap == D_HARBOR || curMap == D_FOREST) fog();
}

void ambienceFront(void)
{
    if (curMap == D_NEON || curMap == D_STORM) rain(0);
    if (curMap == D_STORM) { rain(1); lightning(); } /* twice the rain */
    if (curMap == D_MOUNTAIN) snow();
    if (curMap == D_MEADOW) leaves();
    if (curMap == D_FOREST) fireflies();
}

static void fog(void)
{
    for (int band = 0; band < 3; band++) {
        int cy = 110 + band * 50;
        for (int y = cy - 10; y < cy + 10; y++) {
            float a = (1 - fabsf((float)(y - cy)) / 10) * 0.10f;
            for (int x = 0; x < FW; x++) {
                float wv = 0.5f + 0.5f * sinf((x + gt * (12 + band * 6)) * 0.03f + band);
                if (a * wv * 16 > bayer[y & 3][x & 3] * 0.1f) pmix(x, y, C(150, 170, 180), a * wv);
            }
        }
    }
}
