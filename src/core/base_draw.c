/* base_draw.c - Drawing into any pixel surface: pixels, shapes, sprites and the 3x5 pixel font. */
#include "core.h"

unsigned *tgt;
int tgtW, tgtH;
int ox, oy; /* screen shake offset for world drawing */
int clipL, clipR = 1 << 30; /* drawing is cut off left of clipL and from clipR on (scrolling labels) */

void fillRect(int x, int y, int w, int h, unsigned c)
{
    for (int j = y; j < y + h; j++)
        for (int i = x; i < x + w; i++) pset(i, j, c);
}

void fillCircle(int cx, int cy, int r, unsigned c)
{
    for (int j = -r; j <= r; j++)
        for (int i = -r; i <= r; i++)
            if (i * i + j * j <= r * r + r / 2) pset(cx + i, cy + j, c);
}

void line(int x0, int y0, int x1, int y1, unsigned c)
{
    int dx = x1 > x0 ? x1 - x0 : x0 - x1, dy = y1 > y0 ? y0 - y1 : y1 - y0;
    int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1, err = dx + dy;
    for (int guard = 0; guard < 2000; guard++) {
        pset(x0, y0, c);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void thickLine(int x0, int y0, int x1, int y1, unsigned c, int t)
{
    int horiz = (x1 > x0 ? x1 - x0 : x0 - x1) >= (y1 > y0 ? y1 - y0 : y0 - y1);
    for (int o = 0; o < t; o++) {
        if (horiz) line(x0, y0 + o, x1, y1 + o, c); else line(x0 + o, y0, x1 + o, y1, c);
    }
}
void drawSprite(const Spr *s, int x, int y, int flip, int mode, unsigned tint)
{
    for (int j = 0; j < s->h; j++) {
        const char *row = s->rows[j];
        int len = (int)strlen(row);
        for (int i = 0; i < s->w && i < len; i++) {
            char ch = row[i];
            if (ch == '.') continue;
            unsigned c = pal(ch);
            if (mode == DM_SOLID) c = tint;
            else if (mode == DM_FILL && ch != 'k') c = tint;
            else if (mode == DM_TINT) c = mixc(c, tint, 0.45f);
            pset(flip ? x + s->w - 1 - i : x + i, y + j, c);
        }
    }
}

/* draw a sprite standing on (cx, feet) in world coordinates */
void drawFeet(const Spr *s, float cx, float feet, int flip, int mode, unsigned tint)
{
    drawSprite(s, (int)floorf(cx) - s->w / 2 + ox, (int)floorf(feet) - s->h + oy, flip, mode, tint);
}
static const Glyph FONT[] = {
    { 'A', { ".#.", "#.#", "###", "#.#", "#.#" } }, { 'B', { "##.", "#.#", "##.", "#.#", "##." } },
    { 'C', { ".##", "#..", "#..", "#..", ".##" } }, { 'D', { "##.", "#.#", "#.#", "#.#", "##." } },
    { 'E', { "###", "#..", "##.", "#..", "###" } }, { 'F', { "###", "#..", "##.", "#..", "#.." } },
    { 'G', { ".##", "#..", "#.#", "#.#", ".##" } }, { 'H', { "#.#", "#.#", "###", "#.#", "#.#" } },
    { 'I', { "###", ".#.", ".#.", ".#.", "###" } }, { 'J', { "..#", "..#", "..#", "#.#", ".#." } },
    { 'K', { "#.#", "#.#", "##.", "#.#", "#.#" } }, { 'L', { "#..", "#..", "#..", "#..", "###" } },
    { 'M', { "#.#", "###", "###", "#.#", "#.#" } }, { 'N', { "##.", "#.#", "#.#", "#.#", "#.#" } },
    { 'O', { ".#.", "#.#", "#.#", "#.#", ".#." } }, { 'P', { "##.", "#.#", "##.", "#..", "#.." } },
    { 'Q', { ".#.", "#.#", "#.#", "##.", ".##" } }, { 'R', { "##.", "#.#", "##.", "#.#", "#.#" } },
    { 'S', { ".##", "#..", ".#.", "..#", "##." } }, { 'T', { "###", ".#.", ".#.", ".#.", ".#." } },
    { 'U', { "#.#", "#.#", "#.#", "#.#", "###" } }, { 'V', { "#.#", "#.#", "#.#", "#.#", ".#." } },
    { 'W', { "#.#", "#.#", "###", "###", "#.#" } }, { 'X', { "#.#", "#.#", ".#.", "#.#", "#.#" } },
    { 'Y', { "#.#", "#.#", ".#.", ".#.", ".#." } }, { 'Z', { "###", "..#", ".#.", "#..", "###" } },
    { '0', { "###", "#.#", "#.#", "#.#", "###" } }, { '1', { ".#.", "##.", ".#.", ".#.", "###" } },
    { '2', { "##.", "..#", ".#.", "#..", "###" } }, { '3', { "##.", "..#", ".#.", "..#", "##." } },
    { '4', { "#.#", "#.#", "###", "..#", "..#" } }, { '5', { "###", "#..", "##.", "..#", "##." } },
    { '6', { ".##", "#..", "###", "#.#", "###" } }, { '7', { "###", "..#", ".#.", ".#.", ".#." } },
    { '8', { "###", "#.#", "###", "#.#", "###" } }, { '9', { "###", "#.#", "###", "..#", "##." } },
    { '.', { "...", "...", "...", "...", ".#." } }, { ',', { "...", "...", "...", ".#.", "#.." } },
    { ':', { "...", ".#.", "...", ".#.", "..." } }, { '!', { ".#.", ".#.", ".#.", "...", ".#." } },
    { '?', { "##.", "..#", ".#.", "...", ".#." } }, { '-', { "...", "...", "###", "...", "..." } },
    { '+', { "...", ".#.", "###", ".#.", "..." } }, { '/', { "..#", "..#", ".#.", "#..", "#.." } },
    { '(', { ".#.", "#..", "#..", "#..", ".#." } }, { ')', { ".#.", "..#", "..#", "..#", ".#." } },
    { '<', { "..#", ".#.", "#..", ".#.", "..#" } }, { '>', { "#..", ".#.", "..#", ".#.", "#.." } },
    { '=', { "...", "###", "...", "###", "..." } }, { '\'', { ".#.", ".#.", "...", "...", "..." } },
    { '%', { "#.#", "..#", ".#.", "#..", "#.#" } },
};

const Glyph *glyph(char c)
{
    if (c >= 'a' && c <= 'z') c = (char)(c - 32);
    for (unsigned i = 0; i < sizeof FONT / sizeof FONT[0]; i++)
        if (FONT[i].c == c) return &FONT[i];
    return 0; /* space and anything unknown draw nothing */
}

int textWidth(const char *s, int scale) { int n = (int)strlen(s); return n ? (n * 4 - 1) * scale : 0; }

void drawText(int x, int y, const char *s, unsigned c, int scale)
{
    for (; *s; s++, x += 4 * scale) {
        const Glyph *g = glyph(*s);
        if (!g) continue;
        for (int j = 0; j < 5; j++)
            for (int i = 0; i < 3; i++)
                if (g->r[j][i] == '#') fillRect(x + i * scale, y + j * scale, scale, scale, c);
    }
}
void text(int x, int y, const char *s, unsigned c, int scale, int align)
{
    int w = textWidth(s, scale);
    if (align == AL_CENTER) x -= w / 2; else if (align == AL_RIGHT) x -= w;
    drawText(x + scale, y + scale, s, C_OUT, scale);
    drawText(x, y, s, c, scale);
}

/* neon-tube text: a soft halo, then a bright core */
void neonText(int x, int y, const char *s, unsigned col, int scale, int on)
{
    int w = textWidth(s, scale);
    x -= w / 2;
    if (on) {
        unsigned halo = scalec(col, 120);
        for (int dy = -1; dy <= 1; dy++)
            for (int dx = -1; dx <= 1; dx++) if (dx || dy) drawText(x + dx * scale / 2 + dx, y + dy * scale / 2 + dy, s, halo, scale);
        drawText(x, y, s, mixc(col, C_WHITE, 0.35f), scale);
    } else {
        drawText(x, y, s, scalec(col, 70), scale);
    }
}
