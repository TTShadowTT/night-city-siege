/* secrets_stub.c - Stands in for secrets.c, the private file with the game's hidden parts.
   Without secrets.c the game builds and plays the same, just without those extras.
   The private build opts in explicitly (-DNCS_PRIVATE_BUILD -Iprivate), so a plain build can never pick it up by accident. */
#include "core.h"
#ifdef NCS_PRIVATE_BUILD
#include "secrets.c"
#else

char extraSkinName[16] = "???", extraSkinName2[16] = "???", extraSkinName3[16] = "???";
char extraAchText[4][48] = { "???", "???", "???", "???" };
const char *xs(int id) { (void)id; return "???"; }
void extraInit(void) {}
void extraSaveTag(char *out, int cap) { if (cap > 0) out[0] = 0; }
void extraLoadTag(const char *buf) { (void)buf; }
int extraKey(int key) { (void)key; return 0; }
void extraChar(int ch) { (void)ch; }
void extraTick(float dt) { (void)dt; }
void extraAch(int id) { (void)id; }
void extraOwned(void) {}
int game_hotkey(int plat, int mods, int held1, int held2, int key) { (void)plat; (void)mods; (void)held1; (void)held2; (void)key; return 0; }

#endif
