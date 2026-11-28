/* ui_widgets.c - Menu building blocks: panels, shaped panels, buttons, map previews and the backdrop. */
#include "core.h"

/* --- menu drawing --- */
void frame(int x, int y, int w, int h, unsigned fill, unsigned border)
{
    fillRect(x, y, w, h, fill);
    for (int i = 0; i < w; i++) { pset(x + i, y, border); pset(x + i, y + h - 1, border); }
    for (int j = 0; j < h; j++) { pset(x, y + j, border); pset(x + w - 1, y + j, border); }
}

/* ---- shaped panels (UI_SHAPED styles): the outline is a top / bottom row per column, so round,
   cut and wavy shapes all share one fill + border pass; each style then adds its decoration */
#define SHP_MAXW 480
static short shpT[SHP_MAXW], shpB[SHP_MAXW];

static int shpIn(int i, int j, int w) { return i >= 0 && i < w && j >= shpT[i] && j <= shpB[i]; }

static void shpRound(int w, int h, int r)
{
    r = clampi(r, 0, h / 2);
    for (int i = 0; i < w; i++) {
        int k = i < r ? i : w - 1 - i < r ? w - 1 - i : -1, cut = 0;
        if (k >= 0) { float cx = r - k - 0.5f; cut = (int)(r - sqrtf(fmaxf(0.0f, r * r - cx * cx)) + 0.5f); }
        shpT[i] = (short)cut; shpB[i] = (short)(h - 1 - cut);
    }
}

/* fill + border of the current outline at (x, y); thick = border width */
static void shpDraw(int x, int y, int w, int h, unsigned fill, unsigned border, int thick)
{
    for (int i = 0; i < w; i++)
        for (int j = shpT[i]; j <= shpB[i]; j++) {
            int edge = 0;
            for (int d = 1; d <= thick && !edge; d++)
                edge = !shpIn(i - d, j, w) || !shpIn(i + d, j, w) || !shpIn(i, j - d, w) || !shpIn(i, j + d, w);
            pset(x + i, y + j, edge ? border : fill);
        }
    (void)h;
}

static void shpShadow(int x, int y, int w, int dx, int dy, unsigned c)
{
    for (int i = 0; i < w; i++) fillRect(x + i + dx, y + shpT[i] + dy, 1, shpB[i] - shpT[i] + 1, c);
}

static void spider(int sx, int sy, unsigned body)
{
    for (int k = -1; k <= 1; k += 2) { /* four legs a side, bent */
        pset(sx + 2 * k, sy - 1, body); pset(sx + 3 * k, sy - 2, body);
        pset(sx + 2 * k, sy, body); pset(sx + 3 * k, sy, body); pset(sx + 4 * k, sy + 1, body);
        pset(sx + 2 * k, sy + 1, body); pset(sx + 3 * k, sy + 2, body);
        pset(sx + 2 * k, sy + 2, body); pset(sx + 2 * k, sy + 3, body);
    }
    fillRect(sx - 1, sy - 1, 3, 4, body);
    fillRect(sx - 1, sy - 2, 3, 1, body);
    pset(sx - 1, sy + 3, C(255, 120, 20)); pset(sx + 1, sy + 3, C(255, 120, 20));
    pset(sx - 1, sy - 2, C(255, 40, 40)); pset(sx + 1, sy - 2, C(255, 40, 40)); /* eyes */
}

static void cobweb(int cx, int cy, int R, int dirX, unsigned c)
{
    float ang[5];
    for (int s = 0; s < 5; s++) { /* spokes fan out from the corner */
        ang[s] = s * (1.5708f / 4.0f);
        for (int t = 0; t <= R; t++) pmix(cx + dirX * (int)(cosf(ang[s]) * t), cy + (int)(sinf(ang[s]) * t), c, 0.6f);
    }
    for (int ring = 1; ring <= 3; ring++) { /* sagging threads between the spokes */
        float rr = R * ring / 3.2f;
        for (int s = 0; s < 4; s++)
            for (int t = 0; t <= 8; t++) {
                float u = t / 8.0f, a = ang[s] + (ang[s + 1] - ang[s]) * u, sag = rr * (1.0f - 0.18f * sinf(u * 3.14159f));
                pmix(cx + dirX * (int)(cosf(a) * sag), cy + (int)(sinf(a) * sag), c, 0.55f);
            }
    }
}

static void shapedPanel(int x, int y, int w, int h, int focus)
{
    unsigned edge = focus ? ui.edgeHi : ui.edge, fill = focus ? ui.fillHi : ui.fill, shadow = C(4, 4, 8);
    w = clampi(w, 1, SHP_MAXW);
    int small = h < 14, thick = 1, sx = 2, sy = 2;
    unsigned seed = hash3(x, y, w);
    switch (ui.deco) {
    case 4: shpRound(w, h, small ? h / 2 : 9); break;
    case 5: shpRound(w, h, small ? 3 : 5); break;
    case 6: { int c = clampi(h / 3, 2, 6);
        for (int i = 0; i < w; i++) { int k = i < c ? c - i : w - 1 - i < c ? c - (w - 1 - i) : 0; shpT[i] = (short)k; shpB[i] = (short)(h - 1 - k); }
        break; }
    case 7: { float amp = small ? 0.9f : 1.6f;
        for (int i = 0; i < w; i++) {
            shpT[i] = (short)(amp + amp * sinf(i * 0.32f + gt * 2.6f) + 0.5f);
            shpB[i] = (short)(h - 1 - (int)(amp + amp * sinf(i * 0.32f + gt * 2.6f + 2.0f) + 0.5f));
        }
        break; }
    case 9: case 14: shpRound(w, h, 3); break;
    case 13: shpRound(w, h, 0); thick = 2; sx = 3; sy = 3; shadow = ui.titleA; break;
    default: shpRound(w, h, 2); break;
    }
    shpShadow(x, y, w, sx, sy, shadow);
    if (ui.deco == 5) { /* neon tube: a soft coloured glow around the tube */
        unsigned g = focus ? ui.edgeHi : ui.edge;
        for (int i = -2; i < w + 2; i++) {
            int ci = clampi(i, 0, w - 1);
            for (int d = 1; d <= 2; d++) { pmix(x + i, y + shpT[ci] - d, g, d == 1 ? 0.45f : 0.2f); pmix(x + i, y + shpB[ci] + d, g, d == 1 ? 0.45f : 0.2f); }
        }
        for (int j = 0; j < h; j++)
            for (int d = 1; d <= 2; d++) { pmix(x - d, y + j, g, d == 1 ? 0.45f : 0.2f); pmix(x + w - 1 + d, y + j, g, d == 1 ? 0.45f : 0.2f); }
        int flick = !focus && (hash3((int)(gt * 12), x, y) & 63) == 0;
        edge = flick ? scalec(g, 120) : mixc(g, C_WHITE, 0.55f);
    }
    shpDraw(x, y, w, h, fill, edge, thick);

    switch (ui.deco) {
    case 4: /* bubblegum: a glossy shine along the top */
        for (int i = 3; i < w - 3; i++) if (shpT[i] + 2 < shpB[i]) pmix(x + i, y + shpT[i] + 2, C_WHITE, 0.3f);
        if (!small) { pset(x + w - 9, y + 4, C_WHITE); pset(x + w - 7, y + 3, mixc(fill, C_WHITE, 0.7f)); }
        break;
    case 6: /* sci-fi: lit corner ticks and a status light */
        fillRect(x + w / 2 - 6, y + 2, 12, 1, focus ? ui.hi : scalec(edge, 150));
        if (!small) fillRect(x + w / 2 - 6, y + h - 3, 12, 1, focus ? ui.hi : scalec(edge, 150));
        if (!small) fillRect(x + w - 10, y + h - 4, 4, 1, focus ? ui.hi : scalec(edge, 160));
        break;
    case 7: /* ocean: bubbles rising inside */
        for (int k = 0; k < 2 + w / 40; k++) {
            unsigned hk = hash3(seed, k, 7);
            int bx = x + 3 + (int)(hk % (unsigned)(w > 6 ? w - 6 : 1));
            float sp = 6.0f + (hk >> 8) % 8, ph = (float)((hk >> 12) % 100);
            int by = y + h - 2 - (int)fmodf(gt * sp + ph, (float)(h > 4 ? h - 4 : 1));
            bx += (int)(sinf(gt * 3 + ph) * 1.2f);
            if (shpIn(bx - x, by - y, w)) { pmix(bx, by, C(190, 245, 255), 0.7f); pmix(bx + 1, by, C(190, 245, 255), 0.3f); pmix(bx, by - 1, C(190, 245, 255), 0.3f); }
        }
        break;
    case 8: /* inferno: flames licking up from the top edge */
        for (int i = 1; i < w - 1; i++) {
            float n = 0.55f + 0.25f * sinf(i * 0.9f + gt * 11 + (seed & 7)) + 0.2f * sinf(i * 0.37f - gt * 7);
            int fh = (int)(n * (small ? 4 : 7));
            for (int k = 0; k < fh; k++) {
                float t = (float)k / (fh > 1 ? fh - 1 : 1);
                unsigned c = t < 0.4f ? mixc(C(255, 240, 120), C(255, 150, 30), t / 0.4f) : mixc(C(255, 150, 30), C(200, 30, 10), (t - 0.4f) / 0.6f);
                pmix(x + i, y - 1 - k, c, 0.9f - 0.5f * t);
            }
        }
        if (((int)(gt * 6) + (seed & 15)) % 9 == 0) pset(x + 3 + (int)(seed % (unsigned)(w > 6 ? w - 6 : 1)), y - (small ? 6 : 9), C(255, 200, 60));
        break;
    case 9: { /* slime / blood: drips grow from the bottom edge, fall off and grow again */
        int nd = 1 + w / 10, maxL = small ? 4 : 7;
        unsigned dc = focus ? ui.edgeHi : ui.edge, hl = mixc(dc, C_WHITE, 0.45f);
        for (int k = 0; k < nd; k++) {
            unsigned hk = hash3(seed, k, 9);
            int dx = x + 3 + (int)(hk % (unsigned)(w > 8 ? w - 8 : 1)), dw = 2 + (hk >> 9) % 2;
            float per = 2.0f + (float)((hk >> 11) % 100) / 40.0f, ph = (float)((hk >> 3) % 97) / 10.0f;
            float t = fmodf(gt + ph, per) / per;
            int L = (int)(maxL * fminf(1.0f, t / 0.75f) + 0.5f), base = y + shpB[clampi(dx - x, 0, w - 1)];
            fillRect(dx, base, dw, L + 1, dc);
            fillRect(dx, base + L, dw + 1, 2, dc); /* the heavy drop at the end */
            pset(dx, base + 1, hl);
            if (t > 0.75f) { int fy = base + maxL + 2 + (int)((t - 0.75f) * 4 * 14); fillRect(dx, fy, 2, 2, dc); pset(dx, fy, hl); }
        }
        break; }
    case 10: { /* halloween: cobwebs in the corners and a spider on a thread */
        int R = clampi(h - 3, 5, 16);
        cobweb(x + 1, y + 1, R, 1, C(225, 225, 235));
        if (w > 60) cobweb(x + w - 2, y + 1, R, -1, C(225, 225, 235));
        if (!small && w > 40 && (focus || seed % 3u == 0)) {
            int tx = x + w + 4, drop = h / 2 + (int)(sinf(gt * 1.7f + (seed & 15)) * 2.5f);
            for (int j = y - 1; j < y + drop; j++) pmix(tx, j, C(220, 220, 230), 0.6f);
            spider(tx, y + drop + 1, C(20, 12, 24));
        }
        break; }
    case 11: { /* christmas: candy-cane border, snow on top, fairy lights underneath */
        int march = (int)(gt * 5);
        for (int i = 0; i < w; i++)
            for (int j = shpT[i]; j <= shpB[i]; j++)
                if (!shpIn(i - 1, j, w) || !shpIn(i + 1, j, w) || !shpIn(i, j - 1, w) || !shpIn(i, j + 1, w))
                    pset(x + i, y + j, (((i + j) / 3 + march) & 1) ? C(230, 30, 40) : C(250, 250, 250));
        for (int i = 1; i < w - 1; i++) {
            int sh = 1 + (int)(hash3(seed, i / 3, 11) % 3u);
            if (i % 3 == 1) sh = (sh + 1 + (int)(hash3(seed, i / 3 + 1, 11) % 3u)) / 2;
            fillRect(x + i, y - sh + 1, 1, sh + 1, C(248, 250, 255));
            pset(x + i, y + 1, C(205, 220, 240));
        }
        if (!small || w > 50)
            for (int i = 4, n = 0; i < w - 3; i += 8, n++) {
                static const unsigned bulb[4] = { C(255, 50, 50), C(60, 230, 90), C(255, 210, 60), C(80, 160, 255) };
                int on = ((n + march / 2) % 3) != 0;
                pset(x + i, y + h, C(30, 60, 30));
                fillRect(x + i, y + h + 1, 2, 2, on ? bulb[n & 3] : scalec(bulb[n & 3], 90));
                if (on) { pmix(x + i - 1, y + h + 1, bulb[n & 3], 0.3f); pmix(x + i + 2, y + h + 2, bulb[n & 3], 0.3f); }
            }
        break; }
    case 12: { /* glitch: split red / cyan edges that jump, scanlines and a torn slice */
        int step = (int)(gt * 10);
        unsigned hk = hash3(step, x, y);
        int off = (hk % 5u) == 0 ? 3 : 1;
        for (int j = 1; j < h - 1; j += 2) for (int i = 1; i < w - 1; i++) pdark(x + i, y + j, 215);
        for (int i = 0; i < w; i++) {
            pmix(x + i - off, y, C(255, 30, 80), 0.75f); pmix(x + i + off, y + h - 1, C(0, 240, 255), 0.75f);
        }
        for (int j = 0; j < h; j++) { pmix(x - off, y + j, C(255, 30, 80), 0.75f); pmix(x + w - 1 + off, y + j, C(0, 240, 255), 0.75f); }
        if ((hk >> 4) % 4u == 0) {
            int sj = y + 2 + (int)((hk >> 8) % (unsigned)(h > 4 ? h - 4 : 1)), sh = 1 + (hk >> 12) % 2;
            fillRect(x + 4 + (int)((hk >> 14) % 6u), sj, w / 3, sh, (hk & 1) ? C(255, 30, 80) : C(0, 240, 255));
        }
        break; }
    case 15: { /* cyberpsychosis: a cracked panel, magenta / cyan ghosts of its edge that double up in fits,
                  and a toxic green bolt crawling down one side */
        int step = (int)(gt * 8);
        unsigned hk = hash3(step, x / 8, y / 8);
        int fit = hash3((int)(gt * 3), x / 16, 77) % 7u == 0; /* every so often the panel loses it */
        int g = fit ? 3 : 1 + (sinf(gt * 5 + (float)(seed & 15)) > 0.6f);
        if (fit) for (int i = 1; i < w - 1; i++) for (int j = shpT[i] + 1; j < shpB[i]; j++) pmix(x + i, y + j, C(255, 40, 200), 0.12f);
        for (int c = 0; c < (small ? 1 : 2); c++) { /* cracks, like a face splitting apart */
            unsigned r = hash3(seed, c, 9);
            int kx = x + 4 + (int)(r % (unsigned)(w > 8 ? w - 8 : 1)), ky = y + (c ? h - 2 : 1), dy = c ? -1 : 1;
            for (int t = 0; t < (small ? h / 2 : h - 3); t++) {
                r = r * 1103515245u + 12345u;
                kx += (int)((r >> 16) % 3u) - 1; ky += dy;
                if (ky <= y || ky >= y + h - 1) break;
                pmix(kx, ky, C(255, 90, 230), 0.7f);
                pset(kx + 1, ky, C(4, 2, 10));
                if (((r >> 20) & 7) == 0) { pmix(kx - 1, ky, C(255, 90, 230), 0.5f); pmix(kx - 2, ky + dy, C(255, 90, 230), 0.5f); } /* a branch */
            }
        }
        for (int k = 1; k <= (fit ? 2 : 1); k++) { /* the doubled outlines */
            int o = g * k;
            float a = k == 1 ? 0.7f : 0.35f;
            for (int i = 0; i < w; i++) { pmix(x + i - o, y + shpT[i], C(255, 40, 200), a); pmix(x + i + o, y + shpB[i], C(0, 220, 255), a); }
            for (int j = 0; j < h; j++) { pmix(x - o, y + j, C(255, 40, 200), a); pmix(x + w - 1 + o, y + j, C(0, 220, 255), a); }
        }
        if ((hk & 3) != 0 && h > 6) { /* the bolt flickers */
            int bx = (seed >> 3) & 1 ? x + w - 3 : x + 2;
            unsigned r = hash3(step / 2, (int)seed, 5);
            for (int j = 2; j < h - 2; j++) {
                r = r * 1103515245u + 12345u;
                int jx = bx + (int)((r >> 16) % 3u) - 1;
                pset(jx, y + j, C(140, 255, 60));
                pmix(jx - 1, y + j, C(140, 255, 60), 0.3f); pmix(jx + 1, y + j, C(140, 255, 60), 0.3f);
            }
        }
        break; }
    case 13: /* comic: halftone dots */
        for (int i = 3; i < w - 3; i++)
            for (int j = 3; j < h - 3; j++)
                if ((i % 4 == 0 && j % 4 == 0) || (i % 4 == 2 && j % 4 == 2)) pmix(x + i, y + j, C_WHITE, 0.16f);
        break;
    case 14: { /* jungle: a vine along the top with leaves, two vines hanging down the sides */
        unsigned vine = C(40, 130, 40), leaf = C(90, 200, 60), leaf2 = C(60, 160, 45);
        for (int i = 0; i < w; i++) {
            int vy = y + (int)(sinf(i * 0.25f + (seed & 7)) * 1.3f);
            pset(x + i, vy, vine);
            if (i % 6 == 3) { int up = (i / 6) & 1 ? -1 : 1; pset(x + i, vy + up, leaf); pset(x + i + 1, vy + up, leaf); pset(x + i + 1, vy + 2 * up, leaf2); }
        }
        for (int s = 0; s < 2 && w > 30; s++) {
            int vx = s ? x + w - 4 : x + 3, len = small ? h - 2 : h * 2 / 3;
            for (int j = 0; j < len; j++) {
                int sway = (int)(sinf(gt * 1.6f + j * 0.35f + s * 2) * (j / 6.0f));
                pset(vx + sway, y + j, vine);
                if (j % 4 == 2) { int sd = (j / 4) & 1 ? -1 : 1; pset(vx + sway + sd, y + j, leaf); pset(vx + sway + 2 * sd, y + j + 1, leaf2); }
            }
        }
        break; }
    }
}

void panel(int x, int y, int w, int h, int focus)
{
    if (ui.deco >= 4) { shapedPanel(x, y, w, h, focus); return; }
    unsigned edge = focus ? ui.edgeHi : ui.edge;
    fillRect(x + 2, y + 2, w, h, C(4, 4, 8));
    frame(x, y, w, h, focus ? ui.fillHi : ui.fill, edge);
    switch (ui.deco) {
    case 1: /* cyberpunk: lit bar down the left, a clipped top-right corner and a tab on top */
        fillRect(x + 1, y + 1, 2, h - 2, focus ? ui.hi : scalec(edge, 200));
        for (int k = 0; k < 5; k++) { fillRect(x + w - 5 + k, y, 5 - k, 1, C(4, 4, 8)); pset(x + w - 6 + k, y + k, edge); }
        fillRect(x + 6, y - 1, w / 4, 2, edge);
        break;
    case 2: /* double line */
        if (w > 8 && h > 8) {
            unsigned in = scalec(edge, 130);
            fillRect(x + 2, y + 2, w - 4, 1, in); fillRect(x + 2, y + h - 3, w - 4, 1, in);
            fillRect(x + 2, y + 2, 1, h - 4, in); fillRect(x + w - 3, y + 2, 1, h - 4, in);
        }
        break;
    case 3: { /* corner brackets just outside the frame */
        unsigned c = focus ? ui.hi : edge;
        int L = h < 12 ? 3 : 5;
        fillRect(x - 2, y - 2, L, 1, c); fillRect(x - 2, y - 2, 1, L, c);
        fillRect(x + w - L + 2, y - 2, L, 1, c); fillRect(x + w + 1, y - 2, 1, L, c);
        fillRect(x - 2, y + h + 1, L, 1, c); fillRect(x - 2, y + h - L + 2, 1, L, c);
        fillRect(x + w - L + 2, y + h + 1, L, 1, c); fillRect(x + w + 1, y + h - L + 2, 1, L, c);
        break; }
    }
}

void drawButton(const Btn *b, int focus)
{
    int sc = b->h >= 19 ? 2 : 1;
    panel(b->x, b->y, b->w, b->h, focus);
    text(b->x + b->w / 2, b->y + (b->h - 5 * sc) / 2 + 1, b->label, focus ? ui.hi : ui.txt, sc, AL_CENTER);
    if (focus) {
        int ay = b->y + b->h / 2 - 5 + (int)(sinf(gt * 8) * 1.5f);
        text(b->x - 12, ay, ">", ui.hi, 2, AL_CENTER);
        text(b->x + b->w + 11, ay, "<", ui.hi, 2, AL_CENTER);
    }
}

/* a tab at the top of a screen: the open one is lit */
void drawTab(const Btn *b, int on, int focus)
{
    frame(b->x, b->y, b->w, b->h, on ? ui.fillHi : ui.fill, focus ? ui.edgeHi : on ? ui.hi : scalec(ui.edge, 200));
    if (on) fillRect(b->x + 1, b->y + b->h - 2, b->w - 2, 1, ui.hi);
    text(b->x + b->w / 2, b->y + 5, b->label, on || focus ? ui.hi : C(170, 170, 190), 1, AL_CENTER);
}

void drawSpriteScaled(const Spr *s, int x, int y, int sc)
{
    for (int j = 0; j < s->h; j++) {
        const char *row = s->rows[j];
        int len = (int)strlen(row);
        for (int i = 0; i < s->w && i < len; i++)
            if (row[i] != '.') fillRect(x + i * sc, y + j * sc, sc, sc, pal(row[i]));
    }
}

void drawSkinScaled(const Spr *s, int cx, int feet, int sc)
{
    drawSpriteScaled(s, cx - s->w * sc / 2, feet - s->h * sc, sc);
}

void drawLock(int cx, int cy)
{
    fillRect(cx - 3, cy - 7, 1, 6, C(190, 190, 200));
    fillRect(cx + 3, cy - 7, 1, 6, C(190, 190, 200));
    fillRect(cx - 2, cy - 8, 5, 1, C(190, 190, 200));
    frame(cx - 5, cy - 1, 11, 8, C(210, 170, 60), C_OUT);
    fillRect(cx - 4, cy, 9, 1, C(250, 215, 110));
    pset(cx, cy + 2, C_OUT); pset(cx, cy + 3, C_OUT);
}

/* centred text broken onto lines of at most `maxc` characters */
int wrapText(int cx, int y, const char *s, int maxc, unsigned c)
{
    char line_[40];
    int n = (int)strlen(s), start = 0;
    while (start < n) {
        int end = start + maxc < n ? start + maxc : n;
        if (end < n) { int b = end; while (b > start && s[b] != ' ') b--; if (b > start) end = b; }
        int len = end - start < 39 ? end - start : 39;
        memcpy(line_, s + start, len); line_[len] = 0;
        text(cx, y, line_, c, 1, AL_CENTER);
        y += 7;
        start = end;
        while (s[start] == ' ') start++;
    }
    return y;
}

void drawMapPreview(int d, int x, int y, int w, int h, int locked)
{
    const District *dd = &DISTRICTS[d];
    if (hasPic(d)) { /* a picture map: the picture, shrunk */
        int k = mapPic(d);
        buildPicture(k);
        for (int j = 0; j < h; j++)
            for (int i = 0; i < w; i++) pset(x + i, y + j, picCol(k, i * PBW / w, j * PBH / h));
        if (locked) for (int j = 0; j < h; j++) for (int i = 0; i < w; i++) pdark(x + i, y + j, 90);
        return;
    }
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++) {
            float t = j / (float)h * 6;
            int b = (int)t;
            if ((t - b) * 16 > bayer[j & 3][i & 3]) b++;
            pset(x + i, y + j, mixc(dd->skyTop, dd->skyBot, b / 6.f));
        }
    if (d != D_MEADOW && d != D_STORM)
        for (int k = 0; k < 14; k++) pset(x + (int)(hash3(k, d, 1) % (unsigned)w), y + (int)(hash3(k, d, 2) % (unsigned)(h / 2)), C(220, 220, 240));
    if (dd->moonR) fillCircle(x + w * dd->moonX / FW, y + 10 + (d == D_MOUNTAIN ? 14 : 0), d == D_MEADOW || d == D_MOUNTAIN ? 6 : 4,
                             d == D_SKYLINE ? C(250, 150, 110) : d == D_MEADOW ? C(255, 244, 170) : d == D_MOUNTAIN ? C(255, 196, 120) : C(244, 238, 210));
    if (d == D_ARASAKA) { /* the tower crown up close, the city lights far below */
        for (int i = 0; i < w; i++) for (int j = h - 12; j < h; j++) if (hash3(i, j, 31) % 4 == 0) pset(x + i, y + j, hash3(i, j, 32) % 3 ? C(255, 150, 120) : C(150, 210, 255));
        arasakaTower(x + w / 2, y + 22, y + h - 4, 40, C(24, 22, 30), 0);
        fillRect(x, y + h - 4, w, 4, C(20, 20, 26));
        fillRect(x, y + h - 4, w, 1, C(230, 34, 50));
        if (locked) for (int j = 0; j < h; j++) for (int i = 0; i < w; i++) pdark(x + i, y + j, 90);
        return;
    }
    if (d == D_MOON) { /* a huge Earth over the white dust, two small figures sitting below it */
        earth(x + w / 2, y + 31, 24);
        for (int i = 0; i < w; i++)
            for (int j = y + h - 10; j < y + h; j++) pset(x + i, j, j == y + h - 10 ? C(214, 214, 224) : C(176, 176, 188));
        int cx = x + w / 2;
        fillRect(cx - 12, y + h - 16, 6, 6, C(245, 212, 36)); fillCircle(cx - 9, y + h - 18, 2, C(36, 30, 28));
        fillRect(cx + 6, y + h - 15, 5, 5, C(234, 238, 246)); fillCircle(cx + 8, y + h - 17, 2, C(214, 218, 238));
        if (locked) for (int j = 0; j < h; j++) for (int i = 0; i < w; i++) pdark(x + i, y + j, 90);
        return;
    }
    if (d == D_ORBIT) { /* no picture in this version: just the curve of the Earth */
        orbitEarth(x + w * 3 / 4, y - h / 3, h * 5 / 4, x, y, x + w, y + h);
        if (locked) for (int j = 0; j < h; j++) for (int i = 0; i < w; i++) pdark(x + i, y + j, 90);
        return;
    }
    if (d == D_NIGHTCITY) arasakaTower(x + w * 7 / 10, y + 10, y + h, 12, C(70, 40, 80), 0);
    if (IS_NATURE(d)) { /* outdoors: ridges, a peak, trees, the ground */
        if (d == D_STORM) for (int k = 0; k < 6; k++) fillCircle(x + (int)(hash3(k, 5, 5) % (unsigned)w), y + 4, 7, C(46, 50, 60));
        if (d == D_MEADOW) for (int k = 0; k < 3; k++) { int cx = x + (int)(hash3(k, 6, 6) % (unsigned)w); fillCircle(cx, y + 14, 4, C_WHITE); fillCircle(cx + 5, y + 13, 5, C_WHITE); }
        for (int i = 0; i < w; i++) {
            int top = y + h - 14 - (int)((0.5f + 0.3f * sinf(i * 0.09f + d) + 0.2f * sinf(i * 0.23f)) * (h - 30));
            if (d == D_MOUNTAIN) { int pk = y + 8 + (i < w / 2 ? w / 2 - i : i - w / 2) * 3 / 2; if (pk < top) top = pk; }
            for (int j = top; j < y + h; j++) {
                unsigned c = dd->farCol;
                if (d == D_MOUNTAIN && j - top < 6 && top < y + h - 30) c = C(240, 244, 252);
                pset(x + i, j, c);
            }
        }
        for (int i = 2; i < w; i += 7 + (int)(hash3(i, d, 8) % 6)) { /* treeline */
            int th = 8 + (int)(hash3(i, d, 9) % 12);
            for (int j = 0; j < th; j++) fillRect(x + i - j / 3, y + h - 4 - th + j, j / 3 * 2 + 1, 1, d == D_MEADOW ? C(60, 150, 60) : d == D_STORM ? C(30, 30, 34) : dd->midCol);
            if (d == D_MOUNTAIN) pset(x + i, y + h - 4 - th, C(240, 244, 252));
        }
        unsigned gc = d == D_FOREST ? C(60, 120, 50) : d == D_MEADOW ? C(110, 200, 80) : d == D_STORM ? C(76, 66, 54) : C(244, 248, 255);
        fillRect(x, y + h - 4, w, 4, gc);
        if (d == D_STORM) line(x + w / 3, y, x + w / 3 + 6, y + 18, C(230, 240, 255));
        if (locked)
            for (int j = 0; j < h; j++)
                for (int i = 0; i < w; i++) pdark(x + i, y + j, 90);
        return;
    }
    for (int bx = 0; bx < w;) {
        int bw = 5 + (int)(hash3(bx, d, 3) % 9), bh = 12 + (int)(hash3(bx, d, 5) % (unsigned)(h - 20));
        if (bx + bw > w) bw = w - bx;
        fillRect(x + bx, y + h - bh, bw, bh, dd->midCol);
        for (int wy = y + h - bh + 2; wy < y + h - 4; wy += 3)
            for (int wx = x + bx + 1; wx < x + bx + bw - 1; wx += 2)
                if (hash3(wx, wy, d) % 3 == 0) pset(wx, wy, hash3(wx, wy, 9) % 4 ? C(255, 210, 120) : C(150, 220, 255));
        bx += bw + 1;
    }
    if (d == D_CITYCENTER) /* neon strips and a hologram */
        for (int k = 0; k < 8; k++) fillRect(x + (int)(hash3(k, 3, 3) % (unsigned)(w - 20)), y + 18 + (int)(hash3(k, 4, 4) % 30), 8 + (int)(hash3(k, 5, 5) % 12), 1, k & 1 ? dd->neonA : dd->neonB);
    if (d == D_CITYCENTER) for (int j = 0; j < 12; j++) for (int i = 0; i < 30; i++) pmix(x + 8 + i, y + 6 + j, dd->neonA, (j == 0 || j == 11 || i == 0 || i == 29) ? 0.8f : 0.25f);
    fillRect(x, y + h - 4, w, 4, scalec(dd->midCol, 90));
    fillRect(x, y + h - 4, w, 1, dd->neonA);
    if (locked)
        for (int j = 0; j < h; j++)
            for (int i = 0; i < w; i++) pdark(x + i, y + j, 90);
}

void coinsCorner(void)
{
    char s[16];
    sfmt(s, sizeof s, "%d", coins);
    text(FW - 6, 6, s, C(255, 214, 80), 2, AL_RIGHT);
    drawCoin(FW - 14 - textWidth(s, 2), 10);
}

void backdrop(int dimK)
{
    px = FW / 2.f + sinf(gt * 0.2f) * 80; /* slow parallax drift */
    animateLayers();
    composite(dimK);
    target(fb, FW, FH);
    ambienceBack();
    ambienceFront();
}
