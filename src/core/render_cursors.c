/* render_cursors.c - Every cursor (crosshair) skin. */
#include "core.h"

/* a little bitmap: # main colour, o outline, r accent, w white, anything else see-through; (hx, hy) is the hot spot */
static void cursorBits(const char *const *rows, int n, int hx, int hy, unsigned main, unsigned accent)
{
    for (int y = 0; y < n; y++)
        for (int x = 0; rows[y][x]; x++) {
            char ch = rows[y][x];
            if (ch == '#') curPx(x - hx, y - hy, main);
            else if (ch == 'o') curPx(x - hx, y - hy, C_OUT);
            else if (ch == 'r') curPx(x - hx, y - hy, accent);
            else if (ch == 'w') curPx(x - hx, y - hy, C_WHITE);
        }
}

/* the small "usable" cursors: set pixels with cmSet, then cmFlush draws them with a dark outline so they show on any background */
static unsigned cmCol[11 * 11];
static void cmSet(int i, int j, unsigned c) { cmCol[(j + 5) * 11 + i + 5] = c; } /* colours always have alpha set, so 0 means empty */
static void cmFlush(void)
{
    for (int j = -5; j <= 5; j++)
        for (int i = -5; i <= 5; i++) {
            if (cmCol[(j + 5) * 11 + i + 5]) continue;
            for (int n = 0; n < 9; n++) {
                int a = i + n % 3 - 1, b = j + n / 3 - 1;
                if (a >= -5 && a <= 5 && b >= -5 && b <= 5 && cmCol[(b + 5) * 11 + a + 5]) { curPx(i, j, C_OUT); break; }
            }
        }
    for (int j = -5; j <= 5; j++)
        for (int i = -5; i <= 5; i++)
            if (cmCol[(j + 5) * 11 + i + 5]) { curPx(i, j, cmCol[(j + 5) * 11 + i + 5]); cmCol[(j + 5) * 11 + i + 5] = 0; }
}

void drawCursor(int k, int x, int y, int scale)
{
    static const unsigned rainbow[6] = { C(255, 60, 60), C(255, 170, 40), C(255, 240, 60), C(80, 255, 120), C(60, 200, 255), C(200, 90, 255) };
    static const char *const arrow[11] = { "o", "oo", "owo", "owwo", "owwwo", "owwwwo", "owwwwwo", "owwwoooo", "owoowo", "oo.owo", "....oo" };
    static const char *const skull[7] = { ".#####.", "#######", "#rr#rr#", "#rr#rr#", "###.###", ".#####.", ".#.#.#." };
    static const char *const heart[6] = { ".##.##.", "#w#####", "#######", ".#####.", "..###..", "...#..." };
    static const char *const star[7] = { "...#...", "..###..", "#######", ".#####.", "..###..", ".##.##.", "##...##" };
    static const char *const sword[10] = { "w", ".w#", "..w#", "...w#", "....w#", ".....w#.r", "......wr", "......ro", ".....r..o", ".........r" };
    static const char *const flame[8] = { "...r...", "..rr...", "..r#r..", ".r##r.r", ".r#w#r.", "r##w##r", "r#www#r", ".r###r." };
    static const char *const flame2[8] = { "....r..", "...rr..", "..r#r..", "r.r##r.", ".r#w#r.", "r##w##r", "r#www#r", ".r###r." };
    static const char *const ghost[7] = { "..###..", ".#####.", "##o#o##", "##o#o##", "#######", "#######", "#.#.#.#" };
    static const char *const slime[6] = { "...#...", "..###..", ".#w###.", "##o#o##", "#######", ".#####." };
    static const char *const bolt[8] = { "....##", "...##.", "..##..", ".#####", "...##.", "..##..", ".##...", "##...." };
    static const char *const crown[6] = { "r..r..r", "#..#..#", "##.#.##", "#######", "#r#r#r#", "#######" };
    unsigned red = C(255, 80, 80);
    curX = x; curY = y; curS = scale;
    switch (k) {
    default: /* CLASSIC */
        for (int i = 2; i <= 4; i++) { curPx(-i, 0, C_WHITE); curPx(i, 0, C_WHITE); curPx(0, -i, C_WHITE); curPx(0, i, C_WHITE); }
        curPx(0, 0, red);
        break;
    case 1: /* DOT */
        for (int j = -2; j <= 2; j++) for (int i = -2; i <= 2; i++) if (i * i + j * j <= 5) curPx(i, j, C_OUT);
        for (int j = -1; j <= 1; j++) for (int i = -1; i <= 1; i++) curPx(i, j, C_WHITE);
        curPx(0, 0, red);
        break;
    case 2: /* RING */
        for (int j = -5; j <= 5; j++) for (int i = -5; i <= 5; i++) { int d = i * i + j * j; if (d >= 13 && d <= 25) curPx(i, j, C(80, 230, 255)); }
        curPx(0, 0, C_WHITE);
        break;
    case 3: /* SNIPER: a scope ring with lines running through it */
        for (int j = -7; j <= 7; j++) for (int i = -7; i <= 7; i++) { int d = i * i + j * j; if (d >= 36 && d <= 49) curPx(i, j, C(90, 255, 120)); }
        for (int i = 2; i <= 9; i++) { curPx(-i, 0, C(90, 255, 120)); curPx(i, 0, C(90, 255, 120)); curPx(0, -i, C(90, 255, 120)); curPx(0, i, C(90, 255, 120)); }
        curPx(0, 0, red);
        break;
    case 4: /* X */
        for (int i = 2; i <= 4; i++) { curPx(-i, -i, C_WHITE); curPx(i, -i, C_WHITE); curPx(-i, i, C_WHITE); curPx(i, i, C_WHITE); }
        curPx(0, 0, red);
        break;
    case 5: /* BRACKETS */
        for (int i = 0; i < 3; i++)
            for (int s = 0; s < 4; s++) {
                int sx = s & 1 ? 1 : -1, sy = s & 2 ? 1 : -1;
                curPx(sx * (5 - i), sy * 5, C(255, 214, 80)); curPx(sx * 5, sy * (5 - i), C(255, 214, 80));
            }
        curPx(0, 0, C_WHITE);
        break;
    case 6: { /* NEON: a glowing plus that pulses between cyan and pink */
        unsigned a = mixc(C(0, 240, 255), C(255, 60, 220), 0.5f + 0.5f * sinf(gt * 4));
        for (int i = 1; i <= 5; i++) { curPx(-i + 1, 1, C(90, 20, 90)); curPx(i + 1, 1, C(90, 20, 90)); curPx(1, -i + 1, C(90, 20, 90)); curPx(1, i + 1, C(90, 20, 90)); }
        for (int i = 1; i <= 5; i++) { curPx(-i, 0, a); curPx(i, 0, a); curPx(0, -i, a); curPx(0, i, a); }
        curPx(0, 0, C_WHITE);
        break; }
    case 7: /* RAINBOW: the colours run round the plus */
        for (int i = 2; i <= 5; i++)
            for (int s = 0; s < 4; s++) {
                unsigned c = rainbow[(i + s + (int)(gt * 10)) % 6];
                curPx(s == 0 ? -i : s == 1 ? i : 0, s == 2 ? -i : s == 3 ? i : 0, c);
            }
        curPx(0, 0, C_WHITE);
        break;
    case 8: cursorBits(arrow, 11, 0, 0, C_WHITE, red); break;
    case 9: cursorBits(skull, 7, 3, 3, C(235, 235, 225), C(200, 20, 30)); break;
    case 10: cursorBits(heart, 6, 3, 2, C(255, 70, 120), red); break;
    case 11: cursorBits(star, 7, 3, 3, C(255, 214, 60), red); break;
    case 12: curPx(0, 0, C_WHITE); break; /* PIXEL: just the classic's centre dot, in white */
    case 13: /* TARGET: a red and white bullseye */
        for (int j = -4; j <= 4; j++) for (int i = -4; i <= 4; i++) {
            int d = i * i + j * j;
            if (d >= 13 && d <= 20) curPx(i, j, red); else if (d >= 4 && d <= 5) curPx(i, j, C_WHITE);
        }
        curPx(0, 0, red);
        break;
    case 14: { /* DIAMOND: a pulsing outline */
        unsigned a = mixc(C_WHITE, C(80, 230, 255), 0.5f + 0.5f * sinf(gt * 5));
        for (int j = -4; j <= 4; j++) for (int i = -4; i <= 4; i++) if ((i < 0 ? -i : i) + (j < 0 ? -j : j) == 4) curPx(i, j, a);
        curPx(0, 0, red);
        break; }
    case 15: { /* LOCK-ON: corner brackets that snap in and out */
        int r = 3 + (int)(2.99f * (0.5f + 0.5f * sinf(gt * 6)));
        for (int s = 0; s < 4; s++) {
            int sx = s & 1 ? 1 : -1, sy = s & 2 ? 1 : -1;
            curPx(sx * r, sy * r, red); curPx(sx * (r - 1), sy * r, red); curPx(sx * r, sy * (r - 1), red);
        }
        curPx(0, 0, red);
        break; }
    case 16: { /* RADAR: a scope with a sweeping beam */
        for (int j = -5; j <= 5; j++) for (int i = -5; i <= 5; i++) { int d = i * i + j * j; if (d >= 20 && d <= 25) curPx(i, j, C(40, 140, 60)); }
        for (int t = 0; t < 3; t++) {
            float a = gt * 4 - t * 0.35f;
            unsigned c = t == 0 ? C(140, 255, 150) : t == 1 ? C(80, 200, 100) : C(50, 140, 70);
            for (int r = 1; r <= 4; r++) curPx((int)roundf(cosf(a) * r), (int)roundf(sinf(a) * r), c);
        }
        curPx(0, 0, C_WHITE);
        break; }
    case 17: { /* GLITCH: a plus that now and then splits into red and cyan */
        int o = ((int)(gt * 12)) % 5 == 0 ? 1 : 0, l = ((int)(gt * 7)) % 3 == 0 ? 5 : 4;
        for (int i = 2; i <= l; i++) {
            curPx(-i - o, 0, C(255, 40, 80)); curPx(i - o, 0, C(255, 40, 80)); curPx(-o, -i, C(255, 40, 80)); curPx(-o, i, C(255, 40, 80));
            curPx(-i + o, 0, C(0, 230, 255)); curPx(i + o, 0, C(0, 230, 255)); curPx(o, -i, C(0, 230, 255)); curPx(o, i, C(0, 230, 255));
        }
        for (int i = 2; i <= l; i++) { curPx(-i, 0, C_WHITE); curPx(i, 0, C_WHITE); curPx(0, -i, C_WHITE); curPx(0, i, C_WHITE); }
        curPx(0, 0, C_WHITE);
        break; }
    case 18: /* TRI: three ticks pointing in */
        for (int s = 0; s < 3; s++) {
            float a = -1.5708f + s * 2.0944f;
            for (int r = 2; r <= 4; r++) curPx((int)roundf(cosf(a) * r), (int)roundf(sinf(a) * r), C_WHITE);
        }
        curPx(0, 0, red);
        break;
    case CUR_SWORD: cursorBits(sword, 10, 0, 0, C(170, 185, 210), C(255, 200, 60)); break;
    case 20: cursorBits(((int)(gt * 8)) & 1 ? flame2 : flame, 8, 3, 5, C(255, 150, 40), C(230, 50, 30)); break;
    case 21: cursorBits(ghost, 7, 3, 3, C(200, 220, 255), red); break;
    case 22: { /* SLIME: a toxic blob with a drip falling off it */
        cursorBits(slime, 6, 3, 3, C(60, 255, 30), C(20, 170, 20));
        int dy = ((int)(gt * 6)) % 4;
        curPx(-2, 3 + dy, C(60, 255, 30));
        if (dy < 3) curPx(2, 3 + (dy + 2) % 3, C(20, 170, 20));
        break; }
    case 23: cursorBits(bolt, 8, 3, 3, C(255, 230, 60), red); break;
    case 24: /* MOON: a crescent with a little star on the hot spot */
        for (int j = -4; j <= 4; j++) for (int i = -4; i <= 4; i++)
            if (i * i + j * j <= 16 && (i - 2) * (i - 2) + (j + 1) * (j + 1) > 9) curPx(i, j, C(255, 240, 170));
        curPx(0, 0, C_WHITE);
        break;
    case 25: cursorBits(crown, 6, 3, 3, C(255, 200, 40), red); break;
    case 26: /* MINI CROSS */
        for (int i = 1; i <= 2; i++) { cmSet(-i, 0, C_WHITE); cmSet(i, 0, C_WHITE); cmSet(0, -i, C_WHITE); cmSet(0, i, C_WHITE); }
        cmSet(0, 0, red); cmFlush();
        break;
    case 27: cmSet(0, 0, C_WHITE); cmFlush(); break; /* SPECK: one white pixel in a dark box */
    case 28: /* LIME DOT */
        cmSet(-1, 0, C(120, 255, 60)); cmSet(1, 0, C(120, 255, 60)); cmSet(0, -1, C(120, 255, 60)); cmSet(0, 1, C(120, 255, 60));
        cmSet(0, 0, C_WHITE); cmFlush();
        break;
    case 29: /* MINI X */
        for (int i = 1; i <= 2; i++) { cmSet(-i, -i, C_WHITE); cmSet(i, -i, C_WHITE); cmSet(-i, i, C_WHITE); cmSet(i, i, C_WHITE); }
        cmSet(0, 0, red); cmFlush();
        break;
    case 30: /* MINI RING */
        for (int j = -2; j <= 2; j++) for (int i = -2; i <= 2; i++) { int d = i * i + j * j; if (d >= 4 && d <= 5) cmSet(i, j, C(80, 230, 255)); }
        cmSet(0, 0, C_WHITE); cmFlush();
        break;
    case 31: /* GAP CROSS: bright yellow so it shows on the busiest levels */
        for (int i = 2; i <= 3; i++) { cmSet(-i, 0, C(255, 230, 40)); cmSet(i, 0, C(255, 230, 40)); cmSet(0, -i, C(255, 230, 40)); cmSet(0, i, C(255, 230, 40)); }
        cmSet(0, 0, C(255, 230, 40)); cmFlush();
        break;
    case 32: /* CHEVRON: a little ^ whose tip is the hot spot */
        for (int i = 0; i <= 2; i++) { cmSet(-i, i, C_WHITE); cmSet(i, i, C_WHITE); }
        cmFlush();
        break;
    case 33: { /* BLINK: a dot that flashes white and red */
        unsigned c = ((int)(gt * 3)) & 1 ? red : C_WHITE;
        cmSet(0, 0, c); cmSet(-1, 0, c); cmSet(1, 0, c); cmSet(0, -1, c); cmSet(0, 1, c); cmFlush();
        break; }
    }
    curS = 1;
}

void crosshair(void) { drawCursor(cursorSkin, mx, my, 1); }
