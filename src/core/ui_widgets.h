/* ui_widgets.h - Menu building blocks: panels, shaped panels, buttons, map previews and the backdrop. */
#ifndef NCS_UI_WIDGETS_H
#define NCS_UI_WIDGETS_H

/* --- menu drawing --- */
void frame(int x, int y, int w, int h, unsigned fill, unsigned border);
void panel(int x, int y, int w, int h, int focus);
void drawButton(const Btn *b, int focus);
/* a tab at the top of a screen: the open one is lit */
void drawTab(const Btn *b, int on, int focus);
void drawSpriteScaled(const Spr *s, int x, int y, int sc);
/* a skin standing with its feet on y = feet, centred on cx (the skins are not all the same size) */
void drawSkinScaled(const Spr *s, int cx, int feet, int sc);
void drawLock(int cx, int cy);
/* centred text broken onto lines of at most `maxc` characters */
int wrapText(int cx, int y, const char *s, int maxc, unsigned c);
void drawMapPreview(int d, int x, int y, int w, int h, int locked);
void coinsCorner(void);
void backdrop(int dimK);

#endif
