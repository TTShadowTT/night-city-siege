/* world_districts.h - The districts (maps): their ledges, ropes, enemies and colours. */
#ifndef NCS_WORLD_DISTRICTS_H
#define NCS_WORLD_DISTRICTS_H

enum { EN_THUG, EN_DRONE, EN_GUNNER, EN_BOMBER, EN_BRUTE, EN_SHIELD, EN_BOSS, EN_NINJA, EN_JET, EN_MECH, EN_TYPES };
#define EN_NEW 3 /* EN_NINJA.. join the waves one by one, each after a big boss */
#define EN_GRUNTS EN_BOSS /* the types a wave is built from */
enum { D_DOWNTOWN, D_NEON, D_HARBOR, D_SKYLINE, D_FOREST, D_MEADOW, D_STORM, D_MOUNTAIN, D_NIGHTCITY, D_ARASAKA, D_CITYCENTER, D_MOON, D_ORBIT, D_LAUNCH, D_FREEFALL, D_COUNT };
#define IS_NATURE(m) ((m) >= D_FOREST && (m) <= D_MOUNTAIN) /* outdoor maps: ridges and trees instead of buildings */

typedef struct { float x, y, w; } Plat;
typedef struct { float x, y0, y1; } Rope;
typedef struct {
    const char *name, *blurb, *bossName;
    int nPlat; Plat plat[12];
    int nRope; Rope rope[8];
    int signature, weights[EN_GRUNTS];
    unsigned skyTop, skyBot, farCol, midCol, haze, neonA, neonB, armor, armorLight;
    int moonX, moonY, moonR;
} District;

extern const District DISTRICTS[D_COUNT];
extern int curMap;
extern const District *D;
extern float gravK; /* gravity multiplier: low on the moon */

#endif
