/* gameplay_achievements.c - Achievements: one-off goals that each pay coins once, listed on their own screen. */
#include "core.h"

const Achievement ACHS[ACH_COUNT] = {
    { "FIRST BLOOD", "SLAY YOUR FIRST ENEMY", 50, 0 },
    { "UNTOUCHABLE", "CLEAR A WAVE WITHOUT GETTING HIT", 150, 0 },
    { "GUNSHIP ACE", "DOWN A GUNSHIP WITHOUT GETTING HIT IN ITS WAVE", 400, 0 },
    { "GIANT SLAYER", "BEAT A BIG BOSS (EVERY 20TH WAVE)", 500, 0 },
    { "SMASHED", "BEAT ADAM SMASHER", 800, 0 },
    { "ROCKET MAN", "ROCKET JUMP: FIRE A ROCKET AT YOUR FEET", 100, 0 },
    { "CLOSE CALL", "CLEAR A WAVE WITH 10 HEALTH OR LESS", 300, 0 },
    { "EVENT HORIZON", "CRUSH 5 ENEMIES WITH ONE BLACK HOLE", 300, 0 },
    { "OVERCLOCKED", "TAKE 10 UPGRADES IN ONE RUN", 200, 0 },
    { "COMBO GOD", "REACH A 100 KILL COMBO", 1000, 0 },
    { "ENDLESS LEGEND", "SURVIVE 40 WAVES IN ENDLESS", 1500, 0 },
    { "CITY TOUR", "OWN EVERY MAP", 1000, 0 },
    { "FASHION ICON", "OWN 10 PLAYER SKINS", 500, 0 },
    { "ZOOKEEPER", "ADOPT 10 PETS", 500, 0 },
    { "HARD BOILED", "CLEAR A DISTRICT ON HARD MODE", 1000, 0 },
    { "PLAYER TWO", "PLAY WITH A CONTROLLER", 100, 0 },
    { extraAchText[0], extraAchText[1], 300, 1 },
    { "LEGEND OF NIGHT CITY", "UNLOCK EVERY OTHER ACHIEVEMENT", 3000, 0 },
    { extraAchText[2], extraAchText[3], 250, 1 },
};
int achMask, achPage;
int achAt(int slot) { return slot < ACH_ALL ? slot : slot == ACH_COUNT - 1 ? ACH_ALL : slot + 1; }

int achDone(int id) { return (achMask >> id) & 1; }
int achCount(void)
{
    int n = 0;
    for (int i = 0; i < ACH_COUNT; i++) n += achDone(i);
    return n;
}

void achGive(int id)
{
    char s[64];
    if (bot || achDone(id)) return;
    achMask |= 1 << id;
    coins += ACHS[id].coins;
    sfmt(s, sizeof s, "ACHIEVEMENT: %s  +%d", ACHS[id].name, ACHS[id].coins);
    toast(s);
    sfx(S_WAVE, 0.6f);
    extraAch(id);
    if (id != ACH_ALL && achCount() == ACH_COUNT - 1) achGive(ACH_ALL);
    saveGame();
}

static int bits(int m)
{
    int n = 0;
    for (; m; m &= m - 1) n++;
    return n;
}

/* the ones about what you own: checked after buying, and when the game starts */
void achCheckOwned(void)
{
    int pets = 0;
    extraOwned();
    for (int i = 1; i < PET_COUNT; i++) pets += i < 31 ? (petsMask >> i) & 1 : (petsMask2 >> (i - 31)) & 1;
    if ((mapsMask & ((1 << D_COUNT) - 1)) == (1 << D_COUNT) - 1) achGive(ACH_ALL_MAPS);
    if (bits(skinsMask & ((1 << SKIN_COUNT) - 1)) >= 10) achGive(ACH_SKINS10);
    if (pets >= 10) achGive(ACH_PETS10);
}
