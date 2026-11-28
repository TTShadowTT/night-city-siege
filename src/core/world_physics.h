/* world_physics.h - Ledge and street collision for anything that walks, falls or climbs. */
#ifndef NCS_WORLD_PHYSICS_H
#define NCS_WORLD_PHYSICS_H

float surfaceBelow(float x, float y, int *platIdx);
int platAt(float x, float y);
/* move a body with gravity; one-way platforms catch it only while falling. returns 1 when standing */
int bodyStep(float *x, float *y, float *vx, float *vy, float hw, float hh, int *onPlat, float dropT, int gravity, float dt);

#endif
