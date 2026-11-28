/* gameplay_enemies.h - Enemy AI, including the giant bosses. */
#ifndef NCS_GAMEPLAY_ENEMIES_H
#define NCS_GAMEPLAY_ENEMIES_H

float megaAim(const Enemy *e, float *sx, float *sy);
void megaGun(const Enemy *e, float *gx, float *gy);
void updateEnemies(float dt);

#endif
