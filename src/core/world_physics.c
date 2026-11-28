/* world_physics.c - Ledge and street collision for anything that walks, falls or climbs. */
#include "core.h"

/* physics helpers */

/* top of the first platform (or the street) at or below y, at horizontal position x */
float surfaceBelow(float x, float y, int *platIdx)
{
    float best = GROUND_Y;
    if (platIdx) *platIdx = -1;
    for (int i = 0; i < D->nPlat; i++) {
        const Plat *p = &D->plat[i];
        if (x > p->x + 2 && x < p->x + p->w - 2 && p->y >= y && p->y < best) { best = p->y; if (platIdx) *platIdx = i; }
    }
    return best;
}

int platAt(float x, float y)
{
    for (int i = 0; i < D->nPlat; i++) {
        const Plat *p = &D->plat[i];
        if (x >= p->x && x <= p->x + p->w && fabsf(p->y - y) < 2) return i;
    }
    return -1;
}

/* move a body with gravity; one-way platforms catch it only while falling. returns 1 when standing */
int bodyStep(float *x, float *y, float *vx, float *vy, float hw, float hh, int *onPlat, float dropT, int gravity, float dt)
{
    if (gravity) { *vy += GRAV * gravK * dt; if (*vy > 420) *vy = 420; }
    float oldFeet = *y + hh;
    *x += *vx * dt;
    if (*x < hw) { *x = hw; *vx = 0; }
    if (*x > FW - hw) { *x = FW - hw; *vx = 0; }
    *y += *vy * dt;
    float feet = *y + hh;
    if (onPlat) *onPlat = -1;
    if (feet >= GROUND_Y) { *y = GROUND_Y - hh; if (*vy > 0) *vy = 0; return 1; }
    if (*vy >= 0 && dropT <= 0)
        for (int i = 0; i < D->nPlat; i++) {
            const Plat *p = &D->plat[i];
            if (*x + hw > p->x + 1 && *x - hw < p->x + p->w - 1 && oldFeet <= p->y + 1 && feet >= p->y) {
                *y = p->y - hh; *vy = 0;
                if (onPlat) *onPlat = i;
                return 1;
            }
        }
    return 0;
}
