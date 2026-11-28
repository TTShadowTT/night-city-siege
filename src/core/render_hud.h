/* render_hud.h - The in-game HUD: health, ammo, weapons, score and boss bars. */
#ifndef NCS_RENDER_HUD_H
#define NCS_RENDER_HUD_H

extern const char *const CURSORS[CURSOR_COUNT];
extern int curX, curY, curS;

void drawCoin(int x, int y);
void hudBar(int x, int y, int w, int h, float frac, unsigned c);
void renderHud(void);
void curPx(int x, int y, unsigned c);

#endif
