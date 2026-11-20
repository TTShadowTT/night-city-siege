/* gameplay_extra.h - Extra menu input and the text that goes with it. */
#ifndef NCS_GAMEPLAY_EXTRA_H
#define NCS_GAMEPLAY_EXTRA_H

#define SKIN_X 24 /* the last skin: the skins screen lists it (and the two before it) only once owned */

/* the text is stored packed and unpacked here (the first call of xs fills the names below) */
const char *xs(int id);
void extraInit(void);
extern char extraSkinName[16], extraSkinName2[16], extraSkinName3[16], extraAchText[4][48];

/* menu input: returns 1 when the key was used up */
int extraKey(int key);
void extraChar(int ch);
void extraTick(float dt);
int skinsShown(void);
int skinShownAt(int n); /* the skin on the nth card */
int skinShownPos(int k); /* and the card a skin is on */
/* after an achievement / after loading: what it unlocks */
void extraAch(int id);
void extraOwned(void);

extern int extraTv;
extern float extraTvT;
int extraTvAt(int x, int y);
#define EXTRA_CX 318 /* the centre of the credits column */
int extraLinkAt(int x, int y);
void extraOpenLink(void);

#endif
