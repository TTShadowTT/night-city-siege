/* world_layers.c - Builds the far, middle and front parallax layers of a district. */
#include "core.h"

static void farTower(int cx, int top, unsigned col)
{
    arasakaTower(cx, top, FH, 30, scalec(col, 150), 1);
    addBeacon(cx, top - 16, 0);
}

static void buildFar(void)
{
    target(farL, FARW, FH);
    for (int y = 0; y < FH; y++)
        for (int x = 0; x < FARW; x++) {
            float tt = clampf(y / (float)GROUND_Y, 0, 1) * 8;
            int b = (int)tt;
            if ((tt - b) * 16 > bayer[y & 3][x & 3]) b++;
            pset(x, y, mixc(D->skyTop, D->skyBot, b / 8.0f));
        }
    nStars = 0;
    int starN = curMap == D_MEADOW || curMap == D_STORM ? 0 : curMap == D_MOUNTAIN ? 70 : curMap == D_NIGHTCITY || curMap == D_CITYCENTER ? 25 : curMap == D_MOON || curMap == D_ORBIT ? 260 : 170; /* none by day, under storm clouds or city glow */
    for (int i = 0; i < starN; i++) {
        int x = lr(FARW), y = lr(curMap == D_MOUNTAIN ? 80 : 170);
        unsigned c = lr(4) == 0 ? C(255, 255, 230) : C(150, 150, 190);
        pset(x, y, c);
        if (nStars < 60 && lr(3) == 0) { starX[nStars] = (short)x; starY[nStars] = (short)y; nStars++; }
    }
    if (D->moonR > 0) {
        int mx0 = D->moonX + 16, my0 = D->moonY, r = D->moonR;
        unsigned mc = curMap == D_SKYLINE ? C(250, 150, 110) : curMap == D_MEADOW ? C(255, 244, 170) : curMap == D_MOUNTAIN ? C(255, 196, 120) : C(244, 238, 210);
        for (int j = -r - 12; j <= r + 12; j++)
            for (int i = -r - 12; i <= r + 12; i++) {
                float d = sqrtf((float)(i * i + j * j));
                if (d <= r) {
                    int sun = curMap == D_MEADOW || curMap == D_MOUNTAIN;
                    unsigned c = (i + j > r / 2 && !sun) ? scalec(mc, 210) : mc;
                    pset(mx0 + i, my0 + j, c);
                } else if (d <= r + 12) {
                    float g = (1 - (d - r) / 12) * 0.3f;
                    if (g * 16 > bayer[(my0 + j) & 3][(mx0 + i) & 3] * 0.6f) pmix(mx0 + i, my0 + j, mc, g);
                }
            }
        if (!IS_NATURE(curMap) || curMap == D_FOREST) /* craters on a moon, not on a sun */
            for (int k = 0; k < 4; k++) fillCircle(mx0 - r / 3 + lr(r), my0 - r / 3 + lr(r), 1 + lr(3), scalec(mc, 200));
    }
    if (hasPic(curMap)) { /* the picture itself, as pixel art; the edges repeat where the parallax looks past it */
        int k = mapPic(curMap);
        buildPicture(k);
        nStars = 0; nBeacons = 0;
        for (int y = 0; y < FH; y++)
            for (int x = 0; x < FARW; x++) pset(x, y, picCol(k, (x - (FARW - FW) / 2) / 2, y / 2));
        return;
    }
    if (IS_NATURE(curMap)) { natureFar(); return; }
    if (curMap == D_MOON) { moonFar(); return; }
    if (curMap == D_ORBIT) { orbitEarth(FARW * 7 / 10, 30, 250, 0, 0, FARW, FH); return; }
    /* distant skyline: Night City has megabuildings, from the top of Arasaka Tower the city lies far below */
    int nc = curMap == D_NIGHTCITY || curMap == D_CITYCENTER, high = curMap == D_ARASAKA;
    if (curMap == D_NIGHTCITY) farTower(FARW * 7 / 10, 18, D->farCol);
    for (int x = -4; x < FARW;) {
        int w = nc ? 14 + lr(40) : 10 + lr(26), h = high ? 12 + lr(50) : nc ? 40 + lr(120) : 30 + lr(95), top = GROUND_Y - 6 - h;
        if (high) top += 10;
        unsigned bc = scalec(D->farCol, 220 + lr(60));
        fillRect(x, top, w, FH - top, bc);
        int roof = lr(4);
        if (roof == 1) fillRect(x + w / 4, top - 5 - lr(8), w / 2, 14, bc);
        if (roof == 2) for (int k = 0; k < w / 2; k++) fillRect(x + k, top - k / 2, w - 2 * k, 1, bc);
        int litPct = high ? 32 : nc ? 18 : 11;
        unsigned warm = nc ? C(255, 220, 90) : high ? C(255, 150, 120) : C(255, 210, 130), cool = nc ? C(255, 90, 180) : C(150, 210, 255);
        for (int yy = top + 3; yy < GROUND_Y + (high ? 10 : 0); yy += high ? 3 : 4)
            for (int xx = x + 2; xx < x + w - 2; xx += high ? 2 : 3)
                if (lr(100) < litPct) pset(xx, yy, mixc(bc, lr(3) ? warm : cool, 0.45f));
        if (nc && lr(4) == 0 && h > 60) { /* a holo billboard on the side of a megabuilding */
            int bw = w - 4 < 6 ? w - 2 : w - 4, by = top + 6 + lr(20);
            unsigned hc = lr(2) ? D->neonA : lr(2) ? D->neonB : C(255, 60, 170);
            fillRect(x + 2, by, bw, 8 + lr(8), scalec(hc, 120));
            fillRect(x + 2, by, bw, 1, hc);
        }
        if (lr(10) < 3) { int ax = x + w / 2, len = 5 + lr(10); line(ax, top - 1, ax, top - len, bc); addBeacon(ax, top - len - 1, 0); }
        x += w + lr(4);
    }
    /* haze toward the street */
    for (int y = GROUND_Y - 110; y < FH; y++) {
        float a = clampf((y - (GROUND_Y - 110)) / 110.0f, 0, 1) * 0.4f;
        for (int x = 0; x < FARW; x++)
            if (a * 16 > bayer[y & 3][x & 3] * 0.5f) pmix(x, y, D->haze, a);
    }
}

static void drawCrane(int x, int baseY, int h, int boom, unsigned c)
{
    for (int y = baseY - h; y < baseY; y++) {
        pset(x, y, c); pset(x + 5, y, c);
        if (((y - baseY) % 6) == 0) line(x, y, x + 5, y + 5, c);
    }
    fillRect(x - boom / 3, baseY - h - 2, boom, 2, c);
    line(x + 2, baseY - h - 12, x - boom / 3, baseY - h - 2, c);
    line(x + 2, baseY - h - 12, x + boom * 2 / 3, baseY - h - 2, c);
    line(x + boom / 2, baseY - h, x + boom / 2, baseY - h + 20 + lr(30), c);
    addBeacon(x + 2, baseY - h - 13, 1);
}

/* David, seen from behind, sitting: yellow jacket with a black band, arms back on the ground, messy dark hair */
static void sittingDavid(int cx, int by)
{
    unsigned y0 = C(245, 212, 36), y1 = C(200, 168, 22), dark = C(28, 28, 34);
    fillRect(cx - 12, by - 4, 24, 4, C(34, 34, 40));                       /* seat and legs, stretched away from us */
    thickLine(cx - 11, by - 27, cx - 16, by - 1, y0, 4);                  /* arms leaning back */
    thickLine(cx + 11, by - 27, cx + 16, by - 1, y1, 4);
    fillRect(cx - 17, by - 4, 3, 2, dark); fillRect(cx + 14, by - 4, 3, 2, dark);
    pset(cx - 16, by - 1, C(220, 170, 130)); pset(cx + 16, by - 1, C(200, 150, 112));
    for (int y = by - 30; y < by - 3; y++) {                              /* the jacket */
        int t = y - (by - 30), half = t < 3 ? 8 + t : 10 + t / 8;
        for (int x = cx - half; x <= cx + half; x++) pset(x, y, x > cx + half / 3 ? y1 : y0);
    }
    fillRect(cx - 11, by - 12, 23, 3, dark);                              /* the black band */
    fillRect(cx - 11, by - 13, 23, 1, C(240, 240, 230));
    fillRect(cx - 1, by - 28, 2, 15, y1);                                 /* back seam */
    fillRect(cx - 6, by - 33, 12, 4, C(225, 190, 24));                    /* collar */
    fillCircle(cx, by - 38, 6, C(36, 30, 28));                            /* hair */
    for (int k = 0; k < 6; k++) line(cx - 5 + k * 2, by - 43, cx - 7 + k * 3, by - 46 + (k & 1), C(36, 30, 28));
    pset(cx - 3, by - 41, C(70, 62, 58)); pset(cx - 2, by - 42, C(70, 62, 58));
}

/* Lucy, seen from behind, sitting: pale silver bob, white jacket with a dark panel, navy suit */
static void sittingLucy(int cx, int by)
{
    unsigned w0 = C(234, 238, 246), w1 = C(186, 192, 210), navy = C(26, 30, 62), cyan = C(90, 220, 255);
    fillRect(cx - 10, by - 5, 20, 5, navy);
    fillRect(cx - 10, by - 5, 20, 1, cyan);
    thickLine(cx - 8, by - 23, cx - 13, by - 1, w0, 3);
    thickLine(cx + 8, by - 23, cx + 13, by - 1, w1, 3);
    line(cx - 9, by - 18, cx - 12, by - 6, navy); line(cx + 9, by - 18, cx + 12, by - 6, navy);
    pset(cx - 13, by - 1, C(240, 214, 200)); pset(cx + 13, by - 1, C(220, 194, 180));
    for (int y = by - 26; y < by - 4; y++) {
        int t = y - (by - 26), half = t < 3 ? 6 + t : 8 + t / 9;
        for (int x = cx - half; x <= cx + half; x++) pset(x, y, x > cx + half / 3 ? w1 : w0);
    }
    fillRect(cx - 2, by - 20, 5, 15, navy);                               /* the dark back panel */
    fillRect(cx - 5, by - 16, 2, 4, navy); fillRect(cx + 4, by - 16, 2, 4, navy);
    fillRect(cx - 4, by - 27, 9, 2, cyan);                                /* glowing collar */
    fillCircle(cx, by - 33, 6, C(214, 218, 238));                         /* the bob */
    fillRect(cx - 6, by - 33, 13, 5, C(214, 218, 238));
    for (int x = cx - 6; x <= cx + 6; x++) pset(x, by - 28, (x & 1) ? C(150, 200, 255) : C(190, 170, 240));
    for (int y = by - 37; y < by - 28; y++) pset(cx + 5, y, C(170, 176, 214));
    pset(cx - 3, by - 37, C_WHITE); pset(cx - 2, by - 38, C_WHITE);
}

/* the moon: David and Lucy sitting on the white dust, looking up at the Earth */
static void moonMid(void)
{
    for (int y = 240; y < FH; y++)
        for (int x = 0; x < MIDW; x++) {
            unsigned h = hash3(x, y, 51);
            pset(x, y, y == 240 ? C(214, 214, 224) : (h % 11) == 0 ? C(150, 150, 162) : C(186, 186, 198));
        }
    for (int k = 0; k < 9; k++) { /* pale rocks */
        int bx = lr(MIDW), br = 2 + lr(5);
        fillCircle(bx, 243, br, C(130, 130, 142));
        fillCircle(bx - 1, 242, br - 1, C(226, 226, 234));
    }
    int mx = MIDW / 2;
    for (int i = -14; i <= 14; i++) { pset(mx - 38 + i, 247, C(120, 120, 132)); pset(mx + 38 + i * 3 / 4, 247, C(120, 120, 132)); }
    sittingDavid(mx - 38, 247);
    sittingLucy(mx + 38, 247);
}

/* on top of Arasaka Tower: the crown rises right behind the fight, masts and railings on the roof */
static void arasakaMid(void)
{
    int cx = MIDW / 2;
    for (int k = 0; k < 2; k++) { /* antenna masts with blinking tips */
        int ax = k ? MIDW - 46 : 46, top = 40 + k * 14;
        fillRect(ax - 1, top, 3, GROUND_Y + 10 - top, C(34, 32, 42));
        for (int y = top + 8; y < GROUND_Y; y += 10) line(ax - 5, y, ax + 5, y + 6, C(34, 32, 42));
        fillRect(ax - 8, top + 20, 17, 2, C(40, 38, 50));
        addBeacon(ax, top - 1, 1);
    }
    arasakaTower(cx, 50, FH, 120, C(24, 22, 30), 1);
    for (int k = 0; k < 2 && nSigns < 24; k++) { /* the name down both sides of the crown */
        Sign *s = &signs[nSigns++];
        sfmt(s->txt, sizeof s->txt, "%s", "ARASAKA");
        s->vertical = 1;
        s->w = 7; s->h = (short)(7 * 6 + 3);
        s->x = (short)(k ? cx + 36 : cx - 43); s->y = (short)90;
        s->col = D->neonA;
        fillRect(s->x, s->y, s->w, s->h, C(14, 8, 12));
        drawSignTextTo(s, 1);
    }
    for (int x = 0; x < MIDW; x += 9) fillRect(x, GROUND_Y - 10, 1, 12, C(70, 70, 86)); /* glass railing */
    fillRect(0, GROUND_Y - 11, MIDW, 1, C(150, 150, 170));
}

static void buildMid(void)
{
    static const char *cityWords[] = { "BAR", "HOTEL", "24H", "NEON", "RAMEN", "CLUB", "OPEN", "CAFE", "PIZZA", "MOTEL", "SUSHI", "LIVE" };
    static const char *ncWords[] = { "LIZZIES", "NCPD", "RIPPER", "BD", "CHROME", "TRAUMA", "NOODLE", "DOLLS", "AFTRLFE", "KIROSHI", "VENDING", "PACHINK" };
    static const char *ccWords[] = { "KARAOKE", "SAKE", "RAMEN", "ARCADE", "TATTOO", "SUSHI", "PACHINK", "BD", "CHROME", "KABUKI", "NOODLE", "LIZZIES" };
    const char *const *words = curMap == D_NIGHTCITY ? ncWords : curMap == D_CITYCENTER ? ccWords : cityWords;
    int cc = curMap == D_CITYCENTER;
    target(midL, MIDW, FH);
    for (int i = 0; i < MIDW * FH; i++) midL[i] = 0;
    nWindows = nSigns = 0;
    if (IS_NATURE(curMap)) { natureMid(); return; }
    if (curMap == D_ARASAKA) { arasakaMid(); return; }
    if (hasPic(curMap) || curMap == D_ORBIT) return; /* everything is in the picture */
    if (curMap == D_MOON) { moonMid(); return; }
    for (int x = -10 + lr(10); x < MIDW;) {
        int w = 26 + lr(40), h = 60 + lr(110), top = GROUND_Y + 20 - h;
        unsigned bc = scalec(D->midCol, 220 + lr(60));
        fillRect(x, top, w, FH - top, bc);
        fillRect(x, top, 1, FH - top, scalec(bc, 140));
        fillRect(x + w - 2, top, 2, FH - top, scalec(bc, 70));
        fillRect(x, top, w, 1, scalec(bc, 160));
        int wx0 = x + 3 + lr(2);
        for (int wy = top + 4; wy < GROUND_Y + 4; wy += 7)
            for (int wx = wx0; wx + 2 < x + w - 2; wx += 5) {
                int lit = lr(100) < 32;
                unsigned on = lr(10) < 7 ? C(255, 214, 120) : C(150, 220, 255), off = scalec(bc, 70);
                fillRect(wx, wy, 2, 3, lit ? on : off);
                if (nWindows < 260 && lr(4) == 0) {
                    Window *wn = &windows[nWindows++];
                    wn->x = (short)wx; wn->y = (short)wy; wn->on = on; wn->off = off; wn->lit = (unsigned char)lit;
                }
            }
        int prop = cc ? 5 + lr(3) : lr(10); /* the city center has a sign on every roof */
        if (prop < 3) { /* water tower */
            int tx = x + 4 + lr(w > 14 ? w - 14 : 1);
            fillRect(tx, top - 10, 9, 7, scalec(bc, 120));
            fillRect(tx + 1, top - 3, 1, 3, scalec(bc, 90)); fillRect(tx + 7, top - 3, 1, 3, scalec(bc, 90));
            fillRect(tx - 1, top - 11, 11, 1, scalec(bc, 150));
        } else if (prop < 5) {
            int ax = x + w / 2, len = 8 + lr(14);
            line(ax, top - 1, ax, top - len, scalec(bc, 150));
            addBeacon(ax, top - len - 1, 1);
        } else if (prop < 8 && nSigns < 24) { /* rooftop sign */
            Sign *s = &signs[nSigns++];
            const char *wd = words[lr(12)];
            sfmt(s->txt, sizeof s->txt, "%s", wd);
            s->vertical = 0;
            s->w = (short)(textWidth(wd, 1) + 4); s->h = 9;
            s->x = (short)(x + (w - s->w) / 2); s->y = (short)(top - s->h - 3);
            s->col = lr(2) ? D->neonA : D->neonB;
            fillRect(s->x + 2, s->y + s->h, 1, 3, scalec(bc, 120)); fillRect(s->x + s->w - 3, s->y + s->h, 1, 3, scalec(bc, 120));
            fillRect(s->x, s->y, s->w, s->h, C(14, 12, 20));
            for (int i = 0; i < s->w; i++) { pset(s->x + i, s->y, scalec(s->col, 160)); pset(s->x + i, s->y + s->h - 1, scalec(s->col, 160)); }
            drawSignTextTo(s, 1);
        }
        if (nSigns < 24 && lr(100) < (cc ? 85 : curMap == D_NEON || curMap == D_NIGHTCITY ? 55 : 15) && h > 70) { /* vertical sign on the facade */
            Sign *s = &signs[nSigns++];
            static const char *vw[] = { "BAR", "HOT", "INN", "CLUB", "24H", "EAT" };
            static const char *ncv[] = { "BAR", "CHROME", "DOLLS", "RIPPER", "BD", "NCPD" };
            static const char *ccv[] = { "SAKE", "HOTEL", "ARCADE", "BAR", "BD", "RAMEN" };
            sfmt(s->txt, sizeof s->txt, "%s", curMap == D_NIGHTCITY ? ncv[lr(6)] : cc ? ccv[lr(6)] : vw[lr(6)]);
            s->vertical = 1;
            s->w = 7; s->h = (short)(strlen(s->txt) * 6 + 3);
            s->x = (short)(x + 2); s->y = (short)(top + 8);
            s->col = lr(2) ? D->neonA : D->neonB;
            fillRect(s->x, s->y, s->w, s->h, C(14, 12, 20));
            for (int j = 0; j < s->h; j++) { pset(s->x, s->y + j, scalec(s->col, 150)); pset(s->x + s->w - 1, s->y + j, scalec(s->col, 150)); }
            drawSignTextTo(s, 1);
        }
        x += w + 3 + lr(12);
    }
    if (curMap == D_HARBOR) { drawCrane(60, GROUND_Y + 10, 150, 110, C(20, 30, 40)); drawCrane(400, GROUND_Y + 10, 170, 130, C(20, 30, 40)); }
    if (curMap == D_NIGHTCITY) /* an elevated train line across the street */
        for (int x = 0; x < MIDW; x++) {
            fillRect(x, 170, 1, 5, (x % 24) < 2 ? C(20, 18, 28) : C(34, 30, 46));
            if (x % 48 == 10) fillRect(x, 175, 5, GROUND_Y + 10 - 175, C(26, 24, 36));
            pset(x, 175, (x / 6) & 1 ? D->neonA : C(80, 70, 30));
        }
    if (cc) { /* strings of coloured lights sagging across the street, and neon tubes along the facades */
        static const unsigned bulbs[4] = { C(255, 50, 180), C(60, 255, 230), C(255, 220, 60), C(255, 120, 60) };
        for (int k = 0; k < 7; k++) {
            int x0 = lr(MIDW), span = 60 + lr(70), y0 = 80 + lr(110);
            for (int i = 0; i <= span; i++) {
                float t = i / (float)span;
                int y = y0 + (int)(4 * t * (1 - t) * (10 + span / 8));
                pset(x0 + i, y, C(30, 26, 40));
                if (i % 5 == 2) pset(x0 + i, y + 1, bulbs[(i / 5 + k) & 3]);
            }
        }
        for (int k = 0; k < 10; k++) {
            int y = 70 + lr(150), x0 = lr(MIDW), len = 20 + lr(50);
            fillRect(x0, y, len, 1, lr(2) ? D->neonA : D->neonB);
        }
    }
    if (curMap == D_SKYLINE) {
        drawCrane(120, GROUND_Y + 10, 180, 140, C(30, 16, 24));
        for (int gy = 70; gy < GROUND_Y; gy += 14) fillRect(330, gy, 90, 2, C(40, 22, 30));
        for (int gx = 330; gx <= 420; gx += 18) fillRect(gx, 70, 2, GROUND_Y - 70, C(40, 22, 30));
    }
}

static void drawPlatform(const Plat *p)
{
    int x0 = (int)p->x, y0 = (int)p->y, w = (int)p->w;
    for (int i = 0; i < w; i++) {
        int x = x0 + i, edge = i == 0 || i == w - 1;
        for (int j = 0; j < 6; j++) {
            unsigned c = 0;
            switch (curMap) {
            case D_DOWNTOWN:
                c = j == 0 ? C(176, 172, 190) : j == 5 ? C(56, 52, 70) : C(110, 106, 126);
                if (j >= 2 && j <= 3 && (x % 16) == 0) c = C(80, 76, 96);
                break;
            case D_NEON:
                c = j == 0 ? D->neonA : j == 1 ? C(60, 56, 80) : j == 5 ? scalec(D->neonB, 200) : (((x + j) & 1) ? C(52, 48, 70) : C(30, 28, 40));
                break;
            case D_HARBOR:
                if (j <= 1) c = ((x % 9) == 0) ? C(70, 50, 32) : (j == 0 ? C(152, 110, 70) : C(128, 92, 58));
                else if (j <= 3) c = ((x % 12) < 3) ? C(96, 68, 44) : 0;
                else c = j == 4 ? C(140, 100, 64) : C(100, 72, 46);
                break;
            case D_FOREST: /* mossy log */
                c = j == 0 ? ((x % 5) ? C(90, 150, 60) : C(60, 110, 40)) : j == 5 ? C(50, 32, 22) : ((x + j * 3) % 11 == 0 ? C(70, 46, 30) : C(110, 74, 46));
                break;
            case D_MEADOW: /* grass on dirt */
                c = j == 0 ? C(120, 210, 90) : j == 1 ? ((x % 3) ? C(80, 170, 70) : C(120, 210, 90)) : j == 5 ? C(110, 76, 46) : (hash3(x, j, 5) % 9 == 0 ? C(150, 140, 130) : C(150, 104, 64));
                break;
            case D_STORM: /* wet slate */
                c = j == 0 ? C(150, 160, 176) : j == 5 ? C(40, 42, 50) : ((x % 13) == 0 || hash3(x, j, 6) % 17 == 0 ? C(50, 54, 62) : C(86, 92, 104));
                break;
            case D_MOUNTAIN: /* snow on rock */
                c = j <= 1 ? (j == 0 ? C(250, 252, 255) : C(214, 224, 240)) : j == 5 ? C(56, 52, 70) : (hash3(x, j, 7) % 7 == 0 ? C(70, 66, 88) : C(104, 98, 124));
                break;
            case D_NIGHTCITY: /* hazard-striped steel with a cyan underglow */
                c = j == 0 ? C(255, 220, 40) : j == 1 ? ((((x + j) / 3) & 1) ? C(255, 200, 30) : C(24, 22, 30)) : j == 5 ? D->neonB
                    : ((x % 14) == 0 ? C(60, 230, 255) : C(44, 40, 58));
                break;
            case D_ARASAKA: /* black corporate glass, red trim */
                c = j == 0 ? C(200, 206, 222) : j == 1 ? C(230, 34, 50) : j == 5 ? C(10, 10, 14) : ((x % 20) == 0 ? C(56, 54, 66) : C(26, 26, 32));
                break;
            case D_CITYCENTER: /* neon tube on top, glowing dots, cyan underside */
                c = j == 0 ? D->neonA : j == 1 ? C(40, 30, 56) : j == 5 ? D->neonB : ((x % 8) == 4 && j == 3 ? ((x / 8) & 1 ? C(255, 220, 60) : D->neonB) : C(30, 24, 44));
                break;
            case D_MOON: /* grey moon rock on a steel frame */
                c = j == 0 ? C(200, 200, 210) : j == 1 ? C(160, 160, 172) : j == 5 ? C(70, 74, 90) : (hash3(x, j, 9) % 6 == 0 ? C(96, 96, 108) : C(130, 130, 142));
                break;
            case D_ORBIT: /* a solar panel strip: cyan edge, blue cells on a steel frame */
                c = j == 0 ? C(150, 240, 255) : j == 1 || j == 5 ? C(60, 66, 84) : ((x % 6) == 0 ? C(90, 130, 200) : (j == 3 ? C(36, 70, 140) : C(28, 54, 116)));
                break;
            case D_LAUNCH: /* a dark roof ledge with a green neon edge, like David's jacket */
                c = j == 0 ? C(130, 255, 100) : j == 1 ? C(40, 70, 50) : j == 5 ? C(14, 18, 30) : ((x % 12) == 0 ? C(44, 52, 76) : C(26, 32, 50));
                break;
            case D_FREEFALL: /* a glowing gold window ledge */
                c = j == 0 ? C(255, 240, 170) : j == 1 ? C(230, 180, 70) : j == 5 ? C(70, 46, 14) : ((x % 7) == 0 ? C(120, 84, 26) : C(196, 146, 48));
                break;
            default:
                if (j == 0) c = C(232, 120, 64);
                else if (j == 1) c = (x % 6 == 0) ? C(120, 50, 24) : C(200, 84, 40);
                else if (j == 5) c = C(160, 64, 30);
                else c = (((x - j) % 6 == 0) || ((x + j) % 6 == 0)) ? C(186, 76, 36) : 0;
                break;
            }
            if (edge && c) c = C_OUT;
            if (c) pset(x, y0 + j, c);
        }
    }
}

static void drawRope(const Rope *r)
{
    int x = (int)r->x;
    int vine = curMap == D_FOREST || curMap == D_MEADOW;
    int steel = curMap == D_HARBOR || curMap >= D_NIGHTCITY;
    unsigned a = steel ? C(160, 164, 176) : vine ? C(90, 170, 70) : C(160, 118, 72);
    unsigned b = steel ? C(96, 100, 112) : vine ? C(50, 110, 40) : C(104, 74, 44);
    fillRect(x - 2, (int)r->y0, 5, 2, C(60, 60, 72));
    for (int y = (int)r->y0 + 2; y < (int)r->y1; y++) {
        pset(x, y, ((y >> 1) & 1) ? a : b);
        pset(x + 1, y, ((y >> 1) & 1) ? b : scalec(b, 180));
    }
    fillRect(x - 1, (int)r->y1 - 2, 3, 2, b);
}

static void buildFore(void)
{
    target(foreL, FW, FH);
    for (int i = 0; i < FW * FH; i++) foreL[i] = 0;
    for (int y = GROUND_Y; y < FH; y++)
        for (int x = 0; x < FW; x++) {
            int j = y - GROUND_Y;
            unsigned c;
            unsigned h = hash3(x, y, curMap);
            switch (curMap) {
            case D_DOWNTOWN:
                c = j == 0 ? C(150, 146, 164) : j == 1 ? C(120, 116, 134) : ((x % 24) == 0 ? C(56, 52, 68) : C(76, 72, 90));
                if (j > 2 && (h & 31) == 0) c = C(96, 92, 110);
                break;
            case D_NEON:
                c = j < 3 ? (j == 0 ? C(96, 92, 120) : C(70, 66, 90)) : C(30, 26, 40);
                if (j == 11 && (x / 12) % 2 == 0) c = C(200, 170, 60);
                if (j > 3 && (h % 23) == 0) c = mixc(c, (h & 64) ? D->neonA : D->neonB, 0.5f);
                break;
            case D_HARBOR:
                c = ((j % 5) == 4) ? C(50, 36, 26) : ((((x + (j / 5) * 13) % 28) == 0) ? C(60, 42, 28) : (((j / 5) & 1) ? C(104, 76, 50) : C(116, 84, 54)));
                break;
            case D_FOREST: /* forest floor: grass, dark soil, roots */
                c = j == 0 ? C(60, 120, 50) : j == 1 ? ((h & 1) ? C(40, 90, 40) : C(60, 120, 50)) : ((h % 13) == 0 ? C(70, 50, 34) : C(40, 30, 24));
                break;
            case D_MEADOW:
                c = j < 2 ? ((h & 3) ? C(110, 200, 80) : C(80, 170, 60)) : j == 2 ? C(70, 140, 50) : ((h % 19) == 0 ? C(160, 150, 140) : C(140, 96, 58));
                if (j == 0 && (h % 37) == 0) c = (h & 64) ? C(255, 240, 120) : C(255, 140, 180); /* flowers */
                break;
            case D_STORM: /* mud with puddles */
                c = j == 0 ? C(70, 64, 56) : ((x / 30 + j / 6) % 5 == 0 && j < 9 ? C(70, 84, 104) : ((h % 7) == 0 ? C(56, 48, 40) : C(76, 66, 54)));
                break;
            case D_MOUNTAIN:
                c = j < 4 ? ((h % 9) == 0 ? C(214, 224, 240) : C(244, 248, 255)) : ((h % 11) == 0 ? C(70, 66, 88) : C(96, 92, 116));
                break;
            case D_NIGHTCITY: /* wet street: yellow curb, lane dashes, neon in the puddles */
                c = j == 0 ? C(255, 220, 40) : j == 1 ? C(90, 84, 70) : C(36, 32, 46);
                if (j == 10 && (x / 14) % 2 == 0) c = C(230, 230, 240);
                if (j > 2 && (h % 19) == 0) c = mixc(c, (h & 64) ? C(255, 60, 170) : D->neonB, 0.55f);
                break;
            case D_CITYCENTER: /* wet tiles, a zebra crossing, pink and cyan reflections */
                c = j == 0 ? C(120, 100, 150) : j == 1 ? C(70, 56, 90) : ((x + j) % 16 == 0 ? C(24, 20, 34) : C(34, 28, 48));
                if (j >= 4 && j <= 14 && x >= 200 && x < 280 && (x / 8) % 2 == 0) c = C(200, 196, 214);
                if (j > 2 && (h % 9) == 0) c = mixc(c, (h & 64) ? D->neonA : D->neonB, 0.6f);
                break;
            case D_MOON: /* grey dust with small craters and boot prints */
                c = j == 0 ? C(220, 220, 230) : ((h % 13) == 0 ? C(124, 124, 136) : (h % 7) == 0 ? C(200, 200, 210) : C(170, 170, 182));
                if (j >= 5 && j <= 7 && (x % 40) < 12 && ((x % 40) & 4) == 0) c = C(130, 130, 142);
                break;
            case D_LAUNCH: /* the rooftop: green edge light, dark tar, a purple stripe like the edge in the picture */
                c = j == 0 ? C(130, 255, 100) : j == 1 ? C(30, 40, 46) : ((h % 23) == 0 ? C(30, 34, 50) : C(16, 18, 28));
                if (j == 6 && (x % 24) < 12) c = C(200, 90, 255);
                break;
            case D_FREEFALL: /* the top of the megabuilding: gold plates and lit window slits */
                c = j == 0 ? C(255, 236, 160) : j == 1 ? C(150, 104, 30) : ((x % 20) == 0 || j == 10 ? C(60, 40, 12) : (h % 13) == 0 ? C(120, 84, 24) : C(90, 62, 18));
                if (j >= 4 && j <= 6 && (x % 10) < 6) c = C(255, 210, 100);
                break;
            case D_ORBIT: /* the satellite deck: a glowing edge, steel plates, purple and green running lights */
                c = j == 0 ? C(150, 240, 255) : j == 1 ? C(70, 80, 100) : ((x % 32) == 0 || j == 9 ? C(30, 34, 46) : (h % 17) == 0 ? C(62, 70, 88) : C(48, 54, 70));
                if (j == 5 && (x % 32) >= 14 && (x % 32) < 18) c = (x / 32) & 1 ? C(225, 90, 255) : C(100, 255, 120);
                break;
            case D_ARASAKA: /* polished black roof with red edge and helipad marks */
                c = j == 0 ? C(230, 34, 50) : j == 1 ? C(90, 20, 28) : ((h % 29) == 0 ? C(60, 22, 30) : C(20, 20, 26));
                if (j == 8 && (x / 10) % 3 == 0) c = C(220, 220, 230);
                break;
            default:
                c = j < 3 ? ((((x + j) / 4) & 1) ? C(240, 196, 40) : C(30, 28, 34)) : ((x % 10 == 0 && j % 6 == 4) ? C(130, 134, 146) : C(86, 90, 102));
                break;
            }
            pset(x, y, c);
        }
    for (int i = 0; i < D->nRope; i++) drawRope(&D->rope[i]);
    for (int i = 0; i < D->nPlat; i++) drawPlatform(&D->plat[i]);
}

void loadDistrict(int m)
{
    curMap = m;
    D = &DISTRICTS[m];
    gravK = m == D_MOON ? 0.6f : m == D_ORBIT ? 0.45f : m == D_FREEFALL ? 0.7f : 1;
    bossArmor = D->armor;
    bossArmorLight = D->armorLight;
    lrng = 0x9E3779B9u * (unsigned)(m + 1);
    nBeacons = 0;
    buildFar();
    buildMid();
    buildFore();
}
