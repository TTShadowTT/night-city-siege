/* gameplay_spawn.h - Spawning particles, pickups, enemies, bosses and waves. */
#ifndef NCS_GAMEPLAY_SPAWN_H
#define NCS_GAMEPLAY_SPAWN_H

/* big boss looks: picked at random each time (never the same twice in a row), not unlockable as skins.
   style = signature attack: 0 bullet ring, 1 bullet spiral, 2 homing missiles */
#define MEGA_COUNT 6

extern const char *const MEGA_NAME[MEGA_COUNT];
extern const unsigned MEGA_COL[MEGA_COUNT];
extern const int MEGA_STYLE[MEGA_COUNT];

void addPart(float x, float y, float vx, float vy, float life, unsigned c, int sz, float grav, int bounce);
void bleed(float x, float y, int n, float dirx, float spd);
void gore(float x, float y, int n, float dirx, float spd);
void burst(float x, float y, int n, float spd, unsigned c, float life);
void floater(float x, float y, const char *s, unsigned c);
void addBoom(float x, float y, float r);
void addRopeFx(float x, float y0, float y1);
void spawnEShot(float x, float y, float vx, float vy, int kind);
/* crates parachute in from above; enemy drops pop out where the enemy died */
void dropPickup(float x, float y, int type, int chute);
Enemy *spawnEnemy(int type, float x, float anchorY);
int countAlive(void);
int pickType(void);
void spawnFromQueue(void);
/* a big boss with skin k at strength tier (1 = wave 20), dropping in on the side away from you */
Enemy *spawnMega(int k, int tier);
void startWave(void);
void resetPlayer(void);
void newGame(void);

#endif
