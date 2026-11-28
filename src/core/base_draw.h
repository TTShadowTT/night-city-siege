/* base_draw.h - Drawing into any pixel surface: pixels, shapes, sprites and the 3x5 pixel font. */
#ifndef NCS_BASE_DRAW_H
#define NCS_BASE_DRAW_H

enum { DM_NORMAL, DM_SOLID, DM_FILL, DM_TINT };

typedef struct { char c; const char *r[5]; } Glyph;

enum { AL_LEFT, AL_CENTER, AL_RIGHT };

extern unsigned *tgt;
extern int tgtW, tgtH;
extern int ox, oy; /* screen shake offset for world drawing */
extern int clipL, clipR; /* drawing is cut off left of clipL and from clipR on (scrolling labels) */

void fillRect(int x, int y, int w, int h, unsigned c);
void fillCircle(int cx, int cy, int r, unsigned c);
void line(int x0, int y0, int x1, int y1, unsigned c);
void thickLine(int x0, int y0, int x1, int y1, unsigned c, int t);
void drawSprite(const Spr *s, int x, int y, int flip, int mode, unsigned tint);
/* draw a sprite standing on (cx, feet) in world coordinates */
void drawFeet(const Spr *s, float cx, float feet, int flip, int mode, unsigned tint);
const Glyph *glyph(char c);
int textWidth(const char *s, int scale);
void drawText(int x, int y, const char *s, unsigned c, int scale);
void text(int x, int y, const char *s, unsigned c, int scale, int align);
/* neon-tube text: a soft halo, then a bright core */
void neonText(int x, int y, const char *s, unsigned col, int scale, int on);

static inline void target(unsigned *buf, int w, int h) { tgt = buf; tgtW = w; tgtH = h; }

static inline void pset(int x, int y, unsigned c) { if ((unsigned)x < (unsigned)tgtW && (unsigned)y < (unsigned)tgtH && x >= clipL && x < clipR) tgt[y * tgtW + x] = c; }

static inline void pmix(int x, int y, unsigned c, float t) { if ((unsigned)x < (unsigned)tgtW && (unsigned)y < (unsigned)tgtH) tgt[y * tgtW + x] = mixc(tgt[y * tgtW + x], c, t); }

static inline void pdark(int x, int y, int k256) { if ((unsigned)x < (unsigned)tgtW && (unsigned)y < (unsigned)tgtH) tgt[y * tgtW + x] = scalec(tgt[y * tgtW + x], k256); }

#endif
