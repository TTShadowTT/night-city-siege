/* ui_title.h - The menu title and its 42 text styles. */
#ifndef NCS_UI_TITLE_H
#define NCS_UI_TITLE_H

int textWidthFx(const char *s, int scale, int fx);
int titleH(int scale, int st);
void styledText(int cx, int y, const char *s, int scale, int st, int line);
int titleBottom(int y);
void drawMenuTitle(int y);

#endif
