/* ui_screens_misc.h - Menu screens: quests, keys, quit, upgrade cards and toasts. */
#ifndef NCS_UI_SCREENS_MISC_H
#define NCS_UI_SCREENS_MISC_H

/* a thin bar at the right edge when the screen scrolls */
void drawScrollBar(void);
void renderQuests(void);
void renderKeys(void);
void renderQuit(void);
void renderPerks(void);
/* the upgrades this run has collected, two columns */
void perkList(int y);
void renderToast(void);

#endif
