/* gameplay_progress.c - Progression: coins, quests, skins, enemy skins and the Sandevistan trail. */
#include "core.h"

const Skin SKINS[SKIN_COUNT] = {
    { "ROOKIE", { C(44, 32, 44), C(80, 230, 255), C(220, 60, 70), C(58, 68, 108), C(92, 106, 160), C(40, 40, 58), C(66, 66, 90), C(28, 24, 30) }, 0, -1, { 0 } },
    { "CRIMSON", { C(20, 18, 24), C(240, 240, 250), C(30, 30, 36), C(150, 30, 40), C(200, 60, 64), C(36, 32, 40), C(60, 54, 64), C(20, 18, 22) }, 250, -1, { 0 } },
    { "NEON PUNK", { C(255, 80, 200), C(120, 255, 140), C(60, 255, 220), C(80, 40, 120), C(130, 70, 180), C(30, 30, 50), C(50, 50, 80), C(40, 20, 50) }, 500, -1, { 0 } },
    { "ARCTIC", { C(230, 236, 245), C(80, 200, 255), C(140, 200, 255), C(200, 210, 226), C(245, 248, 255), C(110, 120, 140), C(150, 160, 180), C(70, 80, 100) }, -1, 1, { 0 } },
    { "GOLD AGENT", { C(30, 24, 20), C(255, 230, 120), C(30, 24, 20), C(200, 150, 40), C(250, 210, 90), C(40, 36, 30), C(70, 62, 50), C(30, 24, 20) }, -1, 8, { 0 } },
    { "SHADOW", { C(14, 12, 18), C(255, 40, 40), C(160, 20, 30), C(26, 24, 32), C(46, 44, 58), C(18, 16, 22), C(34, 32, 42), C(10, 10, 14) }, 1500, -1, { 0 } },
    { "MECH-7", { C(90, 96, 112), C(255, 60, 60), C(255, 200, 60), C(120, 128, 146), C(170, 178, 196), C(70, 74, 88), C(110, 116, 132), C(40, 42, 52) }, 800, -1,
      { &sRobot0, &sRobot1, &sRobot2, &sRobot3 } },
    { "NINJA", { C(30, 28, 40), C(255, 220, 160), C(220, 40, 50), C(36, 34, 48), C(60, 58, 78), C(30, 28, 40), C(50, 48, 64), C(20, 18, 26) }, 1000, -1,
      { &sNinja0, &sNinja1, &sNinja2, &sNinja3 } },
    { "ASTRONAUT", { C(255, 255, 255), C(255, 190, 60), C(255, 120, 40), C(225, 228, 236), C(250, 250, 255), C(200, 204, 214), C(235, 238, 245), C(90, 96, 110) }, -1, 10,
      { &sAstro0, &sAstro1, &sAstro2, &sAstro3 } },
    { "NEKO", { C(255, 170, 70), C(80, 255, 120), C(255, 130, 170), C(70, 60, 110), C(110, 96, 160), C(50, 46, 70), C(76, 70, 100), C(30, 28, 40) }, 600, -1,
      { &sNeko0, &sNeko1, &sNeko2, &sNeko3 } },
    { "BONES", { C(232, 226, 206), C(255, 90, 40), C(200, 40, 40), C(30, 28, 36), C(220, 214, 192), C(200, 194, 172), C(232, 226, 206), C(170, 164, 146) }, -1, 11,
      { &sBones0, &sBones1, &sBones2, &sBones3 } },
    { "DAVID", { C(60, 40, 34), C(0, 0, 0), C(0, 0, 0), C(240, 196, 30), C(255, 236, 120), C(78, 80, 66), C(104, 106, 90), C(40, 40, 44) }, 1200, -1,
      { &sDavid0, &sDavid1, &sDavid2, &sDavid3 } },
    { "LUCY", { C(226, 230, 246), C(150, 120, 200), C(80, 230, 200), C(100, 56, 96), C(244, 246, 252), C(150, 118, 150), C(180, 150, 180), C(50, 40, 56) }, 1500, -1,
      { &sLucy0, &sLucy1, &sLucy2, &sLucy3 } },
    { "REBECCA", { C(130, 230, 160), C(255, 160, 40), C(120, 255, 90), C(30, 28, 40), C(52, 48, 66), C(244, 206, 200), C(255, 120, 170), C(30, 30, 40) }, 1300, -1,
      { &sRebecca0, &sRebecca1, &sRebecca2, &sRebecca3 } },
    { "MAINE", { C(240, 220, 150), C(0, 0, 0), C(210, 30, 40), C(26, 24, 30), C(60, 56, 66), C(36, 34, 40), C(58, 56, 64), C(24, 22, 26) }, 1800, -1,
      { &sMaine0, &sMaine1, &sMaine2, &sMaine3 } },
    { "KIWI", { C(240, 230, 186), C(0, 0, 0), C(0, 0, 0), C(180, 34, 38), C(230, 80, 70), C(70, 30, 36), C(100, 44, 50), C(24, 18, 22) }, 1400, -1,
      { &sKiwi0, &sKiwi1, &sKiwi2, &sKiwi3 } },
    { "KNIGHT", { C(170, 176, 190), C(0, 0, 0), C(220, 40, 50), C(150, 156, 170), C(200, 206, 220), C(110, 116, 130), C(150, 156, 170), C(70, 74, 86) }, 900, -1,
      { &sKnight0, &sKnight1, &sKnight2, &sKnight3 } },
    { "DINO", { C(90, 200, 90), C(30, 30, 30), C(255, 200, 60), C(90, 200, 90), C(200, 240, 150), C(70, 160, 70), C(100, 200, 100), C(60, 120, 60) }, 700, -1,
      { &sDino0, &sDino1, &sDino2, &sDino3 } },
    { "FALCO", { C(42, 34, 32), C(30, 28, 34), C(200, 60, 40), C(104, 64, 40), C(146, 96, 60), C(40, 40, 50), C(62, 62, 76), C(32, 26, 24) }, 1400, -1,
      { &sFalco0, &sFalco1, &sFalco2, &sFalco3 } },
    { "GLORIA", { C(110, 22, 34), C(230, 150, 40), C(230, 40, 50), C(196, 174, 40), C(226, 206, 90), C(40, 40, 50), C(62, 62, 76), C(30, 26, 30) }, 1300, -1,
      { &sGloria0, &sGloria1, &sGloria2, &sGloria3 } },
    { "DORIO", { C(240, 210, 120), C(0, 0, 0), C(0, 0, 0), C(110, 60, 120), C(150, 92, 162), C(40, 32, 44), C(64, 54, 70), C(28, 22, 30) }, 1700, -1,
      { &sDorio0, &sDorio1, &sDorio2, &sDorio3 } },
    { "PILAR", { C(22, 20, 26), C(0, 0, 0), C(0, 0, 0), C(40, 40, 50), C(196, 202, 218), C(52, 42, 46), C(76, 64, 68), C(30, 24, 24) }, 1500, -1,
      { &sPilar0, &sPilar1, &sPilar2, &sPilar3 } },
    { extraSkinName2, { C(214, 214, 224), C(0, 0, 0), C(0, 0, 0), C(80, 36, 130), C(120, 64, 180), C(50, 26, 80), C(76, 44, 116), C(170, 120, 40) }, -1, -2,
      { &sSkinY_0, &sSkinY_1, &sSkinY_2, &sSkinY_3 } },
    { extraSkinName3, { C(44, 26, 54), C(0, 0, 0), C(0, 0, 0), C(30, 130, 90), C(64, 182, 132), C(24, 100, 70), C(40, 130, 90), C(200, 160, 50) }, -1, -2,
      { &sSkinZ_0, &sSkinZ_1, &sSkinZ_2, &sSkinZ_3 } },
    { extraSkinName, { C(36, 32, 48), C(255, 40, 60), C(210, 20, 44), C(22, 20, 30), C(78, 72, 100), C(28, 26, 36), C(48, 44, 60), C(12, 10, 16) }, -1, -2,
      { &sSkinX_0, &sSkinX_1, &sSkinX_2, &sSkinX_3 } },
};
static const EnemyBody BODY_ARA = { { &sEAraThug0, &sEAraThug1, &sEAraGun0, &sEAraGun1, &sEAraBrute0, &sEAraBrute1, &sEAraDrone0, &sEAraDrone1 },
    { C(44, 44, 54), C(255, 40, 50), C(210, 30, 40), C(50, 50, 62), C(96, 96, 116), C(34, 34, 42), C(60, 60, 72), C(18, 18, 22) } };
static const EnemyBody BODY_MAX = { { &sEMaxThug0, &sEMaxThug1, &sEMaxGun0, &sEMaxGun1, &sEMaxBrute0, &sEMaxBrute1, &sEMaxDrone0, &sEMaxDrone1 },
    { C(46, 48, 56), C(255, 150, 30), C(255, 200, 40), C(50, 54, 64), C(104, 110, 126), C(36, 38, 46), C(62, 66, 78), C(20, 20, 24) } };
static const EnemyBody BODY_TYG = { { &sETygThug0, &sETygThug1, &sETygGun0, &sETygGun1, &sETygBrute0, &sETygBrute1, &sETygDrone0, &sETygDrone1 },
    { C(84, 44, 150), C(80, 255, 255), C(255, 60, 200), C(40, 30, 60), C(90, 70, 130), C(30, 26, 40), C(56, 50, 70), C(230, 230, 240) } };
static const EnemyBody BODY_PSY = { { &sEPsyThug0, &sEPsyThug1, &sEPsyGun0, &sEPsyGun1, &sEPsyBrute0, &sEPsyBrute1, &sEPsyDrone0, &sEPsyDrone1 },
    { C(40, 10, 20), C(255, 0, 80), C(255, 0, 80), C(120, 20, 50), C(170, 40, 80), C(50, 40, 60), C(80, 70, 90), C(30, 24, 30) } };
static const EnemyBody BODY_SCV = { { &sEScvThug0, &sEScvThug1, &sEScvGun0, &sEScvGun1, &sEScvBrute0, &sEScvBrute1, &sEScvDrone0, &sEScvDrone1 },
    { C(70, 52, 36), C(255, 200, 60), C(200, 90, 40), C(110, 90, 60), C(150, 124, 84), C(70, 60, 46), C(100, 86, 64), C(46, 36, 28) } };
static const EnemyBody BODY_SMA = { { &sESmaThug0, &sESmaThug1, &sESmaGun0, &sESmaGun1, &sESmaBrute0, &sESmaBrute1, &sESmaDrone0, &sESmaDrone1 },
    { C(56, 60, 70), C(255, 30, 30), C(255, 30, 30), C(64, 68, 80), C(130, 136, 150), C(44, 46, 54), C(90, 94, 106), C(30, 30, 36) } };
static const EnemyBody BODY_GANG = { { &sEGangThug0, &sEGangThug1, &sEGangGun0, &sEGangGun1, &sEGangBrute0, &sEGangBrute1, &sEGangDrone0, &sEGangDrone1 },
    { C(30, 26, 34), C(255, 214, 90), C(220, 40, 50), C(140, 36, 52), C(186, 64, 72), C(50, 60, 90), C(72, 86, 124), C(28, 24, 30) } };
static const EnemyBody BODY_ZOM = { { &sEZomThug0, &sEZomThug1, &sEZomGun0, &sEZomGun1, &sEZomBrute0, &sEZomBrute1, &sEZomDrone0, &sEZomDrone1 },
    { C(110, 150, 90), C(255, 40, 40), C(130, 20, 24), C(80, 90, 110), C(110, 120, 140), C(60, 50, 70), C(85, 72, 96), C(50, 44, 40) } };
static const EnemyBody BODY_BOT = { { &sEBotThug0, &sEBotThug1, &sEBotGun0, &sEBotGun1, &sEBotBrute0, &sEBotBrute1, &sEBotDrone0, &sEBotDrone1 },
    { C(140, 148, 165), C(80, 220, 255), C(255, 80, 80), C(100, 106, 120), C(160, 166, 180), C(70, 76, 92), C(110, 118, 136), C(50, 54, 64) } };
static const EnemyBody BODY_CLOWN = { { &sEClownThug0, &sEClownThug1, &sEClownGun0, &sEClownGun1, &sEClownBrute0, &sEClownBrute1, &sEClownDrone0, &sEClownDrone1 },
    { C(255, 110, 40), C(40, 40, 40), C(255, 40, 40), C(160, 60, 220), C(210, 120, 255), C(60, 90, 230), C(100, 140, 255), C(255, 40, 40) } };
static const EnemyBody BODY_ALIEN = { { &sEAlienThug0, &sEAlienThug1, &sEAlienGun0, &sEAlienGun1, &sEAlienBrute0, &sEAlienBrute1, &sEAlienDrone0, &sEAlienDrone1 },
    { C(140, 230, 150), C(16, 16, 20), C(255, 80, 255), C(70, 50, 130), C(110, 90, 190), C(50, 40, 90), C(80, 64, 140), C(30, 24, 50) } };
#define ESKIN_KEYS "tTmenNgGdDcCRxXouUyqS"
const EnemySkin ESKINS[ESKIN_COUNT] = {
    { "STREET GANG", 0, { 0 }, C(170, 16, 24), C(120, 8, 16), 0, "THE ORIGINAL CREW", &BODY_GANG },
    { "ZOMBIES", 400,
      { C(70, 90, 60), C(100, 120, 80), C(120, 170, 100), C(255, 40, 40), C(60, 50, 70), C(85, 72, 96), C(110, 150, 90), C(140, 180, 110),
        C(50, 44, 40), C(70, 62, 56), C(90, 110, 80), C(130, 150, 110), C(180, 255, 90), C(80, 100, 70), C(130, 160, 110), C(180, 255, 90),
        C(90, 120, 70), C(120, 160, 90), C(255, 60, 60), C(200, 255, 120), C(120, 170, 100) },
      C(90, 180, 40), C(50, 110, 20), 0, "THEY BLEED GREEN", &BODY_ZOM },
    { "ROBOTS", 700,
      { C(90, 96, 110), C(140, 148, 165), C(30, 34, 44), C(80, 220, 255), C(70, 76, 92), C(110, 118, 136), C(100, 110, 125), C(150, 160, 175),
        C(50, 54, 64), C(80, 86, 100), C(150, 156, 170), C(200, 206, 220), C(80, 220, 255), C(110, 116, 130), C(170, 176, 190), C(80, 220, 255),
        C(100, 106, 120), C(150, 156, 172), C(80, 220, 255), C(255, 80, 80), C(160, 166, 180) },
      C(40, 40, 48), C(255, 160, 50), 0, "OIL AND SPARKS", &BODY_BOT },
    { "CLOWNS", 1000,
      { C(230, 60, 160), C(255, 120, 200), C(250, 250, 250), C(40, 40, 40), C(60, 90, 230), C(100, 140, 255), C(250, 200, 40), C(255, 230, 90),
        C(40, 160, 80), C(70, 200, 110), C(255, 120, 40), C(255, 180, 80), C(255, 40, 40), C(160, 60, 220), C(210, 120, 255), C(255, 255, 255),
        C(255, 90, 60), C(255, 150, 110), C(60, 220, 255), C(255, 240, 80), C(250, 250, 250) },
      C(255, 80, 160), C(80, 200, 255), 1, "CONFETTI EVERYWHERE", &BODY_CLOWN },
    { "ALIENS", 1500,
      { C(60, 40, 110), C(100, 70, 170), C(120, 230, 140), C(20, 20, 20), C(50, 40, 90), C(80, 64, 140), C(70, 50, 130), C(110, 90, 190),
        C(30, 24, 50), C(50, 40, 80), C(120, 230, 140), C(170, 255, 190), C(255, 80, 255), C(90, 200, 120), C(150, 250, 180), C(255, 80, 255),
        C(70, 170, 110), C(110, 220, 150), C(255, 80, 255), C(255, 80, 255), C(120, 230, 140) },
      C(60, 230, 230), C(20, 150, 160), 0, "GLOWING GOO", &BODY_ALIEN },
    /* night city factions */
    { "ARASAKA", 1200,
      { C(30, 30, 36), C(60, 60, 70), C(20, 20, 24), C(255, 40, 40), C(40, 40, 48), C(70, 70, 82), C(36, 36, 44), C(200, 30, 40),
        C(24, 24, 30), C(46, 46, 56), C(230, 230, 235), C(255, 255, 255), C(255, 30, 40), C(40, 40, 48), C(220, 30, 40), C(255, 60, 60),
        C(30, 30, 38), C(200, 30, 40), C(255, 40, 40), C(255, 50, 60), C(225, 190, 160) },
      C(170, 16, 24), C(120, 8, 16), 0, "CORPO SECURITY", &BODY_ARA },
    { "MAXTAC", 1600,
      { C(24, 26, 30), C(50, 54, 62), C(18, 18, 22), C(255, 140, 30), C(28, 30, 36), C(56, 60, 70), C(30, 32, 38), C(62, 66, 76),
        C(20, 20, 24), C(40, 42, 48), C(46, 50, 58), C(90, 96, 108), C(255, 140, 30), C(36, 38, 44), C(80, 84, 96), C(255, 140, 30),
        C(36, 38, 46), C(70, 74, 86), C(255, 140, 30), C(255, 140, 30), C(200, 160, 130) },
      C(170, 16, 24), C(120, 8, 16), 0, "THE PSYCHO SQUAD", &BODY_MAX },
    { "TYGER CLAWS", 900,
      { C(30, 200, 210), C(80, 240, 250), C(30, 30, 40), C(255, 60, 200), C(40, 30, 60), C(70, 56, 100), C(255, 60, 180), C(255, 120, 210),
        C(30, 26, 40), C(56, 50, 70), C(40, 40, 60), C(255, 60, 200), C(80, 255, 255), C(255, 60, 180), C(255, 140, 220), C(80, 255, 255),
        C(30, 180, 190), C(80, 230, 240), C(255, 60, 200), C(80, 255, 255), C(240, 200, 170) },
      C(170, 16, 24), C(120, 8, 16), 0, "NEON GANG ON BIKES", &BODY_TYG },
    { "CYBERPSYCHOS", 2000,
      { C(170, 170, 185), C(220, 220, 232), C(40, 10, 20), C(255, 0, 80), C(120, 20, 50), C(180, 40, 80), C(150, 150, 165), C(210, 210, 225),
        C(60, 14, 30), C(100, 24, 50), C(200, 200, 215), C(250, 250, 255), C(255, 0, 80), C(140, 140, 155), C(220, 220, 235), C(255, 0, 80),
        C(160, 160, 175), C(220, 220, 235), C(255, 0, 80), C(255, 0, 80), C(200, 200, 212) },
      C(230, 250, 255), C(80, 220, 255), 0, "TOO MUCH CHROME", &BODY_PSY },
    { "SCAVS", 800,
      { C(120, 80, 50), C(160, 110, 70), C(60, 40, 30), C(255, 200, 60), C(80, 70, 50), C(110, 96, 70), C(110, 90, 60), C(150, 124, 84),
        C(56, 44, 34), C(80, 64, 48), C(140, 90, 50), C(190, 130, 80), C(255, 180, 40), C(120, 80, 50), C(170, 120, 80), C(255, 180, 40),
        C(110, 76, 48), C(150, 106, 70), C(255, 200, 60), C(255, 180, 40), C(210, 170, 130) },
      C(170, 16, 24), C(120, 8, 16), 0, "RUSTY CHOP-SHOP CREW", &BODY_SCV },
    { "ADAM SMASHER", 2500, /* every enemy a full-borg: gunmetal chrome, black faceplate, red optics */
      { C(56, 60, 70), C(110, 116, 130), C(16, 16, 20), C(255, 30, 30), C(44, 46, 54), C(90, 94, 106), C(60, 64, 74), C(130, 136, 150),
        C(30, 30, 36), C(70, 72, 82), C(90, 94, 106), C(170, 176, 190), C(255, 30, 30), C(60, 64, 74), C(150, 156, 170), C(255, 30, 30),
        C(64, 68, 80), C(140, 146, 160), C(255, 30, 30), C(255, 30, 30), C(120, 126, 140) },
      C(30, 30, 36), C(255, 120, 30), 0, "FULL BORG. NO MERCY", &BODY_SMA },
};
int eskin, eskinsMask = 1;
const EnemyBody *eBody;

void enemyLook(int k, int on)
{
    eTabOn = on && k > 0;
    eBody = on ? ESKINS[k].body : 0;
    if (!eTabOn) return;
    for (int i = 0; ESKIN_KEYS[i]; i++) eTab[(unsigned char)ESKIN_KEYS[i]] = ESKINS[k].col[i];
}

const Spr *enemyBodySpr(int slot, int f, const Spr *dflt) { return eBody ? eBody->s[slot + f] : dflt; }

/* frame f (0 idle, 1 run, 2 run, 3 jump) of skin k */
/* the colours a skin is drawn with: the last one's visor and veins pulse */
const unsigned *skinPal(int k)
{
    static unsigned p[8];
    if (k != SKIN_X) return SKINS[k].pal;
    int glow = 150 + (int)(106 * (0.5f + 0.5f * sinf(gt * 4)));
    for (int i = 0; i < 8; i++) p[i] = SKINS[k].pal[i];
    p[1] = scalec(p[1], glow); p[2] = scalec(p[2], glow);
    return p;
}

const Spr *skinFrame(int k, int f)
{
    static const Spr *const std_[4] = { &sPlayerIdle, &sPlayerRun1, &sPlayerRun2, &sPlayerJump };
    return SKINS[k].shape[f] ? SKINS[k].shape[f] : std_[f];
}
Ghost ghosts[MAXGHOST];
int ghostN, sandyOff;
int isDavid(int k) { return strncmp(SKINS[k].name, "DAVID", 6) == 0; }
void addGhost(float x, float feet, int face, int f)
{
    Ghost *g = &ghosts[ghostN % MAXGHOST];
    g->x = x; g->y = feet; g->face = face; g->f = f; g->life = GHOST_LIFE; g->n = ghostN++;
}
const Quest QUESTS[QUEST_COUNT] = {
    { "SLAY 100 ENEMIES", P_KILLS, 100, 150, -1 },
    { "DOWN A GUNSHIP", P_BOSSES, 1, 0, 3 },
    { "CLEAR A DISTRICT IN CAMPAIGN", P_DISTRICTS, 1, 200, -1 },
    { "30 KILLS WITH GRENADES OR ROCKETS", P_EXPLOSIVE, 30, 200, -1 },
    { "PICK UP 15 MEDKITS", P_MEDKITS, 15, 150, -1 },
    { "CLIMB 25 ROPES", P_ROPES, 25, 100, -1 },
    { "SCORE 5000 IN ONE RUN", P_BESTSCORE, 5000, 250, -1 },
    { "SURVIVE 10 WAVES IN ENDLESS", P_ENDLESS, 10, 300, -1 },
    { "SURVIVE 15 WAVES IN ENDLESS", P_ENDLESS, 15, 0, 4 },
    { "CRUSH 25 ENEMIES WITH THE GRAVITY GUN", P_GRAVITY, 25, 250, -1 },
    { "CYBERPSYCHOSIS - TAKE 25 UPGRADES", P_PERKS, 25, 0, 8 },
    { "TAKE DOWN 20 RIOT SHIELDS", P_SHIELDS, 20, 0, 10 },
    { "SLAY 1000 ENEMIES", P_KILLS, 1000, 1000, -1 },
    { "CLEAR 50 WAVES", P_WAVES, 50, 300, -1 },
    { "REACH A 30 KILL COMBO", P_COMBO, 30, 300, -1 },
    { "GET 50 MULTI-KILLS", P_DOUBLES, 50, 300, -1 },
    { "DASH 300 TIMES", P_DASHES, 300, 200, -1 },
    { "ROCKET JUMP 25 TIMES", P_RJUMPS, 25, 300, -1 },
    { "FINISH 10 DAILY QUESTS", P_DAILIES, 10, 500, -1 },
    { "EARN 5000 COINS IN RUNS", P_COINS, 5000, 500, -1 },
    { "DOWN 10 GUNSHIPS", P_BOSSES, 10, 600, -1 },
    { "SCORE 25000 IN ONE RUN", P_BESTSCORE, 25000, 600, -1 },
    { "REACH A 60 KILL COMBO", P_COMBO, 60, 700, -1 },
    { "SURVIVE 25 WAVES IN ENDLESS", P_ENDLESS, 25, 800, -1 },
    { "CLEAR 250 WAVES", P_WAVES, 250, 1000, -1 },
    { "FINISH 50 DAILY QUESTS", P_DAILIES, 50, 2000, -1 },
    { "SLAY 5000 ENEMIES", P_KILLS, 5000, 3000, -1 },
};
const int MAP_COST[D_COUNT] = { 0, 300, 600, 1000, 800, 1200, 1500, 2000, 2500, 3000, 2200, 3500, 4000, 4500, 5000 };

int coins, mapsMask = 1, skinsMask = 1, skin, questMask, pstat[P_COUNT];
/* what you can use right now: what you own, or everything in developer mode (the real masks stay untouched) */
int ownsMap(int i) { return devMode || ((mapsMask >> i) & 1); }
int ownsTitle(void) { return devMode || titleOwned; }
int ownsSkin(int i) { return devMode || ((skinsMask >> i) & 1); }
int ownsESkin(int i) { return devMode || ((eskinsMask >> i) & 1); }
