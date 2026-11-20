/* render_giants.h - Enemy sprites, arm guns and the six giant bosses, painted in their own buffer. */
#ifndef NCS_RENDER_GIANTS_H
#define NCS_RENDER_GIANTS_H

void drawArmGun(float sx, float sy, float a, int len, int thick, unsigned col, int muzzle, unsigned armCol);
const Spr *enemySprite(const Enemy *e, int *flip);
void drawGiant(const Enemy *e);

#endif
