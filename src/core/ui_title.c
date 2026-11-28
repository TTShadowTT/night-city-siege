/* ui_title.c - The menu title and its 42 text styles. */
#include "core.h"

/* menu title */

/* the title is drawn in a marker colour, then each marked pixel gets its style's colour from its row, column and the time */
#define TMARK C(1, 2, 3)
static unsigned hueC(float h) /* h in turns: a fully saturated rainbow colour */
{
    float f = (h - floorf(h)) * 6;
    int i = (int)f;
    float t = f - i;
    int up = (int)(255 * t), dn = 255 - up;
    switch (i) {
    case 0: return C(255, up, 0);
    case 1: return C(dn, 255, 0);
    case 2: return C(0, 255, up);
    case 3: return C(0, dn, 255);
    case 4: return C(up, 0, 255);
    default: return C(255, 0, dn);
    }
}

/* letter shapes for the title styles: the same 3x5 font, drawn bold, slanted, tall, wavy, bubbly, dotted, shaky, stencilled or wide */
static int fxW(int fx, int scale) { return fx & FX_WIDE ? scale * 3 / 2 : scale; } /* one font pixel's width */
static int fxH(int fx, int scale) { return fx & FX_TALL ? scale * 3 / 2 : scale; } /* and its height */
static int fxBold(int fx, int scale) { return fx & FX_BOLD ? scale / 2 + 1 : 0; }
int textWidthFx(const char *s, int scale, int fx)
{
    int n = (int)strlen(s);
    return n ? (n * 4 - 1) * fxW(fx, scale) + (fx & FX_ITALIC ? 4 * (scale / 3 > 0 ? scale / 3 : 1) : 0) + fxBold(fx, scale) + (fx & FX_SLASH ? 5 * fxH(fx, scale) / 2 : 0) : 0;
}
static void drawTextFx(int x, int y, const char *s, unsigned c, int scale, int fx)
{
    int pw = fxW(fx, scale), ph = fxH(fx, scale), bold = fxBold(fx, scale), jig = scale / 2 > 0 ? scale / 2 : 1;
    for (int i = 0; *s; s++, i++, x += 4 * pw) {
        const Glyph *g = glyph(*s);
        int ox = 0, oy = 0;
        if (!g) continue;
        if (fx & FX_WAVE) oy = (int)roundf(sinf(gt * 5 + i * 0.8f) * scale * 0.7f);
        if (fx & FX_SHAKY) { unsigned h = hash3(i, (int)(gt * 14), 77); ox = ((int)(h % 3) - 1) * jig; oy += ((int)(h / 3 % 3) - 1) * jig; }
        for (int j = 0; j < 5; j++) {
            int sl = fx & FX_ITALIC ? (4 - j) * (scale / 3 > 0 ? scale / 3 : 1) : 0;
            for (int k = 0; k < 3; k++) {
                if (g->r[j][k] != '#') continue;
                int px0 = x + ox + sl + k * pw, py0 = y + oy + j * ph, bw = pw + bold;
                if (fx & FX_SLASH) { /* a hard slant, one screen row at a time, so strokes look brushed */
                    for (int yy = 0; yy < ph; yy++) fillRect(px0 + (5 * ph - j * ph - yy) / 2, py0 + yy, bw, 1, c);
                    continue;
                }
                if (fx & FX_DOTS) fillRect(px0, py0, pw > 1 ? pw - 1 : 1, ph > 1 ? ph - 1 : 1, c);
                else if (fx & FX_BUBBLE && pw >= 3) { fillRect(px0 + 1, py0, bw - 2, ph, c); fillRect(px0, py0 + 1, bw, ph - 2, c); }
                else if (fx & FX_STENCIL && j == 2 && ph >= 2) { fillRect(px0, py0, bw, ph / 2, c); fillRect(px0, py0 + ph / 2 + 1, bw, ph - ph / 2 - 1, c); }
                else fillRect(px0, py0, bw, ph, c);
            }
        }
        if (fx & FX_SLASH) { /* spiky brush tails: the first letter sweeps down-left, the last up-right, every third one drips a tail */
            int t = scale / 2 > 0 ? scale / 2 : 1, bx = x + ox + 5 * ph / 2, by = y + oy;
            if (i == 0) thickLine(bx - 5 * ph / 2, by + 5 * ph - 1, bx - 5 * ph / 2 - scale * 3, by + 5 * ph + scale, c, t);
            if (!s[1]) thickLine(bx + 3 * pw - 1, by, bx + 3 * pw + scale * 3, by - scale * 2, c, t);
            if (i % 3 == 2 && s[1]) thickLine(x + ox + pw / 2, by + 5 * ph - 1, x + ox - scale * 2, by + 5 * ph + scale * 2, c, t);
        }
    }
}
static void outlineText(int x, int y, const char *s, unsigned c, int scale, int fx)
{
    for (int dy = -1; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++) if (dx || dy) drawTextFx(x + dx, y + dy, s, c, scale, fx);
}
int titleH(int scale, int st) { return 5 * fxH(TSTY[st].fx, scale); }

void styledText(int cx, int y, const char *s, int scale, int st, int line)
{
    int fx = TSTY[st].fx, sc = TSTY[st].sc;
    int w = textWidthFx(s, scale, fx), x = cx - w / 2, h = titleH(scale, st), tick = (int)(gt * 12), pad = fx & FX_SLASH ? scale * 4 + 2 : scale * 2 + 2;
    if (!*s) return;
    if (fx & FX_LABEL && line) { /* line 2 as a tag: plain dark letters in a slanted box, hatching running in from the left */
        unsigned col = sc == SC_CP ? C(240, 250, 40) : sc == SC_CPRED ? C(255, 50, 70) : sc == SC_CPCYAN ? C(0, 230, 255) : sc == SC_PINK ? C(255, 70, 190) : ui.hi;
        int tw = textWidth(s, scale), bh = 7 * scale, bx = cx - tw / 2 - scale * 2, by = y - scale, bw = tw + scale * 4;
        for (int yy = -1; yy <= bh; yy++) fillRect(bx - (yy + 1) / 2 - 1, by + yy, bw + (yy + 1) / 2 + 2, 1, scalec(col, 70)); /* glow */
        for (int yy = 0; yy < bh; yy++) fillRect(bx - yy / 2, by + yy, bw + yy / 2, 1, col);
        drawText(cx - tw / 2, y, s, C(12, 22, 30), scale);
        for (int k = 0; k < 12; k++) { /* the hatching fades out to the left */
            int hx = bx - bh / 2 - 4 - k * (scale + 3);
            thickLine(hx, by + bh - 1, hx + bh / 2, by, mixc(col, C(10, 20, 30), k / 13.f), scale / 2 > 0 ? scale / 2 : 1);
        }
        return;
    }
    if (sc == SC_NEON && !fx) { /* NEON: the original flickering tube */
        neonText(cx, y, s, line ? ui.titleB : ui.titleA, scale, line || !(hash3((int)(gt * 10), 1, 2) % 23 == 0));
        return;
    }
    switch (sc) { /* what goes behind the letters */
    case SC_CHROME: case SC_RAINBOW: outlineText(x, y, s, C(20, 20, 32), scale, fx); break;
    case SC_FIRE: drawTextFx(x + scale / 2 + 1, y + scale / 2 + 1, s, C(70, 12, 0), scale, fx); outlineText(x, y, s, C(40, 8, 0), scale, fx); break;
    case SC_GOLD: drawTextFx(x + scale / 2 + 1, y + scale / 2 + 1, s, C(70, 40, 0), scale, fx); outlineText(x, y, s, C(60, 34, 0), scale, fx); break;
    case SC_TOXIC: outlineText(x, y, s, C(10, 40, 0), scale, fx); break;
    case SC_GLITCH: {
        int o = scale / 2 + (tick % 7 == 0 ? scale : 0);
        drawTextFx(x - o, y, s, C(255, 40, 80), scale, fx); drawTextFx(x + o, y, s, C(0, 230, 255), scale, fx);
        break; }
    case SC_SYNTH: if (!(fx & FX_HOLLOW)) for (int i = scale; i >= 1; i--) drawTextFx(x + i, y + i, s, mixc(C(40, 10, 70), C(140, 30, 160), (float)(scale - i) / scale), scale, fx); break;
    case SC_ICE: outlineText(x, y, s, C(20, 60, 120), scale, fx); break;
    case SC_MATRIX: outlineText(x, y, s, C(0, 30, 10), scale, fx); break;
    case SC_BLOOD: drawTextFx(x + scale / 2 + 1, y + scale / 2 + 1, s, C(30, 0, 0), scale, fx); outlineText(x, y, s, C(40, 0, 0), scale, fx); break;
    case SC_VAPOR: drawTextFx(x + scale, y + scale, s, C(90, 50, 140), scale, fx); outlineText(x, y, s, C(50, 20, 80), scale, fx); break;
    case SC_PINK: outlineText(x, y, s, C(60, 0, 40), scale, fx); break;
    case SC_EDGE: /* yellow with a hard cyan shadow and a magenta edge */
        drawTextFx(x + scale / 2 + 1, y + scale / 2 + 1, s, C(0, 235, 255), scale, fx); outlineText(x, y, s, C(120, 0, 80), scale, fx); break;
    case SC_PSYCHO: { /* red ghosts jumping around the letters */
        int o = scale / 2 + 1 + (tick % 5 == 0 ? scale * 2 : 0);
        drawTextFx(x - o, y + (tick % 3 == 0 ? scale : 0), s, C(110, 0, 10), scale, fx);
        drawTextFx(x + o, y, s, tick % 9 == 0 ? C_WHITE : C(60, 0, 0), scale, fx);
        outlineText(x, y, s, C(20, 0, 0), scale, fx);
        break; }
    case SC_SANDY: /* the afterimages left behind when time slows down */
        for (int k = 4; k >= 1; k--) drawTextFx(x - k * scale * 2, y, s, scalec(hueC(gt * 0.6f + k * 0.13f), 60 + (4 - k) * 40), scale, fx);
        outlineText(x, y, s, C(10, 30, 20), scale, fx);
        break;
    case SC_NET: break; /* just the wireframe */
    case SC_ARASAKA: outlineText(x, y, s, C(20, 0, 0), scale, fx); break;
    case SC_SAMURAI: drawTextFx(x + scale / 2 + 1, y + scale / 2 + 1, s, C(60, 0, 0), scale, fx); outlineText(x, y, s, C(220, 20, 40), scale, fx); break;
    case SC_HOLO: outlineText(x, y, s, C(0, 50, 80), scale, fx); break;
    case SC_MAXTAC: case SC_KABUKI: case SC_BD: outlineText(x, y, s, C(10, 8, 20), scale, fx); break;
    case SC_AFTER: drawTextFx(x + scale / 2 + 1, y + scale / 2 + 1, s, C(50, 20, 0), scale, fx); outlineText(x, y, s, C(60, 25, 0), scale, fx); break;
    case SC_CP: case SC_CPRED: case SC_CPCYAN: { /* a soft glow, then a bright rim */
        unsigned glow = sc == SC_CP ? C(10, 60, 25) : sc == SC_CPRED ? C(60, 0, 12) : C(0, 40, 70);
        unsigned rim = sc == SC_CP ? C(40, 200, 60) : sc == SC_CPRED ? C(150, 0, 25) : C(0, 140, 210);
        for (int dy = -2; dy <= 2; dy++)
            for (int dx = -2; dx <= 2; dx++) if (dx * dx + dy * dy > 2) drawTextFx(x + dx, y + dy, s, glow, scale, fx);
        outlineText(x, y, s, rim, scale, fx);
        break; }
    case SC_STREET: /* a fat graffiti outline */
        for (int dy = -2; dy <= 2; dy++)
            for (int dx = -2; dx <= 2; dx++) if (dx || dy) drawTextFx(x + dx, y + dy, s, C(25, 0, 40), scale, fx);
        break;
    default: drawTextFx(x + scale, y + scale, s, C_OUT, scale, fx); outlineText(x, y, s, C_OUT, scale, fx); break;
    }
    if (fx & FX_HOLLOW) { /* OUTLINE: only the edge is coloured, the letters stay dark inside */
        outlineText(x, y, s, TMARK, scale, fx);
        drawTextFx(x, y, s, C(14, 8, 26), scale, fx);
    } else drawTextFx(x, y, s, TMARK, scale, fx);
    /* the letters can sit a little outside their box (waves, slants, outlines), so recolour a margin around it */
    for (int py = y - pad; py < y + h + pad; py++) {
        if (py < 0 || py >= FH) continue;
        int r = py - y;
        float f = r < 0 ? 0 : r >= h ? 1 : (float)r / (h - 1);
        for (int px_ = x - pad < 0 ? 0 : x - pad; px_ < x + w + pad && px_ < FW; px_++) {
            unsigned *q = &fb[py * FW + px_], c;
            if (*q != TMARK) continue;
            switch (sc) {
            case SC_CHROME: /* sky over a horizon line over ground, with a shine sweeping across */
                c = f < 0.45f ? mixc(C_WHITE, C(160, 175, 200), f / 0.45f) : f < 0.55f ? C(80, 86, 110) : mixc(C(210, 220, 240), C(110, 115, 140), (f - 0.55f) / 0.45f);
                if ((((px_ - x) - r + (int)(gt * 140)) % 220 + 220) % 220 < 5) c = C_WHITE;
                break;
            case SC_FIRE: /* yellow to red, flickering */
                c = f < 0.5f ? mixc(C(255, 245, 140), C(255, 140, 30), f * 2) : mixc(C(255, 140, 30), C(200, 30, 10), (f - 0.5f) * 2);
                if (hash3((px_ - x) / scale, r / scale, tick) % 100 < 14) c = mixc(c, C(255, 255, 200), 0.5f);
                break;
            case SC_RAINBOW: c = hueC((px_ - x) / 90.f - gt * 0.4f + f * 0.15f); break; /* running sideways */
            case SC_GOLD: /* with a glint */
                c = f < 0.4f ? mixc(C(255, 245, 170), C(240, 185, 40), f / 0.4f) : mixc(C(240, 185, 40), C(150, 90, 10), (f - 0.4f) / 0.6f);
                if ((((px_ - x) + r * 2 - (int)(gt * 110)) % 260 + 260) % 260 < 4) c = C_WHITE;
                break;
            case SC_TOXIC: case SC_BLOOD: { /* bubbling goo (or blood) dripping off the bottom */
                int blood = sc == SC_BLOOD;
                c = blood ? mixc(C(240, 60, 60), C(110, 0, 10), f) : mixc(C(210, 255, 120), C(40, 190, 20), f);
                if (hash3((px_ - x) / scale, r / scale, tick / 3) % 100 < 8) c = blood ? C(255, 120, 120) : C(230, 255, 180);
                if (r >= h - 1 && hash3((px_ - x) / scale, 7, 9) % 4 == 0 && (py + 1 >= FH || fb[(py + 1) * FW + px_] != TMARK)) {
                    int len = (int)(gt * 10 + hash3((px_ - x) / scale, 3, 3) % 20) % (scale * 2 + 2);
                    for (int k = 1; k <= len && py + k < FH; k++)
                        fb[(py + k) * FW + px_] = k == len ? (blood ? C(255, 90, 90) : C(200, 255, 110)) : (blood ? C(150, 0, 10) : C(40, 190, 20));
                }
                break; }
            case SC_GLITCH: c = C_WHITE; break; /* white over red and cyan; rows get shifted below */
            case SC_SYNTH: /* sunset gradient with stripes in the lower half */
                c = f < 0.5f ? mixc(C(255, 240, 120), C(255, 120, 60), f * 2) : mixc(C(255, 60, 150), C(170, 40, 230), (f - 0.5f) * 2);
                if (f > 0.5f && r % scale == scale - 1 && scale > 1) c = scalec(c, 150);
                break;
            case SC_ICE: /* frosty with twinkles */
                c = mixc(C_WHITE, C(130, 210, 255), f);
                if (hash3(px_, py, (int)(gt * 4)) % 70 == 0) c = C_WHITE;
                break;
            case SC_MATRIX: { /* green code raining down through the letters */
                unsigned hsh = hash3((px_ - x) / (scale > 1 ? scale : 1), (r / (scale > 1 ? scale : 1)) - (int)(gt * 8), 41);
                c = hsh % 7 == 0 ? C(200, 255, 210) : hsh % 3 == 0 ? C(30, 150, 60) : C(40, 230, 90);
                break; }
            case SC_VAPOR: c = mixc(C(255, 150, 220), C(120, 230, 255), 0.5f + 0.5f * sinf((px_ - x) / 30.f - gt * 2)); break; /* pastel pink and cyan */
            case SC_PINK: c = mixc(C(255, 210, 245), C(255, 40, 180), f); break;
            case SC_EDGE: /* electric yellow */
                c = mixc(C(255, 245, 60), C(255, 175, 0), f);
                if ((((px_ - x) - r + (int)(gt * 160)) % 240 + 240) % 240 < 4) c = C_WHITE;
                break;
            case SC_PSYCHO: { /* red static with black bands tearing through */
                unsigned hsh = hash3(r / (scale > 1 ? scale : 1), tick, 13);
                c = hsh % 6 == 0 ? C(30, 0, 0) : hsh % 17 == 0 ? C_WHITE : mixc(C(255, 70, 80), C(190, 0, 20), f);
                if (hash3(px_, py, tick) % 23 == 0) c = C(255, 180, 180);
                break; }
            case SC_SANDY: c = mixc(C(230, 255, 240), C(90, 255, 160), f); break;
            case SC_NET: /* cyan wireframe with a scan line sweeping down */
                c = ((r - (int)(gt * 30)) % (h + 8) + (h + 8)) % (h + 8) < 2 ? C_WHITE : C(0, 240, 220);
                break;
            case SC_ARASAKA: c = f < 0.5f ? mixc(C(255, 90, 90), C(235, 20, 35), f * 2) : C(200, 0, 25); break;
            case SC_SAMURAI: /* black chrome with a red glint */
                c = mixc(C(90, 90, 104), C(22, 22, 30), f);
                if ((((px_ - x) + r - (int)(gt * 120)) % 230 + 230) % 230 < 4) c = C(255, 60, 70);
                break;
            case SC_HOLO: /* see-through blue with scan lines and a flicker */
                c = (py + tick / 2) % 3 == 0 ? C(20, 90, 130) : mixc(C(170, 245, 255), C(60, 170, 255), f);
                if (hash3(tick / 2, 3, line) % 12 == 0) c = scalec(c, 120);
                break;
            case SC_MAXTAC: { /* police lights: red and blue swapping sides */
                int left = px_ < x + w / 2, ph_ = ((int)(gt * 5)) & 1;
                c = left ^ ph_ ? C(70, 130, 255) : C(255, 50, 70);
                if (r < scale) c = mixc(c, C_WHITE, 0.6f);
                break; }
            case SC_BD: /* braindance: purple with ripples running through */
                c = mixc(C(170, 80, 255), C(255, 120, 220), f);
                if (sinf((px_ - x) / 9.f + gt * 6 + r * 0.25f) > 0.85f) c = mixc(c, C_WHITE, 0.6f);
                break;
            case SC_KABUKI: { /* each letter its own neon sign; now and then one goes dark */
                int idx = (px_ - x) / (4 * fxW(fx, scale));
                static const unsigned signs[3] = { C(255, 60, 180), C(255, 230, 60), C(0, 230, 255) };
                c = signs[idx % 3];
                if (hash3(idx, tick / 5, 21) % 9 == 0) c = scalec(c, 70);
                break; }
            case SC_AFTER: { /* a warm bar sign with one letter on the blink */
                int idx = (px_ - x) / (4 * fxW(fx, scale)), n = (int)strlen(s);
                c = mixc(C(255, 225, 160), C(230, 120, 40), f);
                if (n && idx == (int)(hash3(tick / 30, 5, 5) % (unsigned)n) && hash3(tick, 9, 9) % 3) c = C(90, 50, 25);
                break; }
            case SC_STREET: c = mixc(C(255, 40, 200), C(255, 230, 40), ((px_ - x) + r * 2) / (float)(w + h * 2)); break; /* graffiti fade */
            case SC_CP: c = mixc(C(250, 255, 100), C(205, 240, 0), f); break;
            case SC_CPRED: c = mixc(C(255, 110, 110), C(225, 10, 35), f); break;
            case SC_CPCYAN: c = mixc(C(210, 255, 255), C(0, 215, 255), f); break;
            default: c = line ? C(205, 205, 220) : C_WHITE; break; /* CLEAN */
            }
            if (fx & FX_SLASH && (r == h * 2 / 5 || r == h * 3 / 4) && hash3((px_ - x) / (scale * 2 + 1), r, line) % 3 == 0) c = scalec(c, 70); /* cuts through the paint */
            *q = c;
        }
    }
    if (sc == SC_CP || sc == SC_CPRED || sc == SC_CPCYAN) /* little glitch dashes floating around the logo */
        for (int k = 0; k < 7; k++) {
            unsigned hsh = hash3(k, tick / 6, line + 70);
            int dx = x - pad * 2 + (int)(hsh % (unsigned)(w + pad * 4)), dy = y - pad + (int)(hsh / 11 % (unsigned)(h + pad * 2)), len = 2 + (int)(hsh / 3 % 6);
            fillRect(dx, dy, len * (scale > 2 ? 2 : 1), 1, sc == SC_CP ? C(60, 220, 60) : sc == SC_CPRED ? C(220, 30, 50) : C(0, 190, 240));
        }
    if (sc == SC_ARASAKA) { /* the corp underline */
        fillRect(x, y + h + scale, w, scale / 2 > 0 ? scale / 2 : 1, C(230, 20, 35));
        fillRect(x - scale * 2, y + h + scale - 1, scale + 1, scale / 2 + 2, C_WHITE);
    }
    if (sc == SC_STREET) /* spray paint dots around the tag */
        for (int k = 0; k < w / 3; k++) {
            unsigned hsh = hash3(k, line, 31);
            int dx = x - pad + (int)(hsh % (unsigned)(w + pad * 2)), dy = y - pad + (int)(hsh / 7 % (unsigned)(h + pad * 2));
            if (dx >= 0 && dx < FW && dy >= 0 && dy < FH && fb[dy * FW + dx] != C(25, 0, 40)) pset(dx, dy, k & 1 ? C(255, 40, 200) : C(0, 230, 255));
        }
    if (sc == SC_PSYCHO) /* stray red static around the letters */
        for (int k = 0; k < w / 4; k++) {
            unsigned hsh = hash3(k, tick, 57);
            int dx = x - pad + (int)(hsh % (unsigned)(w + pad * 2)), dy = y - pad + (int)(hsh / 5 % (unsigned)(h + pad * 2));
            if (dx >= 0 && dx < FW && dy >= 0 && dy < FH) pset(dx, dy, C(255, 20, 50));
        }
    if (sc == SC_GLITCH || sc == SC_PSYCHO) /* now and then a slice of the letters jumps sideways */
        for (int r = 0; r < h; r += scale) {
            if (hash3(r / scale, tick / 2, line + 5) % (sc == SC_PSYCHO ? 4 : 9)) continue;
            int sh = (hash3(r, tick, 1) & 1 ? 1 : -1) * scale * 2;
            for (int k = 0; k < scale && y + r + k < FH; k++) {
                unsigned *row = &fb[(y + r + k) * FW], tmp[FW];
                int a = x - scale * 3 < 0 ? 0 : x - scale * 3, b = x + w + scale * 3 > FW ? FW : x + w + scale * 3;
                if (y + r + k < 0) continue;
                for (int i = a; i < b; i++) tmp[i] = row[i];
                for (int i = a; i < b; i++) { int j = i - sh; if (j >= a && j < b) row[i] = tmp[j]; }
            }
        }
}

/* the two lines shrink to fit the screen; returns where the title ends */
static int titleScale(const char *s, int big)
{
    int sc = big;
    while (sc > 1 && textWidthFx(s, sc, TSTY[titleStyle].fx) > FW - 24) sc--;
    return sc;
}
int titleBottom(int y)
{
    int s1 = titleScale(title1, 5), s2 = titleScale(title2, 3);
    if (*title1) y += titleH(s1, titleStyle) + 13;
    if (*title2) y += titleH(s2, titleStyle) + (*title1 ? 0 : 13);
    return y;
}
void drawMenuTitle(int y)
{
    int s1 = titleScale(title1, 5), s2 = titleScale(title2, 3);
    if (*title1) { styledText(FW / 2, y, title1, s1, titleStyle, 0); y += titleH(s1, titleStyle) + 13; }
    if (*title2) styledText(FW / 2, y, title2, s2, titleStyle, 1);
}
