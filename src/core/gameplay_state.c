/* gameplay_state.c - The running game: player, enemies, shots, particles, waves and the screen state. */
#include "core.h"

/* fire rate, fastest to slowest: minigun, pistol, shotgun, grenades, rockets, railgun */
const Weapon WEAP[W_COUNT] = {
    { "PISTOL", 0.16f, 2.0f, 0.02f, 420, 1.2f, 0.9f, 1, 12, 7, 2, S_PISTOL, C(200, 200, 210), &sIcPistol },
    { "MINIGUN", 0.045f, 0.9f, 0.1f, 470, 1.0f, 2.6f, 1, 100, 11, 2, S_SMG, C(110, 170, 230), &sIcSmg },
    { "SHOTGUN", 0.55f, 1.6f, 0.4f, 380, 0.3f, 1.6f, 7, 6, 11, 2, S_SHOTGUN, C(190, 130, 80), &sIcShotgun },
    { "RAILGUN", 1.4f, 14.0f, 0.0f, 0, 0, 2.2f, 1, 4, 12, 2, S_LASER, C(90, 240, 255), &sIcLaser },
    { "GRENADES", 0.8f, 12.0f, 0.03f, 270, 1.1f, 2.0f, 1, 6, 9, 3, S_LAUNCH, C(120, 180, 100), &sIcGrenade },
    { "ROCKETS", 1.0f, 16.0f, 0.01f, 300, 2.0f, 2.4f, 1, 4, 12, 3, S_ROCKET, C(190, 170, 90), &sIcRocket },
    { "GRAVITY GUN", 1.5f, 0.0f, 0.0f, 200, 0.7f, 3.5f, 1, 2, 10, 3, S_WARP, C(180, 120, 255), &sIcGravity },
};
const Perk PERKS[PK_COUNT] = {
    { "TOUGH SKIN", "+25 MAX HEALTH", "AND HEAL 25", 4, C(90, 230, 110) },
    { "HOLLOW POINTS", "+20% DAMAGE", "ALL WEAPONS", 5, C(255, 110, 80) },
    { "QUICK HANDS", "RELOAD 30%", "FASTER", 3, C(255, 220, 90) },
    { "DRUM MAGS", "+50% BULLETS", "PER MAGAZINE", 3, C(200, 200, 220) },
    { "HAIR TRIGGER", "FIRE 15%", "FASTER", 4, C(255, 160, 60) },
    { "VAMPIRE", "HEAL 2 HP", "FOR EVERY KILL", 3, C(220, 40, 70) },
    { "SPRING BOOTS", "ONE MORE JUMP", "IN THE AIR", 2, C(120, 200, 255) },
    { "BLINK DRIVE", "DASH RECHARGES", "35% FASTER", 2, C(150, 210, 255) },
    { "LUCKY CHARM", "MORE MEDKITS", "+50% COINS", 2, C(255, 214, 80) },
    { "BIG BADA BOOM", "EXPLOSIONS AND", "BLACK HOLES 30% BIGGER", 3, C(255, 140, 40) },
    { "PIERCING ROUNDS", "BULLETS GO THROUGH", "ONE MORE ENEMY", 2, C(140, 250, 255) },
    { "KEVLAR", "TAKE 15% LESS", "DAMAGE", 3, C(150, 160, 190) },
    { "SNEAKERS", "RUN 12%", "FASTER", 2, C(120, 255, 180) },
};
int perkLimitOn = 1, perkLimit = PERK_DEFAULT;
int perkCap(void) { return perkLimitOn ? perkLimit : 1 << 30; } /* limit OFF = infinite upgrades */
int perk[PK_COUNT], perkChoice[3], perksTaken, perkPending;
/* how strong each upgrade is, summed over its cards: the first 10 cards of a run count in full, later ones
   less and less, so a long run can't snowball into god mode. Effects with steps (reload, dash, jumps, pierce)
   still use the card count, but those max out early. */
float perkPow[PK_COUNT];
float perkWeight(int nTaken) { return nTaken < PERK_DEFAULT ? 1 : nTaken < 20 ? 0.5f : nTaken < PERK_TOTAL ? 0.3f : 0.15f; }
float perkOpenT; /* when the cards appeared, so a stray click or weapon key doesn't pick one instantly */
int focusBtn;    /* highlighted button / card on any menu screen */

Shot shots[MAXS];
EShot eshots[MAXES];
Part parts[MAXP];
Boom booms[MAXBOOM];
Beam beams[MAXBEAM];
Pickup pu[MAXPU];
Floater fl[MAXF];
Enemy en[MAXE];
RopeFx ropeFx[MAXRFX];
Hole holes[MAXHOLE];

int state = ST_MENU, mode = MODE_CAMPAIGN;
float gt;
/* player */
float px = FW / 2.f, py = GROUND_Y - PHH, pvx, pvy, aim, inv, dashT, dashCd, fireCd, muzzleT, runAnim, dropT, coyote;
int hp, facing = 1, curW, mag[W_COUNT], onGround, onPlat = -1, ropeIdx = -1, jumps, airDash, dashDir;
float reloadT, reloadMax = 1, hurtFlash;
int maxHp = MAX_HP, selfHit;
/* run */
int score, newHi, area, startMap, waveIdx, waveKind, toSpawn, combo, bossIdx = -1, runCoins;
int runWave; /* waves fought this run, across districts: every 20th brings a big boss */
int totalKills, bossKills, deaths, stallKills, runKills, bestCombo, streakN, waveHurt;
float spawnCd, bannerT, interT, comboT, shake, overT, stallT, supplyT, streakT, hitStop, coinFrac;
char callout[24];
float calloutT;
unsigned calloutCol;
/* settings */
int menuMusMap; /* the district behind the menu, for the soundtrack */
int musicOn = 1, perksOn = 1, shakeOn = 1, sfxOn = 1; /* saved as one bit field: 1 = music, 2 = upgrade cards OFF, 4 = shake OFF */
int menuBg = BG_LAST; /* the district behind the main menu: one map, the last one played, a random one, or the picture */
int picUndone;        /* one build made the picture the default; saves from it go back to LAST PLAYED once (settings bit 2048) */
const TitleStyle TSTY[] = {
    { "NEON", 0, SC_NEON }, { "CHROME", 0, SC_CHROME }, { "FIRE", 0, SC_FIRE }, { "RAINBOW", 0, SC_RAINBOW }, { "GOLD", 0, SC_GOLD },
    { "TOXIC", 0, SC_TOXIC }, { "GLITCH", 0, SC_GLITCH }, { "SYNTHWAVE", 0, SC_SYNTH }, { "ICE", 0, SC_ICE }, { "CLEAN", 0, SC_CLEAN },
    { "BOLD", FX_BOLD, SC_CLEAN }, { "RACER", FX_ITALIC | FX_BOLD, SC_CHROME }, { "TALL", FX_TALL, SC_ICE }, { "WAVY", FX_WAVE, SC_RAINBOW },
    { "BUBBLE", FX_BUBBLE | FX_BOLD, SC_VAPOR }, { "DOTS", FX_DOTS, SC_PINK }, { "OUTLINE", FX_HOLLOW, SC_SYNTH }, { "HORROR", FX_SHAKY, SC_BLOOD },
    { "STENCIL", FX_STENCIL | FX_BOLD, SC_GOLD }, { "MATRIX", FX_DOTS, SC_MATRIX }, { "WIDE", FX_WIDE, SC_FIRE }, { "VAPOR", FX_ITALIC, SC_VAPOR },
    { "BLOOD", 0, SC_BLOOD }, { "SKATER", FX_WAVE | FX_BOLD, SC_TOXIC },
    /* night city: street gangs, corps, chrome and the edge */
    { "EDGERUNNER", FX_ITALIC, SC_EDGE }, { "CYBERPSYCHO", FX_SHAKY, SC_PSYCHO }, { "SANDEVISTAN", FX_ITALIC, SC_SANDY },
    { "NETRUNNER", FX_HOLLOW, SC_NET }, { "ARASAKA", FX_BOLD, SC_ARASAKA }, { "SAMURAI", FX_STENCIL | FX_BOLD, SC_SAMURAI },
    { "HOLOGRAM", 0, SC_HOLO }, { "MAXTAC", FX_BOLD, SC_MAXTAC }, { "BRAINDANCE", FX_WAVE, SC_BD }, { "KABUKI", FX_DOTS, SC_KABUKI },
    { "AFTERLIFE", FX_TALL, SC_AFTER }, { "STREET KID", FX_WIDE | FX_BOLD, SC_STREET },
    /* brush-slash letters with spiky tails; line 2 becomes a boxed tag */
    { "CYBER YELLOW", FX_SLASH | FX_LABEL, SC_CP }, { "CYBER RED", FX_SLASH | FX_LABEL, SC_CPRED }, { "CYBER CYAN", FX_SLASH | FX_LABEL, SC_CPCYAN },
    { "SLASHER", FX_SLASH, SC_BLOOD }, { "CHROME SLASH", FX_SLASH, SC_CHROME }, { "TAGLINE", FX_BOLD | FX_LABEL, SC_PINK } };
_Static_assert(sizeof TSTY / sizeof TSTY[0] == TS_COUNT, "TS_COUNT must match the entries in TSTY");
int tsPage; /* the page of style cards on the MENU TITLE screen */
int songDel = -1; /* MY SONGS: the song whose X was clicked once */
char songSearch[SEARCH_MAX + 1]; /* SOUNDTRACK: only songs with this in their name are listed */
int searchEdit;                  /* 1 while typing in the search box */
int sndLblGame, sndLblMine, sndGameAny, sndMineAny; /* where the list put its section labels (for drawing) */
int titleOwned, titleStyle, titleEdit, titleFresh; /* titleEdit: the line being typed (1 or 2), 0 = none; titleFresh: the next letter replaces the old text */
char title1[TITLE1_MAX + 1] = "NIGHT CITY", title2[TITLE2_MAX + 1] = "SIEGE";
int uiStyle;          /* UI STYLE preset: free to pick, saved in the settings bits from 16 up */
UiStyle ui;           /* the current UI STYLE's colors, refreshed every frame */
int devMode;          /* developer mode: everything counts as unlocked while it is on (settings bit 8) */
int optFromPause, sliderDrag, skinTab; /* OPTIONS opened from the pause screen; slider being dragged; skins screen tab */
int cursorSkin; /* the mouse cursor look, picked on the CURSORS tab of the skins screen (all free) */
float musicSent = -1;
int areaClear;
char banner[48], banner2[48];
/* input */
int mx = FW / 2, my = FH / 2, mdown, dashReq, held[4], jumpPress, upPress, downPress, jumpHeld, wantsQuit;
int dmgNumsOn = 1;
int hardOn, hardUnlocked;
float bossCardT;
char bossCardName[32], bossCardSub[48], bossCardLine[48];
unsigned bossCardCol;
int fullOn, fullReq; /* fullscreen: fullOn is reported by the platform, fullReq asks it to toggle */
int bot, god;
float botWeaponT;

int level(void) { return mode == MODE_ENDLESS ? waveIdx : area * 5 + waveIdx; }

/* weapon numbers after upgrades */
int magSize(int w) { return WEAP[w].mag + (int)(WEAP[w].mag * perkPow[PK_MAG] / 2 + 0.5f); }
float dmgMul(void) { return 1 + 0.2f * perkPow[PK_DMG]; }
float blastMul(void) { return 1 + 0.3f * fminf(perkPow[PK_BLAST], 6); }
float dashMax(void) { return perk[PK_DASH] ? 0.7f * (perk[PK_DASH] > 1 ? 0.42f : 0.65f) : 0.7f; }
