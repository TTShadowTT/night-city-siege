/* base_sprites.c - Pixel sprites drawn as text rows, and the palette letters they use. */
#include "core.h"

#define SPR(name, w, h, ...) static const char *const name##_rows[] = { __VA_ARGS__ }; \
                             const Spr name = { w, h, name##_rows }

#define PLAYER_TOP \
    "....kkkk....", "...khhhhkk..", "..khhhhhhhk.", "..khhssssk..", "..khsvvvvk..", "..kssssssk..", \
    "...kSSSSk...", "..krrrrrrk..", ".kjJJjjjjjk.", ".kjJjjjjjjk.", ".kjJjjjjjjk.", ".kjjjjjjjjk.", \
    "..kjjjjjjk..", "..kppppppk.."
SPR(sPlayerIdle, 12, 20, PLAYER_TOP,
    "..kpPkkpPk..", "..kpPkkpPk..", "..kpPkkpPk..", "..kpPkkpPk..", "..kbbkkbbk..", ".kbbbkkbbbk.");
SPR(sPlayerRun1, 12, 20, PLAYER_TOP,
    "..kpPkkpPk..", ".kpPk..kpPk.", ".kpPk...kpk.", "kpPk....kpk.", "kbbk....kbk.", "kbbbk...kbbk");
SPR(sPlayerRun2, 12, 20, PLAYER_TOP,
    "..kpPPPPk...", "...kpPPk....", "...kpPPk....", "...kpPPk....", "...kbbbk....", "..kbbbbk....");
SPR(sPlayerJump, 12, 20, PLAYER_TOP,
    "..kpPkkpPk..", ".kpPk..kpPk.", ".kbbk..kpPk.", ".kbk...kbbk.", "............", "............");

/* skins with their own body shape: a different top half on the shared leg poses (legs use p / P / b) */
#define L_IDLE "..kpPkkpPk..", "..kpPkkpPk..", "..kpPkkpPk..", "..kpPkkpPk..", "..kbbkkbbk..", ".kbbbkkbbbk."
#define L_RUN1 "..kpPkkpPk..", ".kpPk..kpPk.", ".kpPk...kpk.", "kpPk....kpk.", "kbbk....kbk.", "kbbbk...kbbk"
#define L_RUN2 "..kpPPPPk...", "...kpPPk....", "...kpPPk....", "...kpPPk....", "...kbbbk....", "..kbbbbk...."
#define L_JUMP "..kpPkkpPk..", ".kpPk..kpPk.", ".kbbk..kpPk.", ".kbk...kbbk.", "............", "............"
#define SHAPED_H(name, h, TOP) SPR(name##0, 12, h, TOP, L_IDLE); SPR(name##1, 12, h, TOP, L_RUN1); \
                               SPR(name##2, 12, h, TOP, L_RUN2); SPR(name##3, 12, h, TOP, L_JUMP)
#define SHAPED(name, TOP) SPR(name##0, 12, 20, TOP, L_IDLE); SPR(name##1, 12, 20, TOP, L_RUN1); \
                          SPR(name##2, 12, 20, TOP, L_RUN2); SPR(name##3, 12, 20, TOP, L_JUMP)
/* the same legs under a 14 wide body */
#define W_IDLE "...kpPkkpPk...", "...kpPkkpPk...", "...kpPkkpPk...", "...kpPkkpPk...", "...kbbkkbbk...", "..kbbbkkbbbk.."
#define W_RUN1 "...kpPkkpPk...", "..kpPk..kpPk..", "..kpPk...kpk..", ".kpPk....kpk..", ".kbbk....kbk..", ".kbbbk...kbbk."
#define W_RUN2 "...kpPPPPk....", "....kpPPk.....", "....kpPPk.....", "....kpPPk.....", "....kbbbk.....", "...kbbbbk....."
#define W_JUMP "...kpPkkpPk...", "..kpPk..kpPk..", "..kbbk..kpPk..", "..kbk...kbbk..", "..............", ".............."
#define SHAPED_W(name, h, TOP) SPR(name##0, 14, h, TOP, W_IDLE); SPR(name##1, 14, h, TOP, W_RUN1); \
                               SPR(name##2, 14, h, TOP, W_RUN2); SPR(name##3, 14, h, TOP, W_JUMP)
#define ROBOT_TOP \
    "......k.....", "......r.....", "...kkkkkkk..", "..khhhhhhhk.", "..khvvvvvhk.", "..khhhhhhhk.", "..kkkkkkkkk.", \
    ".kJJJJJJJJk.", "kJjjjrrjjjJk", "kJjjjrrjjjJk", "kkjjjjjjjjkk", ".kjjjjjjjjk.", ".kkkkkkkkkk.", "..kppppppk.."
#define NINJA_TOP \
    "............", "...kkkkk....", "..khhhhhk...", ".khhhhhhhk..", "rkhhvvvvhk..", "rrkhhhhhhk..", ".r.khhhhk...", \
    "..kjjjjjjk..", ".kjjJjjjjjk.", ".kjJrrrrrjk.", ".kjjjjjjjjk.", "..kjjjjjjk..", "..kjjjjjjk..", "..kppppppk.."
#define ASTRO_TOP \
    "...kkkkkk...", "..kwwwwwwk..", ".kwwkkkkkwk.", ".kwkvvvvvkwk", ".kwkvwvvvkwk", ".kwkvvvvvkwk", ".kwwkkkkkwk.", \
    "..kkwwwwkk..", "kkkjjjjjjk..", "krkjJjjjjjk.", "krkjJjrrjjk.", "kkkjjjjjjjk.", "..kjjjjjjk..", "..kppppppk.."
#define NEKO_TOP \
    "..k....k....", ".khk..khk...", ".khhkkhhk...", ".khhhhhhhk..", ".khvhhhvhk..", "kkhhhrhhhkk.", "..khhhhhhk..", \
    "..kjjjjjjk..", ".kjjJjjjjjk.", ".kjJjjjjjjk.", "kkjJjjjjjjk.", "hkjjjjjjjjk.", "hhkjjjjjjk..", ".hkppppppk.."
#define BONES_TOP \
    "...kkkkk....", "..khhhhhk...", ".khhhhhhhk..", ".khkkhkkhk..", ".khvkhvkhk..", ".khhhkhhhk..", "..khkhkhk...", \
    "...kJJJk....", ".kJJkJkJJk..", ".kJkJkJkJk..", ".kJJkJkJJk..", "..kkJJJkk...", "...kJJJk....", "..kpppppk..."
SHAPED(sRobot, ROBOT_TOP);
SHAPED(sNinja, NINJA_TOP);
SHAPED(sAstro, ASTRO_TOP);
SHAPED(sNeko, NEKO_TOP);
SHAPED(sBones, BONES_TOP);
/* edgerunners, each with their own build: David (yellow jacket with white panels, cyan collar, gold chain), Lucy (silver bob
   with cyan tips, red eyeliner, white jacket over a purple bodysuit), Rebecca (small, green buns, big black hoodie with a neon hem,
   pink tattooed legs), Maine (the big one: blond flat-top, red eyes, red scarf, black coat with orange lights), Kiwi (platinum
   bob, pink mask, red suit and coat) */
#define DAVID_TOP \
    "....kkk.....", "..kkhhhkk...", ".khhhhhhhk..", ".kxhhhhhhhhk", ".kxxssshhhk.", "..kxsBssBk..", "...kssssk...", \
    "..kwqBBqwk..", ".kJjjByBjjJk", ".kjwjBBBjwjk", ".kjjwBBBwjjk", ".kwjjBBBjjwk", ".kJjjjjjjjJk", "..kppppppk.."
#define LUCY_TOP \
    "...kkkkkk...", "..khhhhhhkk.", ".khhhhhhhhhk", ".khhhhhhhhhk", ".khhsEssEhk.", ".kzhssssszk.", ".kqzkSSSkqk.", \
    "..kJJjjJJk..", ".kJJjEEjJJk.", ".kJJjjjjJJk.", ".kJkjjjjkJk.", "..kjjjjjjk..", "..kBBBBBBk..", "..kppppppk.."
#define REBECCA_TOP \
    "....kkkk....", ".kkkhhhhkkk.", "khhkhhhhkhhk", "khhhhhhhhhhk", "khkhsvsvhkhk", "hk.ksssssk.h", "k...kSSk...k", \
    ".kJjjjjjjJk.", "kJjjjjjjjjJk", "kjjjjjjjjjjk", "kjJjjjjjJjjk", "kjjjjjjjjjjk", ".krrrrrrrrk.", "..kppppppk.."
#define MAINE_TOP \
    "....kkkkkk....", "...khhhhhhk...", "...khhhhhhk...", "...kllllllk...", "...kRRRRllk...", "...klllLllk...", "...klLKKLlk...", \
    "..krrrrrrrrk..", ".kkjjrrrrrjjk.", "kjojkZZZZkjojk", "kjjjkZWWZkjjjk", "kjjjkZZZZkjjjk", "kjjjkWZZWkjjjk", ".kjjkBBBBkjjk.", \
    ".kjjkppppkjjk.", "..kkkppppkkk.."
#define KIWI_TOP \
    "...kkkkkk...", "..khhhhhhk..", ".khhhhhhhhk.", ".khhhhhhhhk.", ".khhsBssBhk.", ".khhiiiiihk.", "..kkiiiik...", \
    "...kjjjk....", "..kJjjjjJk..", ".kJjjjjjjJk.", ".kjjjJjjjjk.", ".kJjjjjjjJk.", ".kjjjjJjjjk.", ".kjjjjjjjjk.", \
    ".kjkppppkjk."
/* and two just for fun */
#define KNIGHT_TOP \
    "...rrr......", "....rkkkk...", "...khhhhhk..", "..khhhhhhhk.", "..khkkkkkhk.", "..khhhhhhhk.", "...khhhhhk..", \
    "..kJJJJJJk..", ".kJjjrrjjJk.", ".kJjrrrrjJk.", ".kJjjrrjjJk.", ".kJjjjjjjJk.", "..kjjjjjjk..", "..kppppppk.."
#define DINO_TOP \
    ".k.k.k......", "krkrkrk.....", "..khhhhhhk..", ".khhhhhhhhk.", ".khvhhhhhhk.", ".khhhwhwhwk.", "..kssssssk..", \
    "..kjjjjjjk..", ".kjjJJJjjjk.", ".kjjJJJjjjk.", "kkjjjjjjjjk.", "hhkjjjjjjjk.", ".hhkjjjjjk..", "..kppppppk.."
SHAPED(sDavid, DAVID_TOP);
SHAPED(sLucy, LUCY_TOP);
SHAPED(sRebecca, REBECCA_TOP);
SHAPED_W(sMaine, 22, MAINE_TOP);
SHAPED_H(sKiwi, 21, KIWI_TOP);
SHAPED(sKnight, KNIGHT_TOP);
SHAPED(sDino, DINO_TOP);
/* the edgerunners' driver: slicked-back hair, beard, brown leather jacket over a grey shirt */
#define FALCO_TOP \
    "....kkkkk...", "...khhhhhkk.", "..khhhhhhhhk", "..khhssssshk", "..khsBssBsk.", "..kssssssk..", "..ksKKKKsk..", \
    "...kKKKKk...", "..kJJCCJJk..", ".kjJjCCjjJjk", ".kjJjCCjjJjk", ".kjjjCCjjjjk", ".kjjjjjjjjjk", "..kjjjjjjk..", \
    "..kppppppk.."
SHAPED_H(sFalco, 21, FALCO_TOP);
/* and more of Night City: Gloria (David's mum: red hair, red implants under the eye, her yellow medic jacket), Dorio (Maine's
   partner: big, blond, purple jacket), Pilar (Rebecca's brother: goggles, beard, long chrome arms, no shirt) */
#define GLORIA_TOP \
    "..kk.kkkk...", ".khhkhhhhk..", ".khkhhrhhhk.", "..khhhrrhhhk", "..khsvssvrk.", "..khsRsssk..", "...kkSSSk...", \
    "..kwqqqqwk..", ".kJwqBBqwJk.", ".kJjwBBwjJk.", ".kjjjBBjjjk.", ".kjjjjjjjjk.", "..kjjjjjjk..", "..kppppppk.."
#define DORIO_TOP \
    "....kkkkkk....", "...khhhhhhkk..", "..khhhhhhhhhk.", "..khhhsssshk..", "...khsBssBk...", "...ksssSssk...", "....kssssk....", \
    "....kSSSSk....", ".kkJJssssJJkk.", "kJjjksBBskjjJk", "kjjjksBBskjjjk", "kjjjkSssSkjjjk", "kjjjksSSskjjjk", "kssk.BBBB.kssk", \
    ".kk.kppppk.kk.", "....kppppk...."
#define PILAR_TOP \
    "...k..k.k...", "..khkkhkhk..", ".khhhhhhhhk.", ".khhhhhhhhk.", ".khyyyyyyk..", "..ksssssk...", "..kKsKKKk...", \
    "...kKKKk....", ".kfkssssskfk", ".kFksSsSskFk", ".kfkssssskfk", ".kFksSSSskFk", ".kfkssssskfk", ".kFkBBBBBkFk", \
    ".kfkppppkfk."
SHAPED(sGloria, GLORIA_TOP);
SHAPED_W(sDorio, 22, DORIO_TOP);
SHAPED_H(sPilar, 21, PILAR_TOP);
/* a figure in black glass armour: a broken red halo over the head, a red visor, a red core and red veins (they pulse, see skinPal) */
#define SKINX_TOP \
    "....rRRRRr....", "...r......r...", "..r..kkkk..r..", "..r.khhhhk.r..", "...kJhhhhhk...", "...khvvvvvk...", "...khhkkhhk...", \
    "....khrrhk....", "..kkkjhhjkkk..", ".kJJkjrrjkJJk.", ".kjJkrRRrkJjk.", ".kjjkjrrjkjjk.", ".kjjkrjjrkjjk.", ".kRkkjrrjkkRk.", \
    "...kjjrrjjk...", "...kkppppkk..."
SHAPED_W(sSkinX_, 22, SKINX_TOP);
/* two more for the trophy shelf */
#define SKINY_TOP \
    "...k.k.k.k..", "...kykykyk..", "...kyRyqyk..", "...kyyyyyk..", "..khssssshk.", "..khsBssBhk.", "..kwssSsswk.", \
    "...kwwwwwk..", ".kwBwwwwBwk.", "kjJwwwwwwJjk", "kjJjjyyjjJjk", "kjJjjyyjjJjk", "kjjjjyyjjjjk", "kyjjjyyjjjyk", \
    ".kJjppppjJk."
#define SKINZ_TOP \
    "....k.k.k...", "....kykyk...", "...khyiyhk..", "..khhhhhhhk.", "..khsBsBshk.", "..khssssshk.", "..khksEskhk.", \
    "..khkyyykhk.", ".khJjjjjJhk.", ".khjJjjJjhk.", ".kjjjiijjjk.", "..kjjjjjjk..", ".kjjjjjjjjk.", "kjjjjjjjjjjk", \
    "kjjjppppjjjk"
SHAPED_H(sSkinY_, 21, SKINY_TOP);
SHAPED_H(sSkinZ_, 21, SKINZ_TOP);

#define THUG_TOP \
    "...kkkkk....", "..kTTTTTk...", ".kTtttttTk..", ".kttmmmmtk..", ".ktmemmemk..", ".ktmmmmmtk..", \
    "..ktttttk...", ".kTttttttk..", "kTtttttttTkk", "kttttttttmmk", "ktkttttttkk.", "kmkttttttk..", \
    ".k.kttttk...", "...knnnnk..."
SPR(sThug0, 12, 19, THUG_TOP, "..knNkknNk..", "..knNk.knNk.", "..knNk.knNk.", "..kbbk.kbbk.", ".kbbbk.kbbbk");
SPR(sThug1, 12, 19, THUG_TOP, "..knNkknNk..", ".knNk..knNk.", ".knNk...knk.", "kbbk....kbk.", "kbbbk...kbbk");

#define GUNNER_TOP \
    "...kkkkk....", "..kGGGGGk...", ".kGgggggGk..", ".kgggrrrrk..", ".kggkrrrrk..", ".kgggggggk..", \
    "..kkSSSkk...", ".kGGgggGGk..", "kGgggggggGk.", "kgGgggggGgk.", "kgkgggggkgk.", "kSkgggggkSk.", \
    ".k.kgggk.k..", "...kddddk..."
SPR(sGunner0, 12, 19, GUNNER_TOP, "..kdDkkdDk..", "..kdDk.kdDk.", "..kdDk.kdDk.", "..kbbk.kbbk.", ".kbbbk.kbbbk");
SPR(sGunner1, 12, 19, GUNNER_TOP, "..kdDkkdDk..", ".kdDk..kdDk.", ".kdDk...kdk.", "kbbk....kbk.", "kbbbk...kbbk");

SPR(sDrone0, 14, 9,
    "kkkkk....kkkkk",
    "..k........k..",
    "..k.kkkkkk.k..",
    "...kcccccck...",
    "..kcCCCCCCck..",
    "..kcCkRRkCck..",
    "...kcccccck...",
    "....kk..kk....",
    "..............");
SPR(sDrone1, 14, 9,
    ".kkk......kkk.",
    "..k........k..",
    "..k.kkkkkk.k..",
    "...kcccccck...",
    "..kcCCCCCCck..",
    "..kcCkRRkCck..",
    "...kcccccck...",
    "....kk..kk....",
    "..............");

#define BOMBER_TOP "....kk....", "....ko....", "...kkkk...", "..kxxxxk..", ".kxXXXxxk.", ".kxXRRxxk.", \
                   ".kxxRRxxk.", ".kxxxxxxk.", "..kxxxxk.."
SPR(sBomber0, 10, 12, BOMBER_TOP, ".kk.kk.kk.", "kxk....kxk", "kk......kk");
SPR(sBomber1, 10, 12, BOMBER_TOP, "..kkkkkk..", "..kxk.kxk.", "..kk..kk..");

#define BRUTE_TOP \
    "......kkkkkk......", ".....kUUUUUUk.....", "....kUuuuuuuUk....", "....kuyyyyyyuk....", \
    "....kuuuuuuuuk....", "...kkkkuuuukkkk...", "..kUUUkkkkkkUUUk..", ".kUuuuuuuuuuuuuUk.", \
    "kUuuuUUUUUUUUuuuUk", "kuuuuuuuuuuuuuuuuk", "kuuukuuuuuuuukuuuk", "kuuukuuUUUUuukuuuk", \
    "kuuukuuuuuuuukuuuk", "kUUUkuuuuuuuukUUUk", "kkkkkuuuuuuuukkkkk", "....kddddddddk...."
SPR(sBrute0, 18, 24, BRUTE_TOP,
    "....kdDDkkdDDk....", "...kddDk..kdDdk...", "...kdDk....kdDk...", "...kdDk....kdDk...",
    "...kdDk....kdDk...", "..kbbbk....kbbbk..", ".kbbbbk....kbbbbk.", ".kkkkkk....kkkkkk.");
SPR(sBrute1, 18, 24, BRUTE_TOP,
    "....kdDDkkdDDk....", "....kdDk..kdDk....", "...kdDk....kdDk...", "..kdDk......kdDk..",
    "..kdDk......kdDk..", ".kbbbk......kbbbk.", "kbbbbk......kbbbbk", "kkkkkk......kkkkkk");

#define SHIELD_TOP \
    "...kkkkk....", "..kNNNNNk...", ".kNnnnnnNk..", ".knnqqqqqk..", ".knnqwqqqk..", ".knnnnnnnk..", \
    "..kkSSSkk...", ".kNNnnnNNk..", "kNnnnnnnnNk.", "knNnnnnnNnk.", "knknnnnnknk.", "kSknnnnnkSk.", \
    ".k.knnnk.k..", "...kddddk..."
SPR(sShield0, 12, 19, SHIELD_TOP, "..kdDkkdDk..", "..kdDk.kdDk.", "..kdDk.kdDk.", "..kbbk.kbbk.", ".kbbbk.kbbbk");
SPR(sShield1, 12, 19, SHIELD_TOP, "..kdDkkdDk..", ".kdDk..kdDk.", ".kdDk...kdk.", "kbbk....kbk.", "kbbbk...kbbk");

SPR(sGunship, 56, 17,
    "...........................kk...........................",
    "...........................kk...........................",
    "..................kkkkkkkkkkkkkkkkkkkk..................",
    "...............kkkaaaaaaaaaaaaaaaaaaaakkk...............",
    "............kkkaaaaaaaaaaaaaaaaaaaaaaaaaakkk............",
    "kk........kkaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaqqqqqqkk......",
    "kakk....kkaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaqqqqqqqqk.....",
    "kaaakkkkaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaqqqqqqqqqk....",
    "kaaaaaaaaAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAk....",
    "kaaakkkkaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaak....",
    "kkkk....kaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaak.....",
    "..........kaaaayyaaaaaaaaaaaaaaaaaaaaaaaaayyaaaaak......",
    "...........kkaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaakk.......",
    "............kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk.........",
    "...............kcck..............kcck...................",
    "..............kcccck............kcccck..................",
    "...........kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk..............");

/* the enemies that join after each big boss: a teleporting cyber ninja, a jetpack trooper, a missile mech */
#define CYNINJA_TOP \
    "...kkkkk....", "..kddddDk...", ".kdddddddk..", ".kdqqqqqdk..", ".kddddddDk..", ".kRRkkkkk...", \
    "kRRkDDDDk...", ".kkDdddddk..", "..kdddddDk..", ".kDdkdddkDk.", ".kdkdddddkk.", ".kSkdddk.k..", \
    "..k.kddk....", "...kddddk..."
SPR(sCyNinja0, 12, 19, CYNINJA_TOP, "..kdDkkdDk..", "..kdDk.kdDk.", "..kdDk.kdDk.", "..kbbk.kbbk.", ".kbbbk.kbbbk");
SPR(sCyNinja1, 12, 19, CYNINJA_TOP, "..kdDkkdDk..", ".kdDk..kdDk.", ".kdDk...kdk.", "kbbk....kbk.", "kbbbk...kbbk");
#define JET_TOP \
    "...kkkkk....", "..kCCCCCk...", ".kCcccccCk..", ".kccvvvvvk..", ".kccccccck..", "kxkkSSSkk...", \
    "kXkCcccCCk..", "kxkcccccCck.", "kXkcccccckck", "kxkcccccckSk", ".kkkcccck.k.", "...kcccck...", "...kddddk..."
SPR(sJet0, 12, 17, JET_TOP, "...kdk.kdk..", ".o.kbk.kbk..", ".oo.........", "..o.........");
SPR(sJet1, 12, 17, JET_TOP, "...kdk.kdk..", ".o.kbk.kbk..", "..o.........", ".oo.........");
#define MECH_TOP \
    "......kkkkkkkk......", ".....kxxxxxxxxk.....", "....kxXXXXXXXXxk....", "...kxXRRxxxxRRXxk...", \
    "...kxxxxxxxxxxxxk...", "kkkkxxXXXXXXXXxxkkkk", "kccckxxxxxxxxxxkccck", "kcRckxxxxxxxxxxkcRck", \
    "kccckxXxxxxxXxxkccck", "kkkkkxxxxxxxxxxkkkkk", "....kkxxxxxxxxkk....", ".....kkkkkkkkkk....."
SPR(sMech0, 20, 18, MECH_TOP, "...kk..........kk...", "..kxxk........kxxk..", ".kxxk..........kxxk.",
    "kxxk............kxxk", "kxk..............kxk", "kk................kk");
SPR(sMech1, 20, 18, MECH_TOP, "....kk........kk....", "...kxxk......kxxk...", "..kxxk........kxxk..",
    ".kxxk..........kxxk.", ".kxk............kxk.", ".kk..............kk.");

/* the Night City factions' own enemies (see EnemyBody): thug, gunner, brute and drone, two frames each.
   h v r j J p P b are the faction's colours (helmet or hair, eyes, accent, armour, armour light, pants, pants light, boots) */
/* ARASAKA */
SPR(sEAraThug0, 12, 20,
    "...kkkkk....", "..khhhhhk...", ".khJhhhhhk..", ".khhhhhhhk..", ".khhkvvvvk..", ".khhhhhhhk..",
    "..kkhhhkk...", ".kJjjjjjJk..", "kJjjrrrjjJk.", "kjJjjrjjJjkk", "kjkjjrjjkffk", "kJkjjjjjk.k.",
    "kfkBBBBBk...", ".k.kjjjk....", "...kppppk...", "..kpPkkpPk..", "..kpPk.kpPk.", "..kpPk.kpPk.",
    "..kbbk.kbbk.", ".kbbbk.kbbbk");
SPR(sEAraThug1, 12, 20,
    "...kkkkk....", "..khhhhhk...", ".khJhhhhhk..", ".khhhhhhhk..", ".khhkvvvvk..", ".khhhhhhhk..",
    "..kkhhhkk...", ".kJjjjjjJk..", "kJjjrrrjjJk.", "kjJjjrjjJjkk", "kjkjjrjjkffk", "kJkjjjjjk.k.",
    "kfkBBBBBk...", ".k.kjjjk....", "...kppppk...", "..kpPkkpPk..", ".kpPk..kpPk.", ".kpPk...kpk.",
    "kbbk....kbk.", "kbbbk...kbbk");
SPR(sEAraGun0, 12, 20,
    "...kkkkk....", "..khhhhhkk..", ".khhhhhhhk..", ".khhhsssk...", ".khhBBBBk...", ".khsssssk...",
    "..kssSSk....", "..kkwwwkk...", ".kjjwrwjjk..", "kjjjwrwjjjk.", "kjJjjrjjJjk.", "kjkjjjjjkjk.",
    "kskjjjjjksk.", ".k.kjjjk.k..", "...kppppk...", "..kpPkkpPk..", "..kpPk.kpPk.", "..kpPk.kpPk.",
    "..kbbk.kbbk.", ".kbbbk.kbbbk");
SPR(sEAraGun1, 12, 20,
    "...kkkkk....", "..khhhhhkk..", ".khhhhhhhk..", ".khhhsssk...", ".khhBBBBk...", ".khsssssk...",
    "..kssSSk....", "..kkwwwkk...", ".kjjwrwjjk..", "kjjjwrwjjjk.", "kjJjjrjjJjk.", "kjkjjjjjkjk.",
    "kskjjjjjksk.", ".k.kjjjk.k..", "...kppppk...", "..kpPkkpPk..", ".kpPk..kpPk.", ".kpPk...kpk.",
    "kbbk....kbk.", "kbbbk...kbbk");
SPR(sEAraBrute0, 20, 25,
    ".......kkkkkk.......", "......khhhhhhk......", ".....khJhhhhhhk.....", ".....khkvvvvvhk.....",
    ".....khhhhhhhhk.....", "..kkkkkkhhhhkkkkkk..", ".kJJJJJkkkkkkJJJJJk.", "kJjjjjjJkjjkJjjjjjJk",
    "kjrrrrjjkjjkjjrrrrjk", "kjjjjjjkjjjjkjjjjjjk", "kkkkkkkjjrrjjkkkkkkk", "kjjjjkjjjrrjjjkjjjjk",
    "kJjjJkjjjjjjjjkJjjJk", "kffffkjjjjjjjjkffffk", "kkkkkkBBBBBBBBkkkkkk", ".....kBkkkkkkBk.....",
    ".....kppppppppk.....", ".....kpPPkkpPPk.....", "....kppPk..kpPpk....", "....kpPk....kpPk....",
    "....kpPk....kpPk....", "....kpPk....kpPk....", "...kbbbk....kbbbk...", "..kbbbbk....kbbbbk..",
    "..kkkkkk....kkkkkk..");
SPR(sEAraBrute1, 20, 25,
    ".......kkkkkk.......", "......khhhhhhk......", ".....khJhhhhhhk.....", ".....khkvvvvvhk.....",
    ".....khhhhhhhhk.....", "..kkkkkkhhhhkkkkkk..", ".kJJJJJkkkkkkJJJJJk.", "kJjjjjjJkjjkJjjjjjJk",
    "kjrrrrjjkjjkjjrrrrjk", "kjjjjjjkjjjjkjjjjjjk", "kkkkkkkjjrrjjkkkkkkk", "kjjjjkjjjrrjjjkjjjjk",
    "kJjjJkjjjjjjjjkJjjJk", "kffffkjjjjjjjjkffffk", "kkkkkkBBBBBBBBkkkkkk", ".....kBkkkkkkBk.....",
    ".....kppppppppk.....", ".....kpPPkkpPPk.....", ".....kpPk..kpPk.....", "....kpPk....kpPk....",
    "...kpPk......kpPk...", "...kpPk......kpPk...", "..kbbbk......kbbbk..", ".kbbbbk......kbbbbk.",
    ".kkkkkk......kkkkkk.");
SPR(sEAraDrone0, 14, 9,
    "kkkkk....kkkkk", "..k........k..", "..k.kkkkkk.k..", "...kwwwwwwk...", "..kwwwwwwwwk..", "..kwwkvvkwwk..",
    "...kfwwwwfk...", "....kkrrkk....", "..............");
SPR(sEAraDrone1, 14, 9,
    ".kkk......kkk.", "..k........k..", "..k.kkkkkk.k..", "...kwwwwwwk...", "..kwwwwwwwwk..", "..kwwkvvkwwk..",
    "...kfwwwwfk...", "....kkrrkk....", "..............");
/* MAXTAC */
SPR(sEMaxThug0, 12, 20,
    "...kkkkkk...", "..khhhhhhk..", ".khJhhhhhhk.", ".khkvvvvvvk.", ".khhhhhhhhk.", "..khhhhhhk..",
    ".kkkkkkkkkk.", "krJjjjjjjJrk", "kjjjrrrjjjjk", "kjJjjjjjjJjk", "kjkjjjjjjkfk", "kfkjjjjjjkfk",
    ".kkBBrBBBkk.", "..kjjjjjjk..", "..kppppppk..", "..kpPkkpPk..", "..kpPk.kpPk.", "..kpPk.kpPk.",
    "..kbbk.kbbk.", ".kbbbk.kbbbk");
SPR(sEMaxThug1, 12, 20,
    "...kkkkkk...", "..khhhhhhk..", ".khJhhhhhhk.", ".khkvvvvvvk.", ".khhhhhhhhk.", "..khhhhhhk..",
    ".kkkkkkkkkk.", "krJjjjjjjJrk", "kjjjrrrjjjjk", "kjJjjjjjjJjk", "kjkjjjjjjkfk", "kfkjjjjjjkfk",
    ".kkBBrBBBkk.", "..kjjjjjjk..", "..kppppppk..", "..kpPkkpPk..", ".kpPk..kpPk.", ".kpPk...kpk.",
    "kbbk....kbk.", "kbbbk...kbbk");
SPR(sEMaxGun0, 12, 21,
    "...kkkkk....", "..khhhhhk...", ".khJhhhhhk..", ".khhkvvvvk..", ".khhhhhhhk..", ".khhkkkhhk..",
    "..kkBBBkk...", "...kBBBk....", ".kkJjjjJkk..", "kJJjjrjjJJk.", "kjJjjrjjJjk.", "kjkjjjjjkjk.",
    "kjkjjjjjkjk.", "kfkjjjjjkfk.", ".kkjjjjjjk..", "..kjjkjjjk..", "..kpPkkpPk..", "..kpPk.kpPk.",
    "..kpPk.kpPk.", "..kbbk.kbbk.", ".kbbbk.kbbbk");
SPR(sEMaxGun1, 12, 21,
    "...kkkkk....", "..khhhhhk...", ".khJhhhhhk..", ".khhkvvvvk..", ".khhhhhhhk..", ".khhkkkhhk..",
    "..kkBBBkk...", "...kBBBk....", ".kkJjjjJkk..", "kJJjjrjjJJk.", "kjJjjrjjJjk.", "kjkjjjjjkjk.",
    "kjkjjjjjkjk.", "kfkjjjjjkfk.", ".kkjjjjjjk..", "..kjjkjjjk..", "..kpPkkpPk..", ".kpPk..kpPk.",
    ".kpPk...kpk.", "kbbk....kbk.", "kbbbk...kbbk");
SPR(sEMaxBrute0, 20, 26,
    "......kkkkkkkk......", ".....khhhhhhhhk.....", "....khJhhhhhhhhk....", "....khkvvvkvvvhk....",
    "....khhhhhhhhhhk....", "....khkBkBkBkhhk....", "..kkkkkhhhhhhkkkkk..", ".krrJJJkkkkkkJJJrrk.",
    "kJjjjjjJkrrkJjjjjjJk", "kjjjjjjjkrrkjjjjjjjk", "kjjjkjjjjjjjjjjkjjjk", "kjjjkjjjjjjjjjjkjjjk",
    "kJjjkjjJJJJJJjjkjjJk", "kjjjkjjjjjjjjjjkjjjk", "kffkkjjjjjjjjjjkkffk", "kkkk.kBBBBBBBBk.kkkk",
    "....kppppppppppk....", "....kppppppppppk....", ".....kpPPkkpPPk.....", "....kppPk..kpPpk....",
    "....kpPk....kpPk....", "....kpPk....kpPk....", "....kpPk....kpPk....", "...kbbbk....kbbbk...",
    "..kbbbbk....kbbbbk..", "..kkkkkk....kkkkkk..");
SPR(sEMaxBrute1, 20, 26,
    "......kkkkkkkk......", ".....khhhhhhhhk.....", "....khJhhhhhhhhk....", "....khkvvvkvvvhk....",
    "....khhhhhhhhhhk....", "....khkBkBkBkhhk....", "..kkkkkhhhhhhkkkkk..", ".krrJJJkkkkkkJJJrrk.",
    "kJjjjjjJkrrkJjjjjjJk", "kjjjjjjjkrrkjjjjjjjk", "kjjjkjjjjjjjjjjkjjjk", "kjjjkjjjjjjjjjjkjjjk",
    "kJjjkjjJJJJJJjjkjjJk", "kjjjkjjjjjjjjjjkjjjk", "kffkkjjjjjjjjjjkkffk", "kkkk.kBBBBBBBBk.kkkk",
    "....kppppppppppk....", "....kppppppppppk....", ".....kpPPkkpPPk.....", ".....kpPk..kpPk.....",
    "....kpPk....kpPk....", "...kpPk......kpPk...", "...kpPk......kpPk...", "..kbbbk......kbbbk..",
    ".kbbbbk......kbbbbk.", ".kkkkkk......kkkkkk.");
SPR(sEMaxDrone0, 16, 9,
    "kkkkkk....kkkkkk", "...k........k...", "...k.kkkkkk.k...", "..kkjjjjjjjjkk..", ".kjjJJJJJJJJjjk.",
    ".kjjkvvvvvvkjjk.", "..kkjjjjjjjjkk..", "....kk.rr.kk....", "................");
SPR(sEMaxDrone1, 16, 9,
    ".kkkk......kkkk.", "...k........k...", "...k.kkkkkk.k...", "..kkjjjjjjjjkk..", ".kjjJJJJJJJJjjk.",
    ".kjjkvvvvvvkjjk.", "..kkjjjjjjjjkk..", "....kk.rr.kk....", "................");
/* TYGER CLAWS */
SPR(sETygThug0, 12, 19,
    "..kkkkk.....", ".khhhhhkk...", "khhrrhhhhhk.", ".kkhhhhhhk..", "..khsssssk..", "..khsBvBvk..",
    "..khsssssk..", "...kssSsk...", ".kkrjjjjrkk.", "kssjrjjrjssk", "kszjjrrjjzsk", "ksskjjjjkssk",
    ".kk.kBBk.kk.", "...kppppk...", "..kpPkkpPk..", "..kpPk.kpPk.", "..kpPk.kpPk.", "..kbbk.kbbk.",
    ".kbbbk.kbbbk");
SPR(sETygThug1, 12, 19,
    "..kkkkk.....", ".khhhhhkk...", "khhrrhhhhhk.", ".kkhhhhhhk..", "..khsssssk..", "..khsBvBvk..",
    "..khsssssk..", "...kssSsk...", ".kkrjjjjrkk.", "kssjrjjrjssk", "kszjjrrjjzsk", "ksskjjjjkssk",
    ".kk.kBBk.kk.", "...kppppk...", "..kpPkkpPk..", ".kpPk..kpPk.", ".kpPk...kpk.", "kbbk....kbk.",
    "kbbbk...kbbk");
SPR(sETygGun0, 12, 19,
    "...kkkkk....", "..khhhhhk...", ".khhhhhhhk..", "khhhssssk...", "kkhsvvvvk...", ".khsssssk...",
    "..kkssskk...", ".krjjjjjrk..", "krjBjjBjjrk.", "kjjjBjjBjjk.", "krkjjBjjkrk.", "kskjjjjjksk.",
    ".k.kjjjjk...", "...kppppk...", "..kpPkkpPk..", "..kpPk.kpPk.", "..kpPk.kpPk.", "..kbbk.kbbk.",
    ".kbbbk.kbbbk");
SPR(sETygGun1, 12, 19,
    "...kkkkk....", "..khhhhhk...", ".khhhhhhhk..", "khhhssssk...", "kkhsvvvvk...", ".khsssssk...",
    "..kkssskk...", ".krjjjjjrk..", "krjBjjBjjrk.", "kjjjBjjBjjk.", "krkjjBjjkrk.", "kskjjjjjksk.",
    ".k.kjjjjk...", "...kppppk...", "..kpPkkpPk..", ".kpPk..kpPk.", ".kpPk...kpk.", "kbbk....kbk.",
    "kbbbk...kbbk");
SPR(sETygBrute0, 20, 22,
    ".........kk.........", "........khhk........", "......kkkhhkkk......", ".....khhhhhhhhk.....",
    ".....kssssssssk.....", ".....kskvsskvsk.....", ".....ksssSSsssk.....", "...kkkksssssskkkk...",
    ".kssssssssssssssssk.", "kszskssssssssssksrsk", "kzszksssssssssskrsrk", "kszskssssSSssssksrsk",
    "kzszksssssssssskrsrk", "kSSSksssssssssskSSSk", "kkkkkwwwwwwwwwwkkkkk", "....kwwwwrrwwwwk....",
    ".....ksSSkksSSk.....", "....ksSSk..ksSSk....", "....ksSk....ksSk....", "...kBBBk....kBBBk...",
    "..kBBBBk....kBBBBk..", "..kkkkkk....kkkkkk..");
SPR(sETygBrute1, 20, 22,
    ".........kk.........", "........khhk........", "......kkkhhkkk......", ".....khhhhhhhhk.....",
    ".....kssssssssk.....", ".....kskvsskvsk.....", ".....ksssSSsssk.....", "...kkkksssssskkkk...",
    ".kssssssssssssssssk.", "kszskssssssssssksrsk", "kzszksssssssssskrsrk", "kszskssssSSssssksrsk",
    "kzszksssssssssskrsrk", "kSSSksssssssssskSSSk", "kkkkkwwwwwwwwwwkkkkk", "....kwwwwrrwwwwk....",
    ".....ksSSkksSSk.....", "....ksSk....ksSk....", "...ksSk......ksSk...", "..kBBBk......kBBBk..",
    ".kBBBBk......kBBBBk.", ".kkkkkk......kkkkkk.");
SPR(sETygDrone0, 14, 8,
    "kkkkk....kkkkk", "..k........k..", "..k.kkkkkk.k..", "..kkrrrrrrkk..", ".kjjjjjjjjjjk.", ".kjjkvvvvkjjk.",
    "..kkrrrrrrkk..", "....kk..kk....");
SPR(sETygDrone1, 14, 8,
    ".kkk......kkk.", "..k........k..", "..k.kkkkkk.k..", "..kkrrrrrrkk..", ".kjjjjjjjjjjk.", ".kjjkvvvvkjjk.",
    "..kkrrrrrrkk..", "....kk..kk....");
/* CYBERPSYCHOS */
SPR(sEPsyThug0, 14, 17,
    "....kkkk......", "...khhhhk.....", "..khhhhhhk....", "..khffvfvk....", "..kfffFffk....", "...kkfkkk.....",
    ".kffkjjjjkffk.", "kfFkjjrrjjkFfk", "kfkjjjjjjjjkfk", "kwkjjjjjjjjkwk", "kw.kBBBBBBk.wk", "w...kppppk...w",
    "...kpPkkpPk...", "...kpPk.kpPk..", "...kpPk.kpPk..", "...kbbk.kbbk..", "..kbbbk.kbbbk.");
SPR(sEPsyThug1, 14, 17,
    "....kkkk......", "...khhhhk.....", "..khhhhhhk....", "..khffvfvk....", "..kfffFffk....", "...kkfkkk.....",
    ".kffkjjjjkffk.", "kfFkjjrrjjkFfk", "kfkjjjjjjjjkfk", "kwkjjjjjjjjkwk", "kw.kBBBBBBk.wk", "w...kppppk...w",
    "...kpPkkpPk...", "..kpPk..kpPk..", "..kpPk...kpk..", ".kbbk....kbk..", ".kbbbk...kbbk.");
SPR(sEPsyGun0, 12, 20,
    "...kkkkk....", "..khhhhhk...", ".khhhhhhhk..", ".kfffsssk...", ".kfvfsvsk...", ".kfffsssk...",
    "..kfFsSk....", "kkkkkssk....", "kfffkjjjkk..", "kFFFkjrjjjk.", "kfffkjjjjjk.", "kFFFkjjjkjk.",
    "kfffkjjjksk.", "kwwk.kjjk.k.", ".kk.kppppk..", "..kpPkkpPk..", "..kpPk.kpPk.", "..kpPk.kpPk.",
    "..kbbk.kbbk.", ".kbbbk.kbbbk");
SPR(sEPsyGun1, 12, 20,
    "...kkkkk....", "..khhhhhk...", ".khhhhhhhk..", ".kfffsssk...", ".kfvfsvsk...", ".kfffsssk...",
    "..kfFsSk....", "kkkkkssk....", "kfffkjjjkk..", "kFFFkjrjjjk.", "kfffkjjjjjk.", "kFFFkjjjkjk.",
    "kfffkjjjksk.", "kwwk.kjjk.k.", ".kk.kppppk..", "..kpPkkpPk..", ".kpPk..kpPk.", ".kpPk...kpk.",
    "kbbk....kbk.", "kbbbk...kbbk");
SPR(sEPsyBrute0, 20, 26,
    ".......kkkkkk.......", "......kffffffk......", ".....kfFffffFfk.....", ".....kfvvkkvvfk.....",
    ".....kfffFFfffk.....", ".....kkBwBwBwkk.....", ".kk..kkkkkkkkkk..kk.", "kfFkkkjjjjjjjjkkkFfk",
    "kfffFkjjrrrrjjkFfffk", "kFfffkjjjjjjjjkfffFk", "kfffFkjrjjjjrjkFfffk", "kFffkkjjjjjjjjkkffFk",
    "kfffk.kjjrrjjk.kfffk", "kwwwk.kjjjjjjk.kwwwk", "kwkwk.kBBBBBBk.kwkwk", "......kppppppk......",
    ".....kppppppppk.....", ".....kpPPPPPPpk.....", ".....kpPPkkpPPk.....", "....kppPk..kpPpk....",
    "....kpPk....kpPk....", "....kpPk....kpPk....", "....kpPk....kpPk....", "...kbbbk....kbbbk...",
    "..kbbbbk....kbbbbk..", "..kkkkkk....kkkkkk..");
SPR(sEPsyBrute1, 20, 26,
    ".......kkkkkk.......", "......kffffffk......", ".....kfFffffFfk.....", ".....kfvvkkvvfk.....",
    ".....kfffFFfffk.....", ".....kkBwBwBwkk.....", ".kk..kkkkkkkkkk..kk.", "kfFkkkjjjjjjjjkkkFfk",
    "kfffFkjjrrrrjjkFfffk", "kFfffkjjjjjjjjkfffFk", "kfffFkjrjjjjrjkFfffk", "kFffkkjjjjjjjjkkffFk",
    "kfffk.kjjrrjjk.kfffk", "kwwwk.kjjjjjjk.kwwwk", "kwkwk.kBBBBBBk.kwkwk", "......kppppppk......",
    ".....kppppppppk.....", ".....kpPPPPPPpk.....", ".....kpPPkkpPPk.....", ".....kpPk..kpPk.....",
    "....kpPk....kpPk....", "...kpPk......kpPk...", "...kpPk......kpPk...", "..kbbbk......kbbbk..",
    ".kbbbbk......kbbbbk.", ".kkkkkk......kkkkkk.");
SPR(sEPsyDrone0, 14, 9,
    "kkkk......kk..", "..k.......k...", "..k.kkkkkk.k..", "...kjjjjjjk...", "..kjfFjjjjjk..", "..kjjkvvkjjk..",
    "...kjjjjjFk...", "....kk..k.....", "......w...w...");
SPR(sEPsyDrone1, 14, 9,
    ".kk.......kkk.", "..k.......k...", "..k.kkkkkk.k..", "...kjjjjjjk...", "..kjfFjjjjjk..", "..kjjkvvkjjk..",
    "...kjjjjjFk...", "....kk..k.....", "......w...w...");
/* SCAVS */
SPR(sEScvThug0, 12, 18,
    "...kkkkk....", "..khhhhhk...", ".khhhhhhhk..", ".khkvkkvkk..", ".khkkkkkkk..", ".khkFFFFk...",
    "..kkFkFkk...", ".kjjjjjjjk..", "kjJjjrjjjJk.", "kjjjjjjjjjkk", "kjjjjjjjjjWk", ".kjjjjjjjk.k",
    "..kpppppk...", "..kpPkkpPk..", "..kpPk.kpPk.", "..kpPk.kpPk.", "..kbbk.kbbk.", ".kbbbk.kbbbk");
SPR(sEScvThug1, 12, 18,
    "...kkkkk....", "..khhhhhk...", ".khhhhhhhk..", ".khkvkkvkk..", ".khkkkkkkk..", ".khkFFFFk...",
    "..kkFkFkk...", ".kjjjjjjjk..", "kjJjjrjjjJk.", "kjjjjjjjjjkk", "kjjjjjjjjjWk", ".kjjjjjjjk.k",
    "..kpppppk...", "..kpPkkpPk..", ".kpPk..kpPk.", ".kpPk...kpk.", "kbbk....kbk.", "kbbbk...kbbk");
SPR(sEScvGun0, 12, 19,
    "...kkkkk....", "..khhhhhk...", ".khhhhhhhk..", ".khhkssssk..", ".khkBvBvk...", ".khhssssk...",
    "..khkSSk....", ".kjWjjjWjk..", "kjjjWjWjjjk.", "kjJjjWjjJjk.", "kjkjWjWjkjk.", "kskWjjjWksk.",
    ".k.kjjjjk...", "...kppppk...", "..kpPkkpPk..", "..kpPk.kpPk.", "..kpPk.kpPk.", "..kbbk.kbbk.",
    ".kbbbk.kbbbk");
SPR(sEScvGun1, 12, 19,
    "...kkkkk....", "..khhhhhk...", ".khhhhhhhk..", ".khhkssssk..", ".khkBvBvk...", ".khhssssk...",
    "..khkSSk....", ".kjWjjjWjk..", "kjjjWjWjjjk.", "kjJjjWjjJjk.", "kjkjWjWjkjk.", "kskWjjjWksk.",
    ".k.kjjjjk...", "...kppppk...", "..kpPkkpPk..", ".kpPk..kpPk.", ".kpPk...kpk.", "kbbk....kbk.",
    "kbbbk...kbbk");
SPR(sEScvBrute0, 20, 24,
    ".......kkkkkk.......", "......kssssssk......", ".....kssssssssk.....", ".....ksBkssBksk.....",
    ".....kssssSsssk.....", ".....kKKKKKKKKk.....", "..kkkkkKKKKKKkkkkk..", ".ksssskwwwwwwkssssk.",
    "ksssssKwwwwwwKsssssk", "kssskwwwwEwwwwwksssk", "kssskwwwwwwwEwwksssk", "kssskwwEwwwwwwwksssk",
    "kSSSkwwwwwwwwwwkSSSk", "kkkkkwwwwwwwwwwkfffk", "....kwwwwwwwwwwkfffk", "....kppppppppppkkkkk",
    ".....kpPPkkpPPk.....", "....kppPk..kpPpk....", "....kpPk....kpPk....", "....kpPk....kpPk....",
    "....kpPk....kpPk....", "...kbbbk....kbbbk...", "..kbbbbk....kbbbbk..", "..kkkkkk....kkkkkk..");
SPR(sEScvBrute1, 20, 24,
    ".......kkkkkk.......", "......kssssssk......", ".....kssssssssk.....", ".....ksBkssBksk.....",
    ".....kssssSsssk.....", ".....kKKKKKKKKk.....", "..kkkkkKKKKKKkkkkk..", ".ksssskwwwwwwkssssk.",
    "ksssssKwwwwwwKsssssk", "kssskwwwwEwwwwwksssk", "kssskwwwwwwwEwwksssk", "kssskwwEwwwwwwwksssk",
    "kSSSkwwwwwwwwwwkSSSk", "kkkkkwwwwwwwwwwkfffk", "....kwwwwwwwwwwkfffk", "....kppppppppppkkkkk",
    ".....kpPPkkpPPk.....", ".....kpPk..kpPk.....", "....kpPk....kpPk....", "...kpPk......kpPk...",
    "...kpPk......kpPk...", "..kbbbk......kbbbk..", ".kbbbbk......kbbbbk.", ".kkkkkk......kkkkkk.");
SPR(sEScvDrone0, 14, 9,
    "kkkkk....kkkkk", "..k........k..", "..k.kkkkkk.k..", "...kjjrjjjk...", "..kjJjjjjrjk..", "..kjjkvvkjjk..",
    "...kjjjjjjk...", "....kk..kk....", "..............");
SPR(sEScvDrone1, 14, 9,
    ".kkk......kkk.", "..k........k..", "..k.kkkkkk.k..", "...kjjrjjjk...", "..kjJjjjjrjk..", "..kjjkvvkjjk..",
    "...kjjjjjjk...", "....kk..kk....", "..............");
/* ADAM SMASHER */
SPR(sESmaThug0, 12, 19,
    "..kkkkkkk...", ".khhhhhhhk..", ".khJJJJJhk..", ".kvvvvvvvk..", ".khhhhhhhk..", ".kfkfkfkfk..",
    "..kkhhhkk...", ".kJJjjjJJk..", "kJjjjrjjjJk.", "kjJjjjjjJjk.", "kjkjjjjjkFk.", "kFkjjjjjkFk.",
    ".k.kjjjk.k..", "...kppppk...", "..kpPkkpPk..", "..kpPk.kpPk.", "..kpPk.kpPk.", "..kbbk.kbbk.",
    ".kbbbk.kbbbk");
SPR(sESmaThug1, 12, 19,
    "..kkkkkkk...", ".khhhhhhhk..", ".khJJJJJhk..", ".kvvvvvvvk..", ".khhhhhhhk..", ".kfkfkfkfk..",
    "..kkhhhkk...", ".kJJjjjJJk..", "kJjjjrjjjJk.", "kjJjjjjjJjk.", "kjkjjjjjkFk.", "kFkjjjjjkFk.",
    ".k.kjjjk.k..", "...kppppk...", "..kpPkkpPk..", ".kpPk..kpPk.", ".kpPk...kpk.", "kbbk....kbk.",
    "kbbbk...kbbk");
SPR(sESmaGun0, 12, 20,
    "...kkkkk....", "..khhhhhk...", ".khJhhhhhk..", ".khhkvvvvk..", ".khhhhhhhk..", ".khkfkfkfk..",
    "kkkkhhhkk...", "kFFFkhkk....", "kfFkJjjjJk..", "kkkJjjrjjJk.", "kjJjjjjjJjk.", "kjkjjjjjkjk.",
    "kFkjjjjjkFk.", ".k.kjjjk.k..", "...kppppk...", "..kpPkkpPk..", "..kpPk.kpPk.", "..kpPk.kpPk.",
    "..kbbk.kbbk.", ".kbbbk.kbbbk");
SPR(sESmaGun1, 12, 20,
    "...kkkkk....", "..khhhhhk...", ".khJhhhhhk..", ".khhkvvvvk..", ".khhhhhhhk..", ".khkfkfkfk..",
    "kkkkhhhkk...", "kFFFkhkk....", "kfFkJjjjJk..", "kkkJjjrjjJk.", "kjJjjjjjJjk.", "kjkjjjjjkjk.",
    "kFkjjjjjkFk.", ".k.kjjjk.k..", "...kppppk...", "..kpPkkpPk..", ".kpPk..kpPk.", ".kpPk...kpk.",
    "kbbk....kbk.", "kbbbk...kbbk");
SPR(sESmaBrute0, 20, 26,
    "......kkkkkkkk......", ".....khhhhhhhhk.....", "....khJJJJJJJJhk....", "....khkkkkkkkkhk....",
    "....khvvvvvvvvhk....", "....khkkkkkkkkhk....", "....kffkfkfkfffk....", "kkkkkkkfffffffkkkkkk",
    "kJJJJJkkkkkkkkJJJJJk", "kjjjjjJkjjjjkJjjjjjk", "kjrrjjjkjrrjkjjjrrjk", "kjjjjjkkjjjjkkjjjjjk",
    "kkkkkkjjjjjjjjkkkkkk", "kjjjjkjjjJJjjjkFFFFk", "kJjjJkjjjjjjjjkFkkFk", "kffffkkBBBBBBkkFFFFk",
    "kkkkk.kBBBBBBk.kkkkk", ".....kppppppppk.....", ".....kpPPkkpPPk.....", "....kppPk..kpPpk....",
    "....kpPk....kpPk....", "....kpPk....kpPk....", "....kpPk....kpPk....", "...kbbbk....kbbbk...",
    "..kbbbbk....kbbbbk..", "..kkkkkk....kkkkkk..");
SPR(sESmaBrute1, 20, 26,
    "......kkkkkkkk......", ".....khhhhhhhhk.....", "....khJJJJJJJJhk....", "....khkkkkkkkkhk....",
    "....khvvvvvvvvhk....", "....khkkkkkkkkhk....", "....kffkfkfkfffk....", "kkkkkkkfffffffkkkkkk",
    "kJJJJJkkkkkkkkJJJJJk", "kjjjjjJkjjjjkJjjjjjk", "kjrrjjjkjrrjkjjjrrjk", "kjjjjjkkjjjjkkjjjjjk",
    "kkkkkkjjjjjjjjkkkkkk", "kjjjjkjjjJJjjjkFFFFk", "kJjjJkjjjjjjjjkFkkFk", "kffffkkBBBBBBkkFFFFk",
    "kkkkk.kBBBBBBk.kkkkk", ".....kppppppppk.....", ".....kpPPkkpPPk.....", ".....kpPk..kpPk.....",
    "....kpPk....kpPk....", "...kpPk......kpPk...", "...kpPk......kpPk...", "..kbbbk......kbbbk..",
    ".kbbbbk......kbbbbk.", ".kkkkkk......kkkkkk.");
SPR(sESmaDrone0, 14, 9,
    "kkkkk....kkkkk", "..k........k..", "..k.kkkkkk.k..", "...kjjjjjjk...", "..kjJJJJJJjk..", "..kjjvvvvjjk..",
    "...kjjjjjjk...", "....kk..kk....", "..............");
SPR(sESmaDrone1, 14, 9,
    ".kkk......kkk.", "..k........k..", "..k.kkkkkk.k..", "...kjjjjjjk...", "..kjJJJJJJjk..", "..kjjvvvvjjk..",
    "...kjjjjjjk...", "....kk..kk....", "..............");
/* STREET GANG */
SPR(sEGangThug0, 12, 19,
    "...kkkkk....", "..kjjjjjk...", ".kjJJJJjjk..", ".kjksssskk..", ".kjsBssBsk..", ".kjrrrrrrk..",
    "..kjrrrrk...", ".kJjjjjjJk..", "kJjjjjjjjJk.", "kjjJjjjjJjkk", "kjkjjjjjjkWk", "kskpppppkWk.",
    ".k.kpppk.kk.", "...kppppk...", "..kpPkkpPk..", "..kpPk.kpPk.", "..kpPk.kpPk.", "..kbbk.kbbk.",
    ".kbbbk.kbbbk");
SPR(sEGangThug1, 12, 19,
    "...kkkkk....", "..kjjjjjk...", ".kjJJJJjjk..", ".kjksssskk..", ".kjsBssBsk..", ".kjrrrrrrk..",
    "..kjrrrrk...", ".kJjjjjjJk..", "kJjjjjjjjJk.", "kjjJjjjjJjkk", "kjkjjjjjjkWk", "kskpppppkWk.",
    ".k.kpppk.kk.", "...kppppk...", "..kpPkkpPk..", ".kpPk..kpPk.", ".kpPk...kpk.", "kbbk....kbk.",
    "kbbbk...kbbk");
SPR(sEGangGun0, 12, 19,
    "..kkkkkk....", ".kgGGGGGk...", "kggggggggk..", ".kkhhhsssk..", ".khsBBBBsk..", ".khssssssk..",
    "..kssSsk....", ".kGGgggGGk..", "kGgggwggggk.", "kgGggwgggGk.", "kgkggwgggkk.", "kskgggggksk.",
    ".k.knnnk.k..", "...knnnnk...", "..kpPkkpPk..", "..kpPk.kpPk.", "..kpPk.kpPk.", "..kbbk.kbbk.",
    ".kbbbk.kbbbk");
SPR(sEGangGun1, 12, 19,
    "..kkkkkk....", ".kgGGGGGk...", "kggggggggk..", ".kkhhhsssk..", ".khsBBBBsk..", ".khssssssk..",
    "..kssSsk....", ".kGGgggGGk..", "kGgggwggggk.", "kgGggwgggGk.", "kgkggwgggkk.", "kskgggggksk.",
    ".k.knnnk.k..", "...knnnnk...", "..kpPkkpPk..", ".kpPk..kpPk.", ".kpPk...kpk.", "kbbk....kbk.",
    "kbbbk...kbbk");
SPR(sEGangBrute0, 20, 24,
    ".......kkkkkk.......", "......kssssssk......", ".....kssssssssk.....", ".....ksBBssBBsk.....",
    ".....kssssSsssk.....", ".....ksskkkkssk.....", "..kkkkkksssskkkkkk..", ".ksssskuuuuuukssssk.",
    "kssssuuUuyyuUuussssk", "ksssskuuuyyuuukssssk", "ksszskuuyyuuuuksszsk", "ksssskuuuuuuuukssssk",
    "kSSSSkuuUUUUuukSSSSk", "kkkkkkuuuuuuuukkkkkk", "....kknnnnnnnnkk....", ".....knnnnnnnnk.....",
    ".....kpPPkkpPPk.....", "....kppPk..kpPpk....", "....kpPk....kpPk....", "....kpPk....kpPk....",
    "....kpPk....kpPk....", "...kbbbk....kbbbk...", "..kbbbbk....kbbbbk..", "..kkkkkk....kkkkkk..");
SPR(sEGangBrute1, 20, 24,
    ".......kkkkkk.......", "......kssssssk......", ".....kssssssssk.....", ".....ksBBssBBsk.....",
    ".....kssssSsssk.....", ".....ksskkkkssk.....", "..kkkkkksssskkkkkk..", ".ksssskuuuuuukssssk.",
    "kssssuuUuyyuUuussssk", "ksssskuuuyyuuukssssk", "ksszskuuyyuuuuksszsk", "ksssskuuuuuuuukssssk",
    "kSSSSkuuUUUUuukSSSSk", "kkkkkkuuuuuuuukkkkkk", "....kknnnnnnnnkk....", ".....knnnnnnnnk.....",
    ".....kpPPkkpPPk.....", ".....kpPk..kpPk.....", "....kpPk....kpPk....", "...kpPk......kpPk...",
    "...kpPk......kpPk...", "..kbbbk......kbbbk..", ".kbbbbk......kbbbbk.", ".kkkkkk......kkkkkk.");
SPR(sEGangDrone0, 14, 9,
    "kkkkk....kkkkk", "..k........k..", "..k.kkkkkk.k..", "...kxxxxxxk...", "..kxXXXXXXxk..", "..kxXkrrkXxk..",
    "...kxxyxxxk...", "....kk..kk....", "..............");
SPR(sEGangDrone1, 14, 9,
    ".kkk......kkk.", "..k........k..", "..k.kkkkkk.k..", "...kxxxxxxk...", "..kxXXXXXXxk..", "..kxXkrrkXxk..",
    "...kxxyxxxk...", "....kk..kk....", "..............");
/* ZOMBIES */
SPR(sEZomThug0, 12, 18,
    "....kkkk....", "...kKKKKk...", "..kKhKKhk...", "..khhhhhhk..", "..khvhhvhk..", "..khhBBhhk..",
    "...khhhhkkkk", "..kjjjkhhhhk", ".kjJrjjkkkkk", ".kjjjjrjk...", ".kjjjjjjk...", "..kjrjjjk...",
    "..kppppk....", "..kpPkkpPk..", "..kpPk.kpPk.", "..kpPk.khPk.", "..kbbk.khhk.", ".kbbbk.khhhk");
SPR(sEZomThug1, 12, 18,
    "....kkkk....", "...kKKKKk...", "..kKhKKhk...", "..khhhhhhk..", "..khvhhvhk..", "..khhBBhhk..",
    "...khhhhkkkk", "..kjjjkhhhhk", ".kjJrjjkkkkk", ".kjjjjrjk...", ".kjjjjjjk...", "..kjrjjjk...",
    "..kppppk....", "..kpPkkpPk..", ".kpPk..kpPk.", ".kpPk...khk.", "kbbk....khk.", "kbbbk...khhk");
SPR(sEZomGun0, 12, 19,
    "...kkkkk....", "..kHHHHHk...", ".kHHHHHHHk..", ".kkkkkkkkk..", ".khhvhhvhk..", ".khhhhBhhk..",
    "..khBhhk....", ".kHHHHHHHk..", "kHHrHHHHHHk.", "kHHHHrHHHHk.", "khkHHHHHkhk.", "khkHHrHHkhk.",
    ".k.kpppk.k..", "...kppppk...", "..kpPkkpPk..", "..kpPk.kpPk.", "..kpPk.kpPk.", "..kbbk.kbbk.",
    ".kbbbk.kbbbk");
SPR(sEZomGun1, 12, 19,
    "...kkkkk....", "..kHHHHHk...", ".kHHHHHHHk..", ".kkkkkkkkk..", ".khhvhhvhk..", ".khhhhBhhk..",
    "..khBhhk....", ".kHHHHHHHk..", "kHHrHHHHHHk.", "kHHHHrHHHHk.", "khkHHHHHkhk.", "khkHHrHHkhk.",
    ".k.kpppk.k..", "...kppppk...", "..kpPkkpPk..", ".kpPk..kpPk.", ".kpPk...kpk.", "kbbk....kbk.",
    "kbbbk...kbbk");
SPR(sEZomBrute0, 20, 22,
    ".......kkkkkk.......", "......khhhhhhk......", ".....khhhhhhhhk.....", ".....khvhhhhvhk.....",
    ".....khhhBBhhhk.....", "....kkhhBwwBhhkk....", "..kkhhhhhhhhhhhhkk..", ".khhhjjjjjjjjjjhhhk.",
    "khhhjjjhhhhhhjjjhhhk", "khhkjjhhhrrhhhjjkhhk", "khhkjhhhhrrrhhhjkhhk", "khhkjhhhhhhhhhhjkhhk",
    "khhkjjhhhhhhhhjjkhhk", "kkkkkjjjjjjjjjjkkkkk", "....kppppppppppk....", "....kppppppppppk....",
    ".....kpPPkkpPPk.....", "....kpPPk..kpPPk....", "....kpPk....kpPk....", "...kbbbk....kbbbk...",
    "..kbbbbk....kbbbbk..", "..kkkkkk....kkkkkk..");
SPR(sEZomBrute1, 20, 22,
    ".......kkkkkk.......", "......khhhhhhk......", ".....khhhhhhhhk.....", ".....khvhhhhvhk.....",
    ".....khhhBBhhhk.....", "....kkhhBwwBhhkk....", "..kkhhhhhhhhhhhhkk..", ".khhhjjjjjjjjjjhhhk.",
    "khhhjjjhhhhhhjjjhhhk", "khhkjjhhhrrhhhjjkhhk", "khhkjhhhhrrrhhhjkhhk", "khhkjhhhhhhhhhhjkhhk",
    "khhkjjhhhhhhhhjjkhhk", "kkkkkjjjjjjjjjjkkkkk", "....kppppppppppk....", "....kppppppppppk....",
    ".....kpPPkkpPPk.....", "....kpPk....kpPk....", "...kpPk......kpPk...", "..kbbbk......kbbbk..",
    ".kbbbbk......kbbbbk.", ".kkkkkk......kkkkkk.");
SPR(sEZomDrone0, 14, 9,
    "..............", "..kkk...kkk...", ".kFFFk.kFFFk..", "..kFFFkFFFk...", "...kkkkkkkk...", "..khKhKhkvvk..",
    ".khhKhKhkvvvk.", "..khKhKhkvvk..", "...k.k.k.k....");
SPR(sEZomDrone1, 14, 9,
    "..............", "..............", "..kkkk.kkkk...", "..kFFFkFFFk...", "...kkkkkkkk...", "..khKhKhkvvk..",
    ".khhKhKhkvvvk.", "..khKhKhkvvk..", "...k.k.k.k....");
/* ROBOTS */
SPR(sEBotThug0, 12, 17,
    "......k.....", ".....krk....", "..kkkkkkkk..", ".kJhhhhhhJk.", ".khkvvvvkhk.", ".khhhhhhhhk.",
    ".kkkkkkkkkk.", "kJkjJJJJjkJk", "khkjjrrjjkhk", "khkjjjjjjkhk", "kfkjjjjjjkfk", ".k.kkkkkkk..",
    "...kpppk....", "..kPPk.kPPk.", "..kpPk.kpPk.", ".kbbbk.kbbbk", ".kkkkk.kkkkk");
SPR(sEBotThug1, 12, 17,
    "......k.....", ".....krk....", "..kkkkkkkk..", ".kJhhhhhhJk.", ".khkvvvvkhk.", ".khhhhhhhhk.",
    ".kkkkkkkkkk.", "kJkjJJJJjkJk", "khkjjrrjjkhk", "khkjjjjjjkhk", "kfkjjjjjjkfk", ".k.kkkkkkk..",
    "...kpppk....", ".kPPk..kPPk.", ".kpPk...kpk.", "kbbbk...kbbk", "kkkkk...kkkk");
SPR(sEBotGun0, 12, 21,
    "....kkkk....", "...khhhhk...", "..khJJhhhk..", "..khhkvvvk..", "..khhhhhhk..", "..kfkfkfhk..",
    "...kkhhkk...", "....kFFk....", "...kkFFkk...", ".kkJJjjJJkk.", "kJjjjvvjjjJk", "kjkjjjjjjkjk",
    "khkjjjjjjkhk", "kfk.kjjk.kfk", "...kpppk....", "...kpppk....", "..kpPkkpPk..", "..kpPk.kpPk.",
    "..kpPk.kpPk.", "..kbbk.kbbk.", ".kbbbk.kbbbk");
SPR(sEBotGun1, 12, 21,
    "....kkkk....", "...khhhhk...", "..khJJhhhk..", "..khhkvvvk..", "..khhhhhhk..", "..kfkfkfhk..",
    "...kkhhkk...", "....kFFk....", "...kkFFkk...", ".kkJJjjJJkk.", "kJjjjvvjjjJk", "kjkjjjjjjkjk",
    "khkjjjjjjkhk", "kfk.kjjk.kfk", "...kpppk....", "...kpppk....", "..kpPkkpPk..", ".kpPk..kpPk.",
    ".kpPk...kpk.", "kbbk....kbk.", "kbbbk...kbbk");
SPR(sEBotBrute0, 20, 20,
    "......kkkkkkkk......", ".....kJJJJJJJJk.....", ".....khhhhhhhhk.....", ".....khvvvvvvhk.....",
    ".....khvkvvkvhk.....", ".....khhhhhhhhk.....", "..kkkkkkkkkkkkkkkk..", ".kJJkJjjjjjjjjJkJJk.",
    "kJhhkjjrrjjrrjjkhhJk", "khhhkjjjjjjjjjjkhhhk", "khhhkjjJJJJJJjjkhhhk", "kffkkjjjjjjjjjjkkffk",
    "kfkfkjjjjjjjjjjkfkfk", "kkkkkkkkkkkkkkkkkkkk", "....kkkkkkkkkkkk....", "...kPPPPPPPPPPPPk...",
    "..kPkbPbkbPbkbPbPk..", "..kPbkbPbkbPbkbPkk..", "...kPPPPPPPPPPPPk...", "....kkkkkkkkkkkk....");
SPR(sEBotBrute1, 20, 20,
    "......kkkkkkkk......", ".....kJJJJJJJJk.....", ".....khhhhhhhhk.....", ".....khvvvvvvhk.....",
    ".....khvkvvkvhk.....", ".....khhhhhhhhk.....", "..kkkkkkkkkkkkkkkk..", ".kJJkJjjjjjjjjJkJJk.",
    "kJhhkjjrrjjrrjjkhhJk", "khhhkjjjjjjjjjjkhhhk", "khhhkjjJJJJJJjjkhhhk", "kffkkjjjjjjjjjjkkffk",
    "kfkfkjjjjjjjjjjkfkfk", "kkkkkkkkkkkkkkkkkkkk", "....kkkkkkkkkkkk....", "...kPPPPPPPPPPPPk...",
    "..kPbkbPbkbPbkbPkk..", "..kPkbPbkbPbkbPbPk..", "...kPPPPPPPPPPPPk...", "....kkkkkkkkkkkk....");
SPR(sEBotDrone0, 14, 9,
    "kkkkk....kkkkk", "..k........k..", "..k..kkkk..k..", "...kkhhhhkk...", "..khhJJJJhhk..", "..khkvvvvkhk..",
    "..khhvkkvhhk..", "...kkhhhhkk...", "......kk......");
SPR(sEBotDrone1, 14, 9,
    ".kkk......kkk.", "..k........k..", "..k..kkkk..k..", "...kkhhhhkk...", "..khhJJJJhhk..", "..khkvvvvkhk..",
    "..khhvkkvhhk..", "...kkhhhhkk...", "......kk......");
/* CLOWNS */
SPR(sEClownThug0, 12, 18,
    ".kk.kkkk.kk.", "khhkwwwwkhhk", "khhwwwwwwhhk", ".kkwBwwBwkk.", "..kwwwwwrk..", "..kwrrrrwk..",
    "...kwwwwk...", ".kiwiwiwik..", "kJjjjijjjJk.", "kjJjjijjJjk.", "kwkjjijjkwk.", ".k.kpppk.k..",
    "...kppppk...", "..kpPkkpPk..", "..kpPk.kpPk.", "..kpPk.kpPk.", ".kbbbkkbbbbk", "kbbbbkkbbbbk");
SPR(sEClownThug1, 12, 18,
    ".kk.kkkk.kk.", "khhkwwwwkhhk", "khhwwwwwwhhk", ".kkwBwwBwkk.", "..kwwwwwrk..", "..kwrrrrwk..",
    "...kwwwwk...", ".kiwiwiwik..", "kJjjjijjjJk.", "kjJjjijjJjk.", "kwkjjijjkwk.", ".k.kpppk.k..",
    "...kppppk...", "..kpPkkpPk..", ".kpPk..kpPk.", ".kpPk...kpk.", "kbbbk...kbbk", "kbbbbk..kbbb");
SPR(sEClownGun0, 12, 19,
    "kk.....kk...", "kyk...kyk...", ".kjk.kJk....", ".kjjkJJk....", ".kjjjJJJk...", ".kwwwwwwk...",
    ".kwwBwBwk...", ".kiwiwiwik..", "kjjjjJJJJJk.", "kjjijJJiJJk.", "kjjjjJJJJJk.", "kwkjjJJJkwk.",
    ".k.kjjJJk...", "...kppPPk...", "..kpPkkpPk..", "..kpPk.kpPk.", "..kpPk.kpPk.", "..kbbk.kbbk.",
    ".kbbbk.kbbbk");
SPR(sEClownGun1, 12, 19,
    "kk.....kk...", "kyk...kyk...", ".kjk.kJk....", ".kjjkJJk....", ".kjjjJJJk...", ".kwwwwwwk...",
    ".kwwBwBwk...", ".kiwiwiwik..", "kjjjjJJJJJk.", "kjjijJJiJJk.", "kjjjjJJJJJk.", "kwkjjJJJkwk.",
    ".k.kjjJJk...", "...kppPPk...", "..kpPkkpPk..", ".kpPk..kpPk.", ".kpPk...kpk.", "kbbk....kbk.",
    "kbbbk...kbbk");
SPR(sEClownBrute0, 20, 24,
    ".......kkkkkk.......", "......kwwwwwwk......", ".....kwwwwwwwwk.....", ".....kwBwwwwBwk.....",
    ".....kwwwwwwrrk.....", ".....kKKKwwKKKk.....", "..kkkkkkwrrwkkkkkk..", ".ksssskrwrwrwkssssk.",
    "kssssskwrwrwrksssssk", "ksssskrwrwrwrwkssssk", "ksssskwrwrwrwrkssssk", "ksssskrwrwrwrwkssssk",
    "kSSSSkwrwrwrwrkSSSSk", "kkkkkkjjjjjjjjkkkkkk", "....kkppppppppkk....", ".....kppppppppk.....",
    ".....kpPPkkpPPk.....", "....kppPk..kpPpk....", "....kpPk....kpPk....", "....kpPk....kpPk....",
    "....kpPk....kpPk....", "...kbbbk....kbbbk...", "..kbbbbk....kbbbbk..", "..kkkkkk....kkkkkk..");
SPR(sEClownBrute1, 20, 24,
    ".......kkkkkk.......", "......kwwwwwwk......", ".....kwwwwwwwwk.....", ".....kwBwwwwBwk.....",
    ".....kwwwwwwrrk.....", ".....kKKKwwKKKk.....", "..kkkkkkwrrwkkkkkk..", ".ksssskrwrwrwkssssk.",
    "kssssskwrwrwrksssssk", "ksssskrwrwrwrwkssssk", "ksssskwrwrwrwrkssssk", "ksssskrwrwrwrwkssssk",
    "kSSSSkwrwrwrwrkSSSSk", "kkkkkkjjjjjjjjkkkkkk", "....kkppppppppkk....", ".....kppppppppk.....",
    ".....kpPPkkpPPk.....", ".....kpPk..kpPk.....", "....kpPk....kpPk....", "...kpPk......kpPk...",
    "...kpPk......kpPk...", "..kbbbk......kbbbk..", ".kbbbbk......kbbbbk.", ".kkkkkk......kkkkkk.");
SPR(sEClownDrone0, 17, 11,
    "..kkk..kkk..kkk..", ".krrwkkyywkkPPwk.", ".krrrkkyyykkPPPk.", ".krrrkkyyykkPPPk.", "..kkk..kkk..kkk..",
    "...F....F....F...", "....F...F...F....", ".....F..F..F.....", "......kkkkk......", "......kkwkk......",
    "......kkkkk......");
SPR(sEClownDrone1, 17, 11,
    "..kkk..kkk..kkk..", ".krrwkkyywkkPPwk.", ".krrrkkyyykkPPPk.", ".krrrkkyyykkPPPk.", "..kkk..kkk..kkk..",
    "....F...F...F....", "....F...F...F....", ".....F..F..F.....", "......kkkkk......", "......kkwkk......",
    "......kkkkk......");
/* ALIENS */
SPR(sEAlienThug0, 12, 17,
    "...kkkkkk...", "..khhhhhhk..", ".khhhhhhhhk.", ".khvvhhvvhk.", ".khvvhhvvhk.", "..khhhhhhk..",
    "...khhhhk...", "....khhk....", "..kkjjjjkk..", ".khkjrrjkhk.", ".khkjjjjkhk.", "..k.kjjk.k..",
    "....kjjk....", "...kpk.kpk..", "...kpk.kpk..", "...kpk.kpk..", "..kbbk.kbbk.");
SPR(sEAlienThug1, 12, 17,
    "...kkkkkk...", "..khhhhhhk..", ".khhhhhhhhk.", ".khvvhhvvhk.", ".khvvhhvvhk.", "..khhhhhhk..",
    "...khhhhk...", "....khhk....", "..kkjjjjkk..", ".khkjrrjkhk.", ".khkjjjjkhk.", "..k.kjjk.k..",
    "....kjjk....", "...kpk.kpk..", "..kpk...kpk.", ".kpk....kpk.", ".kbk.....kbk");
SPR(sEAlienGun0, 12, 21,
    "...kkkkkk...", "..kzzzzzzk..", ".kzwhhhhzzk.", ".kzhvvhvvzk.", ".kzhvvhvvzk.", ".kzhhhhhhzk.",
    ".kzzhhhhzzk.", "..kzzzzzzk..", "...kJJJJk...", ".kkJjjjjJkk.", "kJjjjrrjjjJk", "kjkjjjjjjkjk",
    "kjkjJJJJjkjk", "kJk.kjjk.kJk", "...kjjjjk...", "...kppppk...", "..kpPkkpPk..", "..kpPk.kpPk.",
    "..kpPk.kpPk.", "..kbbk.kbbk.", ".kbbbk.kbbbk");
SPR(sEAlienGun1, 12, 21,
    "...kkkkkk...", "..kzzzzzzk..", ".kzwhhhhzzk.", ".kzhvvhvvzk.", ".kzhvvhvvzk.", ".kzhhhhhhzk.",
    ".kzzhhhhzzk.", "..kzzzzzzk..", "...kJJJJk...", ".kkJjjjjJkk.", "kJjjjrrjjjJk", "kjkjjjjjjkjk",
    "kjkjJJJJjkjk", "kJk.kjjk.kJk", "...kjjjjk...", "...kppppk...", "..kpPkkpPk..", ".kpPk..kpPk.",
    ".kpPk...kpk.", "kbbk....kbk.", "kbbbk...kbbk");
SPR(sEAlienBrute0, 20, 22,
    ".......kkkkkk.......", ".....kkhhhhhhkk.....", "....khhhhhhhhhhk....", "...khhhkkkkkkhhhk...",
    "...khhkwwwwwwkhhk...", "...khhkwwvvwwkhhk...", "...khhkwwvvwwkhhk...", "...khhhkkkkkkhhhk...",
    "..kkhhhhhhhhhhhhkk..", ".khhkhhrhhhhrhhkhhk.", "khhk.khhhhhhhhk.khhk", "khk..khhhrrhhhk..khk",
    "kk...khhhhhhhhk...kk", ".....khhhhhhhhk.....", "....kkhhhhhhhhkk....", "...khhkhhhhhhkhhk...",
    "..khhk.khhhhk.khhk..", "..khk..khkkhk..khk..", ".khhk.khk..khk.khhk.", ".khk..khk..khk..khk.",
    "khk..khk....khk..khk", "kk...kk......kk...kk");
SPR(sEAlienBrute1, 20, 22,
    ".......kkkkkk.......", ".....kkhhhhhhkk.....", "....khhhhhhhhhhk....", "...khhhkkkkkkhhhk...",
    "...khhkwwwwwwkhhk...", "...khhkwwvvwwkhhk...", "...khhkwwvvwwkhhk...", "...khhhkkkkkkhhhk...",
    "..kkhhhhhhhhhhhhkk..", ".khhkhhrhhhhrhhkhhk.", "khhk.khhhhhhhhk.khhk", "khk..khhhrrhhhk..khk",
    "kk...khhhhhhhhk...kk", ".....khhhhhhhhk.....", "....kkhhhhhhhhkk....", "...khhkhhhhhhkhhk...",
    "..khhk.khhhhk.khhk..", "..khk..khkkhk..khk..", "..khk.khk..khk.khk..", "...khkhk....khkhk...",
    "...kkhk......khkk...", "....kk........kk....");
SPR(sEAlienDrone0, 16, 8,
    "......kkkk......", ".....kzwzzk.....", "....kzhvvzzk....", "..kkkkkkkkkkkk..", ".kJJJJJJJJJJJJk.",
    "kjrjjrjjrjjrjjjk", ".kkkkkkkkkkkkkk.", "....kk....kk....");
SPR(sEAlienDrone1, 16, 8,
    "......kkkk......", ".....kzwzzk.....", "....kzhvvzzk....", "..kkkkkkkkkkkk..", ".kJJJJJJJJJJJJk.",
    "kjjrjjrjjrjjrjjk", ".kkkkkkkkkkkkkk.", "....kk....kk....");

SPR(sHeart, 7, 6, ".kk.kk.", "krwkrrk", "krrrrrk", ".krrrk.", "..krk..", "...k...");
SPR(sMedkit, 10, 9,
    "...kkkk...", "kkkwwwwkkk", "kwwwrrwwwk", "kwwwrrwwwk", "kwrrrrrrwk",
    "kwrrrrrrwk", "kwwwrrwwwk", "kwwwrrwwwk", "kkkkkkkkkk");

SPR(sIcPistol, 13, 5, "kkkkkkkk.....", "kXXXXXXXk....", "kkkkkcck.....", "....kcck.....", "....kkk......");
SPR(sIcSmg, 13, 5, "kkkkkkkkkkk..", "kXXXXXXXXXXk.", "kkkkcckkcckk.", "...kcck.kck..", "...kkk..kk...");
SPR(sIcShotgun, 13, 5, "kkkkkkkkkkkkk", "kWWWkXXXXXXXk", "kWWkkkkkkkkkk", "kkk..........", ".............");
SPR(sIcLaser, 13, 5, ".kkkkkkkkkkk.", "kXqqqqqqqqqXk", "kXwwwwwwwwwXk", ".kkkkXkkkkkk.", "....kk.......");
SPR(sIcGrenade, 13, 5, "..kkkkkkkk...", ".kGGGGGGGGkkk", "kGgggggggGXXk", ".kGGGGGGGGkkk", "..kkk.kk.....");
SPR(sIcRocket, 13, 5, "kkkkkkkkkkkk.", "kooHHHHHHHHHk", "kHHHHHHHHHHHk", "kkkkkcckkkkk.", ".....kk......");
SPR(sIcGravity, 13, 5, ".kkkkkkkk.kk.", "kUUUUUUUUkMMk", "kuqqqqquukmmk", "kUUUUUUUUkMMk", ".kkk.kk...kk.");

unsigned bossArmor = C(70, 80, 100), bossArmorLight = C(110, 120, 140);
const unsigned *skinOv; /* while drawing the player: hair, visor, scarf, jacket, jacket light, pants, pants light, boots */

unsigned eTab[128]; /* enemy skin: letter -> colour while an enemy is being drawn */
int eTabOn;

unsigned pal(char ch)
{
    if (eTabOn && (unsigned char)ch < 128 && eTab[(unsigned char)ch]) return eTab[(unsigned char)ch];
    if (skinOv) {
        switch (ch) {
        case 'h': return skinOv[0]; case 'v': return skinOv[1]; case 'r': return skinOv[2]; case 'j': return skinOv[3];
        case 'J': return skinOv[4]; case 'p': return skinOv[5]; case 'P': return skinOv[6]; case 'b': return skinOv[7];
        }
    }
    switch (ch) {
    case 'k': return C_OUT;
    case 'h': return C(44, 32, 44);
    case 's': return C(232, 180, 140);
    case 'S': return C(190, 130, 100);
    case 'v': return C(80, 230, 255);
    case 'r': return C(220, 60, 70);
    case 'j': return C(58, 68, 108);
    case 'J': return C(92, 106, 160);
    case 'p': return C(40, 40, 58);
    case 'P': return C(66, 66, 90);
    case 'b': return C(28, 24, 30);
    case 't': return C(140, 36, 52);
    case 'T': return C(186, 64, 72);
    case 'm': return C(36, 30, 40);
    case 'e': return C(255, 214, 90);
    case 'n': return C(50, 60, 90);
    case 'N': return C(72, 86, 124);
    case 'g': return C(60, 80, 70);
    case 'G': return C(90, 116, 98);
    case 'd': return C(36, 36, 46);
    case 'D': return C(56, 56, 70);
    case 'c': return C(70, 74, 90);
    case 'C': return C(110, 116, 136);
    case 'R': return C(255, 50, 50);
    case 'x': return C(60, 58, 70);
    case 'X': return C(130, 130, 148);
    case 'o': return C(255, 150, 40);
    case 'u': return C(80, 60, 120);
    case 'U': return C(120, 96, 170);
    case 'y': return C(250, 220, 80);
    case 'a': return bossArmor;
    case 'A': return bossArmorLight;
    case 'q': return C(90, 230, 255);
    case 'w': return C(240, 240, 240);
    case 'W': return C(120, 82, 50);
    case 'Z': return C(168, 120, 72);
    case 'H': return C(120, 130, 70);
    case 'B': return C(20, 18, 26);   /* black tee */
    case 'l': return C(160, 108, 76); /* darker skin */
    case 'L': return C(118, 76, 54);
    case 'i': return C(255, 70, 160); /* hot pink */
    case 'f': return C(196, 202, 218); /* chrome */
    case 'F': return C(130, 136, 158);
    case 'E': return C(225, 40, 70);  /* red eyeliner */
    case 'K': return C(70, 46, 34);   /* beard */
    case 'z': return C(50, 210, 190); /* teal */
    }
    return C(255, 0, 255);
}
