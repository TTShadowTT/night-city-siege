/* gameplay_combat.h - Combat: hurting the player, damaging and killing enemies, explosions and weapons. */
#ifndef NCS_GAMEPLAY_COMBAT_H
#define NCS_GAMEPLAY_COMBAT_H

extern int killW; /* the weapon dealing the damage right now (for daily quests), -1 = none */
extern int gravityKill; /* set while a black hole is crushing something */

void hurtPlayer(float fx, float fy, int dmg);
/* people bleed; drones and bomb-bots spark */
int isFlesh(int t);
void announce(const char *s, unsigned c);
void damageEnemy(Enemy *e, float dmg, float kx, float ky);
int inBox(const Enemy *e, float x, float y, float m);
void explode(float x, float y, float radius, float dmg, int byPlayer);
void shoulder(float *sx, float *sy);
void startReload(void);
void fireWeapon(void);
void selectWeapon(int w);
void cycleWeapon(int dir);
void givePickup(Pickup *p);
void dmgFlush(Enemy *e);

#endif
