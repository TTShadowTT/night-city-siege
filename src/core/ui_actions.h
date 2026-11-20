/* ui_actions.h - What every menu button does: buying, equipping, settings and the soundtrack. */
#ifndef NCS_UI_ACTIONS_H
#define NCS_UI_ACTIONS_H

/* volume sliders: the bar sits in the right part of the row */
#define SLIDER_X0 (FW / 2 + 10)
#define SLIDER_W 90

extern float resetArmT; /* when RESET SETTINGS was clicked once; a second click within 3 s does it */

/* UI STYLE: every preset is free - picking one switches the whole interface over at once */
void pickUi(int i);
void setDevMode(int on);
void flipSkinPage(int dir) /* also flips the quest list pages */;
void setSlider(int a, int pct);
int isSlider(int a);
/* the slider value under the mouse: volumes snap to 5%, MAX UPGRADES to whole cards */
int sliderAtMouse(int a);
/* SOUNDTRACK: BY MAP, RANDOM, then every song by name, round and round */
const char *musicModeName(void);
void setMusicMode(int m);
void activate(int a);

#endif
