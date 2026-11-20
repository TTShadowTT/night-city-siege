/* gameplay_pets.c - Pets: the list of pets and their collars. */
#include "core.h"

const PetShape PET_SHAPES[PS_COUNT] = {
    { 13, 8, { /* cat: tail up behind, pointy ears */
        { ".b......b..b.", "b.......bbbb.", "b.......bbbeb", ".bbdbbdbcbbbn", "..bbbbbbcbll.", "..llllllb....", "..b.b..b.b...", "..b.b..b.b..." },
        { "b.......b..b.", "b.......bbbb.", ".b......bbbeb", ".bbdbbdbcbbbn", "..bbbbbbcbll.", "..llllllb....", "...b.b..b.b..", "..b...b.b...b" },
        { "b.......b..b.", "b.......bbbb.", ".b......bbbeb", ".bbdbbdbcbbbn", "..bbbbbbcbll.", "..llllllb....", "..bb...bb....", "..b.b..b.b..." } }, 0 },
    { 13, 8, { /* dog: floppy ear, wagging tail */
        { "........bbb..", "b......bbbbb.", ".b.....dbbebb", ".bbbbbbdcbbbn", ".bbdbbbbcbll.", ".lllllbbb....", ".b.b...b.b...", ".b.b...b.b..." },
        { "b.......bbb..", ".b.....bbbbb.", ".b.....dbbebb", ".bbbbbbdcbbbn", ".bbdbbbbcbll.", ".lllllbbb....", "..b.b...b.b..", ".b...b.b...b." },
        { ".b......bbb..", "b......bbbbb.", ".b.....dbbebb", ".bbbbbbdcbbbn", ".bbdbbbbcbll.", ".lllllbbb....", ".bb....bb....", ".b.b...b.b..." } }, 0 },
    { 9, 7, { /* hamster: a round fluff ball */
        { "...b..b..", "..bbbcbb.", ".bbbbcbeb", "bbdbbcbbn", "bllllcll.", ".llllll..", "..b..b..." },
        { "...b..b..", "..bbbcbb.", ".bbbbcbeb", "bbdbbcbbn", "bllllcll.", ".llllll..", ".b....b.." },
        { "...b..b..", "..bbbcbb.", ".bbbbcbeb", "bbdbbcbbn", "bllllcll.", ".llllll..", "...b..b.." } }, 0 },
    { 10, 9, { /* bunny: tall ears, cotton tail */
        { "......b.b.", "......b.b.", "......b.b.", ".....bbbb.", "l....bbeb.", "lbbbbcbbbn", ".bbbbcbll.", ".bbbbbb...", ".bb...bb.." },
        { "......b.b.", ".....b..b.", ".....bb.b.", ".....bbbb.", "l....bbeb.", "lbbbbcbbbn", ".bbbbcbll.", ".bbbbbb...", "bb.....bb." },
        { ".......bb.", "......b.b.", "......b.b.", ".....bbbb.", "l....bbeb.", "lbbbbcbbbn", ".bbbbcbll.", ".bbbbbb...", "..bb.bb..." } }, 0 },
    { 8, 10, { /* penguin: upright, waddles */
        { "..bbbb..", ".bbbbwen", ".bbbbbb.", "bbCCCCCb", "bbllllb.", "bbllllbb", "bbllllb.", ".bllll..", ".bllll..", "..nn.nn." },
        { "..bbbb..", ".bbbbwen", ".bbbbbb.", "bbCCCCCb", "bbllllb.", "bbllllb.", "bbllllbb", ".bllll..", ".bllll..", ".nn..nn." },
        { "..bbbb..", ".bbbbwen", ".bbbbbb.", "bbCCCCCb", "bbllllb.", "bbllllbb", "bbllllb.", ".bllll..", ".bllll..", "..nn..nn" } }, 0 },
    { 10, 6, { /* frog: big eyes, hops */
        { ".....wb.wb", ".....eb.eb", "..bbbbbbbb", ".bbdbccbbn", ".bllllllb.", "bb.....bb." },
        { ".....wb.wb", ".....eb.eb", "..bbbbbbbb", ".bbdbccbbn", ".bllllllb.", ".bb...bb.." },
        { ".....wb.wb", ".....eb.eb", "..bbbbbbbb", ".bbdbccbbn", ".bllllllb.", "bb......bb" } }, 0 },
    { 12, 8, { /* baby dragon: horns, flapping wings, spiky tail */
        { ".......d..d.", "....dd.bbbb.", "b..dddbbbbeb", "bb.ddbbcbbbn", ".bbbbbbcbll.", "..llllbb....", "..b.b..b.b..", "..b.b..b.b.." },
        { "...dd..d..d.", "....dd.bbbb.", "b...ddbbbbeb", "bb..dbbcbbbn", ".bbbbbbcbll.", "..llllbb....", "...b.b..b.b.", "..b...b.b..b" },
        { ".......d..d.", ".......bbbb.", "b..ddddbbbeb", "bb.dddbcbbbn", ".bbbbbbcbll.", "..llllbb....", "..bb...bb...", "..b.b..b.b.." } }, 0 },
    { 12, 8, { /* bear: round ears, dark legs (a panda's patches are the dark colour too) */
        { ".......d..d.", "......bbbbb.", ".bbbbbbbbdeb", "bbbbbbbcbbbn", "bbbbbbbcbll.", "dddbbbbdd...", ".dd...dd....", ".dd...dd...." },
        { ".......d..d.", "......bbbbb.", ".bbbbbbbbdeb", "bbbbbbbcbbbn", "bbbbbbbcbll.", "dddbbbbdd...", "..dd...dd...", ".dd....dd..." },
        { ".......d..d.", "......bbbbb.", ".bbbbbbbbdeb", "bbbbbbbcbbbn", "bbbbbbbcbll.", "dddbbbbdd...", ".dd..dd.....", "..dd...dd..." } }, 0 },
    { 9, 8, { /* duck: waddles, wing flaps */
        { "....bbb..", "...bbbeb.", "...bbbbnn", "b...cbb..", "bbbbbbbb.", ".bdddbbb.", "..bbbbb..", "...n.n..." },
        { "....bbb..", "...bbbeb.", "...bbbbnn", "b...cbb..", "bbdddbbb.", ".bbbbbbb.", "..bbbbb..", "..n...n.." },
        { "....bbb..", "...bbbeb.", "...bbbbnn", "b...cbb..", "bbbbbbbb.", ".bdddbbb.", "..bbbbb..", "....nn..." } }, 0 },
    { 8, 9, { /* owl: big eyes, ear tufts, flapping wings */
        { ".d....d.", ".bbbbbb.", "bwebbweb", "bbbbnbbb", "dbCCCCbd", "dbllllbd", "dbllllbd", ".bllllb.", "..n..n.." },
        { ".d....d.", ".bbbbbb.", "bwebbweb", "dbbbnbbd", "dbCCCCbd", ".bllllb.", ".bllllb.", ".bllllb.", ".n....n." },
        { ".d....d.", ".bbbbbb.", "bwebbweb", "bbbbnbbb", "dbCCCCbd", "dbllllbd", "dbllllbd", ".bllllb.", "...nn..." } }, 0 },
    { 10, 7, { /* slime: a wobbly blob that squashes and stretches as it bounces along */
        { "...bbbb...", ".bbwbbbbb.", "bwbbbebeb.", "bbbbbbbbbb", "bbcccccccb", "dbbbbbbbbd", ".dddddddd." },
        { "..........", "..bbbbbb..", ".bwbbebeb.", "bbbbbbbbbb", "bbcccccccb", "dbbbbbbbbd", "dddddddddd" },
        { "..bbbbb...", ".bwbbbbbb.", ".bbbbebeb.", ".bbbbbbbb.", ".bccccccb.", ".bbbbbbbb.", "..dddddd.." } }, 0 },
    { 12, 6, { /* axolotl: frilly gills, long tail */
        { "........d.d.", ".......dbbbd", "bb....bbbbeb", ".bbbbbcbbbbn", "..lllllcll..", "..b.b..b.b.." },
        { "........d.d.", ".......dbbbd", ".bb...bbbbeb", "b.bbbbcbbbbn", "..lllllcll..", "...b.b..b.b." },
        { "........d.d.", ".......dbbbd", "bb....bbbbeb", ".bbbbbcbbbbn", "..lllllcll..", "..bb...bb..." } }, 0 },
    /* flying pets (fly = 1): they hover at your shoulder instead of walking; frames are wing beats */
    { 9, 5, { /* bat */
        { "d.......d", "dd.b.b.dd", ".ddbbbdd.", "..bebeb..", "...bnb..." },
        { ".........", "...b.b...", "dddbbbddd", "..bebeb..", "...bnb..." },
        { ".........", "...b.b...", "...bbb...", "ddbebebdd", "d..bnb..d" } }, 1 },
    { 9, 7, { /* butterfly */
        { "bb.....bb", "bdb...bdb", "bbdbebdbb", ".bbbebbb.", "..bbebb..", ".bdb.bdb.", ".bb...bb." },
        { ".........", ".bb...bb.", ".bdbebdb.", "..bbebb..", "...beb...", "..bdedb..", "..b...b.." },
        { ".........", "...b.b...", "...beb...", "...beb...", "...beb...", "...beb...", "........." } }, 1 },
    { 8, 6, { /* bee */
        { "...ww...", "..wwww..", ".bdbdbe.", "dbdbdbbn", ".bdbdbb.", "........" },
        { "........", "..wwww..", ".bdbdbe.", "dbdbdbbn", ".bdbdbb.", "........" },
        { "........", "...ww...", ".bdbdbe.", "dbdbdbbn", ".bdbdbb.", "........" } }, 1 },
    { 9, 5, { /* mini drone: spinning rotors, a red lens */
        { "nnn...nnn", ".d.....d.", ".dbbbbbd.", "..bbebb..", "...lll..." },
        { ".n.....n.", ".d.....d.", ".dbbbbbd.", "..bbebb..", "...lll..." },
        { "n.n...n.n", ".d.....d.", ".dbbbbbd.", "..bbebb..", "...l.l..." } }, 1 },
    { 7, 8, { /* ghost: a wavy hem */
        { "..bbb..", ".bbbbb.", "bbebebb", "bbbbbbb", "bbbnbbb", "bbbbbbb", "bbbbbbb", "b.b.b.b" },
        { "..bbb..", ".bbbbb.", "bbebebb", "bbbbbbb", "bbbnbbb", "bbbbbbb", "bbbbbbb", ".b.b.b." },
        { "..bbb..", ".bbbbb.", "bbebebb", "bbbbbbb", "bbbnbbb", "bbbbbbb", "bbbbbbb", "bb.b.bb" } }, 1 },
    { 8, 8, { /* parrot */
        { "....bbb.", "...bbwen", "...bbbnn", "..dbbcb.", "dddbbb..", ".ddbbb..", "l.dbb...", "l..d...." },
        { "....bbb.", "...bbwen", "...bbbnn", "..dbbcb.", ".ddbbb..", "..dbbb..", "l.dbb...", "l..d...." },
        { "....bbb.", "...bbwen", "...bbbnn", "...bbcb.", "..dbbb..", ".ddbbb..", "lddbb...", "l..d...." } }, 1 },
    { 9, 5, { /* tiny UFO: chasing lights */
        { "...lll...", "..lwlll..", ".bbbbbbb.", "bdbnbdbnb", "..n...n.." },
        { "...lll...", "..lwlll..", ".bbbbbbb.", "bnbdbnbdb", "...n.n..." },
        { "...lll...", "..lwlll..", ".bbbbbbb.", "bdbdbdbdb", "....n...." } }, 1 },
    { 11, 9, { /* sludge: a lopsided goo mound with a droopy tip and two slit eyes */
        { ".....bb....", "....bbbl...", "...bebeb...", "..bbebebb..", "..bbbbbbb..", ".bbbbbbbbb.", ".bdbbbbbdb.", "bbbbbbbbbbb", "..ddddddd.." },
        { "...........", ".....bbl...", "...bebeb...", "..bbebebb..", ".bbbbbbbbb.", ".bbbbbbbbb.", "bdbbbbbbbdb", "bbbbbbbbbbb", ".ddddddddd." },
        { "......b....", ".....bb....", "....bbl....", "...bebeb...", "...bebeb...", "..bbbbbbb..", ".bdbbbbbdb.", ".bbbbbbbbb.", "..ddddddd.." } }, 0 },
};
const Pet PETS[] = {
    { "NO PET", 0, 0, 0, 0, 0, 0, 0, 0 },
    { "ORANGE CAT", PS_CAT, 150, C(240, 150, 50), C(190, 100, 30), C(255, 230, 200), C(30, 30, 20), C(255, 255, 255), C(255, 140, 150) },
    { "BLACK CAT", PS_CAT, 200, C(40, 38, 48), C(28, 26, 34), C(70, 66, 80), C(255, 220, 60), C(255, 255, 255), C(200, 110, 130) },
    { "WHITE CAT", PS_CAT, 200, C(240, 240, 245), C(210, 210, 220), C(255, 255, 255), C(70, 160, 255), C(255, 255, 255), C(255, 150, 170) },
    { "GREY CAT", PS_CAT, 200, C(150, 150, 160), C(100, 100, 112), C(220, 220, 228), C(80, 200, 90), C(255, 255, 255), C(255, 150, 170) },
    { "PUPPY", PS_DOG, 250, C(170, 110, 60), C(110, 70, 40), C(235, 210, 180), C(25, 20, 20), C(255, 255, 255), C(30, 24, 24) },
    { "GOLDEN DOG", PS_DOG, 300, C(230, 180, 90), C(190, 135, 60), C(255, 235, 190), C(25, 20, 20), C(255, 255, 255), C(30, 24, 24) },
    { "BLACK DOG", PS_DOG, 300, C(46, 42, 46), C(30, 28, 30), C(150, 110, 70), C(25, 20, 20), C(255, 255, 255), C(10, 10, 10) },
    { "HUSKY", PS_DOG, 400, C(130, 136, 150), C(80, 84, 96), C(240, 242, 248), C(90, 190, 255), C(255, 255, 255), C(20, 20, 24) },
    { "GOLDEN HAMSTER", PS_HAMSTER, 150, C(225, 160, 80), C(180, 115, 50), C(255, 238, 210), C(20, 16, 16), C(255, 255, 255), C(255, 140, 160) },
    { "WHITE HAMSTER", PS_HAMSTER, 150, C(245, 245, 245), C(215, 215, 220), C(255, 255, 255), C(200, 30, 50), C(255, 255, 255), C(255, 150, 170) },
    { "GREY HAMSTER", PS_HAMSTER, 150, C(140, 140, 150), C(95, 95, 105), C(225, 225, 230), C(20, 16, 16), C(255, 255, 255), C(255, 150, 170) },
    { "WHITE BUNNY", PS_BUNNY, 300, C(245, 245, 250), C(220, 220, 230), C(255, 255, 255), C(220, 40, 70), C(255, 255, 255), C(255, 150, 170) },
    { "BROWN BUNNY", PS_BUNNY, 300, C(150, 105, 70), C(110, 75, 50), C(240, 225, 205), C(20, 16, 16), C(255, 255, 255), C(255, 150, 170) },
    { "FOX", PS_CAT, 500, C(240, 110, 30), C(60, 40, 30), C(255, 245, 235), C(30, 24, 16), C(255, 255, 255), C(30, 24, 24) },
    { "PENGUIN", PS_PENGUIN, 600, C(36, 40, 56), C(24, 26, 36), C(245, 245, 250), C(20, 20, 24), C(255, 255, 255), C(255, 160, 40) },
    { "FROG", PS_FROG, 400, C(80, 190, 70), C(50, 130, 50), C(210, 240, 140), C(20, 20, 20), C(255, 255, 255), C(40, 90, 40) },
    { "ROBO DOG", PS_DOG, 1200, C(170, 178, 196), C(90, 96, 112), C(220, 226, 240), C(255, 40, 40), C(255, 200, 200), C(60, 64, 76) },
    { "NEON CAT", PS_CAT, 1500, C(255, 60, 200), C(0, 230, 255), C(255, 180, 240), C(120, 255, 120), C(255, 255, 255), C(0, 240, 255) },
    { "BABY DRAGON", PS_DRAGON, 2500, C(80, 180, 90), C(230, 90, 50), C(240, 220, 120), C(255, 220, 40), C(255, 255, 255), C(30, 60, 30) },
    { "PANDA", PS_BEAR, 800, C(245, 245, 245), C(30, 30, 36), C(255, 255, 255), C(20, 20, 24), C(255, 255, 255), C(20, 20, 24) },
    { "BROWN BEAR", PS_BEAR, 700, C(130, 85, 50), C(90, 58, 34), C(200, 160, 120), C(20, 16, 14), C(255, 255, 255), C(30, 20, 16) },
    { "POLAR BEAR", PS_BEAR, 900, C(240, 244, 250), C(200, 210, 222), C(255, 255, 255), C(20, 20, 24), C(255, 255, 255), C(20, 20, 24) },
    { "DUCK", PS_DUCK, 350, C(245, 245, 240), C(205, 205, 200), C(255, 255, 255), C(20, 20, 24), C(255, 255, 255), C(255, 150, 30) },
    { "CHICK", PS_DUCK, 250, C(255, 225, 70), C(235, 190, 40), C(255, 245, 160), C(20, 20, 24), C(255, 255, 255), C(255, 140, 30) },
    { "OWL", PS_OWL, 600, C(150, 105, 65), C(100, 68, 40), C(230, 205, 160), C(20, 16, 10), C(255, 210, 60), C(240, 170, 40) },
    { "SNOW OWL", PS_OWL, 800, C(240, 242, 248), C(180, 186, 200), C(255, 255, 255), C(20, 16, 10), C(255, 220, 60), C(60, 60, 70) },
    { "SLIME", PS_SLIME, 700, C(110, 230, 90), C(60, 160, 60), C(190, 255, 170), C(20, 40, 20), C(230, 255, 230), C(20, 40, 20) },
    { "PINK SLIME", PS_SLIME, 700, C(255, 130, 200), C(200, 70, 150), C(255, 200, 235), C(60, 10, 40), C(255, 235, 250), C(60, 10, 40) },
    { "AXOLOTL", PS_AXOLOTL, 1000, C(255, 170, 200), C(230, 70, 130), C(255, 220, 230), C(30, 10, 20), C(255, 255, 255), C(150, 60, 90) },
    { "PIGLET", PS_HAMSTER, 300, C(255, 170, 185), C(230, 130, 150), C(255, 210, 220), C(30, 16, 20), C(255, 255, 255), C(230, 100, 130) },
    { "TOXIC SLIME", PS_SLIME, 1200, C(60, 255, 30), C(20, 150, 20), C(200, 255, 110), C(10, 40, 0), C(240, 255, 220), C(10, 40, 0) }, /* PET_TOXIC: drips */
    { "BAT", PS_BAT, 600, C(70, 50, 90), C(40, 28, 56), C(110, 90, 130), C(255, 60, 60), C(255, 255, 255), C(255, 255, 255) },
    { "BUTTERFLY", PS_BUTTERFLY, 400, C(70, 170, 255), C(20, 60, 160), C(170, 220, 255), C(30, 24, 40), C(255, 255, 255), C(30, 24, 40) },
    { "MONARCH", PS_BUTTERFLY, 400, C(255, 140, 30), C(40, 24, 20), C(255, 210, 120), C(30, 20, 16), C(255, 255, 255), C(30, 20, 16) },
    { "BEE", PS_BEE, 350, C(255, 200, 40), C(40, 30, 20), C(255, 230, 120), C(20, 16, 10), C(220, 240, 255), C(20, 16, 10) },
    { "MINI DRONE", PS_DRONE, 900, C(150, 156, 172), C(70, 74, 88), C(0, 230, 255), C(255, 50, 50), C(255, 255, 255), C(200, 210, 230) },
    { "GHOST", PS_GHOST, 1000, C(235, 240, 255), C(180, 190, 220), C(255, 255, 255), C(30, 30, 50), C(255, 255, 255), C(60, 60, 90) },
    { "PARROT", PS_PARROT, 700, C(230, 40, 40), C(40, 110, 230), C(255, 220, 60), C(20, 16, 16), C(255, 255, 255), C(255, 200, 60) },
    { "TINY UFO", PS_UFO, 1500, C(160, 170, 190), C(90, 255, 120), C(120, 240, 255), C(20, 20, 24), C(255, 255, 255), C(255, 80, 200) },
    { "TOXIC SLUDGE", PS_SLUDGE, 1300, C(90, 240, 40), C(30, 130, 20), C(210, 255, 120), C(10, 30, 0), C(240, 255, 220), C(10, 30, 0) }, /* drips too */
};
_Static_assert(sizeof PETS / sizeof PETS[0] == PET_COUNT, "PET_COUNT must match the entries in PETS");
#define PET_SLUDGE 40
int petGoo(int k) { return k == PET_TOXIC || k == PET_SLUDGE; } /* drippy toxic pets */
int petFlies(int k) { return PET_SHAPES[PETS[k].shape].fly; }
int petFlap(float t) { static const int seq[4] = { 0, 1, 2, 1 }; return seq[(int)t & 3]; } /* wing beat frame */
const unsigned COLLAR_COL[] = { C(230, 40, 50), C(50, 110, 255), C(60, 210, 80), C(255, 214, 40), C(255, 110, 200),
                                       C(150, 70, 230), C(0, 230, 255), C(30, 30, 36), C(250, 250, 250) };
_Static_assert(sizeof COLLAR_COL / sizeof COLLAR_COL[0] == COLLAR_COUNT, "COLLAR_COUNT must match the entries in COLLAR_COL");
int pet, petsMask = 1, petsMask2, collarOn = 1, collarCol; /* saved (SLOP8; SLOP9 adds petsMask2 for pets 31 and up) */
char petNames[PET_COUNT][PETNAME_MAX + 1];
int petNameEdit, petNameFresh;
const char *petName(int k)
{
    for (const char *p = petNames[k]; *p; p++)
        if (*p != ' ') return petNames[k];
    return PETS[k].name;
}
int ownsPet(int i) { return devMode || (i < 31 ? (petsMask >> i) & 1 : (petsMask2 >> (i - 31)) & 1); }
void givePet(int i) { if (i < 31) petsMask |= 1 << i; else petsMask2 |= 1 << (i - 31); }
/* the pet in the game world */
float petX, petY, petVx, petVy, petWalk;
int petFace = 1, petGround = 1, petPlat = -1;
char toastTxt[64];
float toastT;
