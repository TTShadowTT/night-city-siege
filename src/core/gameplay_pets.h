/* gameplay_pets.h - Pets: the list of pets and their collars. */
#ifndef NCS_GAMEPLAY_PETS_H
#define NCS_GAMEPLAY_PETS_H

/* pets
 * A pet runs along at your side; it can't be hurt and only leaves when you go down. Bought on the PETS tab
 * of the skins screen; its collar is set on the EDIT screen under it in the main menu. Shapes are drawn
 * facing right, feet on the last row, with an outline added around them:
 * b body, d dark marks, l light belly / muzzle, e eye, w eye shine, n nose / beak,
 * c collar (body when the collar is off), C collar (light when it's off). Frames: 0 stand, 1 and 2 walk. */
enum { PS_CAT, PS_DOG, PS_HAMSTER, PS_BUNNY, PS_PENGUIN, PS_FROG, PS_DRAGON, PS_BEAR, PS_DUCK, PS_OWL, PS_SLIME, PS_AXOLOTL, PS_BAT, PS_BUTTERFLY, PS_BEE, PS_DRONE, PS_GHOST, PS_PARROT, PS_UFO, PS_SLUDGE, PS_COUNT };
typedef struct { int w, h; const char *rows[3][10]; int fly; } PetShape;
typedef struct { const char *name; int shape, cost; unsigned body, dark, light, eye, shine, nose; } Pet;
#define PET_COUNT 41 /* entries in PETS */
#define PET_TOXIC 31 /* the TOXIC SLIME leaves acid drips */
#define COLLAR_COUNT 9 /* entries in COLLAR_COL */

extern const PetShape PET_SHAPES[PS_COUNT];
extern const Pet PETS[];
extern const unsigned COLLAR_COL[];
extern int pet, petsMask, petsMask2, collarOn, collarCol; /* saved (SLOP8; SLOP9 adds petsMask2 for pets 31 and up) */
/* names you gave your pets ("#P id name ..." in the save); empty = the kind's name. petNameEdit: typing one right now */
#define PETNAME_MAX 10
extern char petNames[PET_COUNT][PETNAME_MAX + 1];
extern int petNameEdit, petNameFresh;
const char *petName(int k);
extern float petX, petY, petVx, petVy, petWalk;
extern int petFace, petGround, petPlat;
extern char toastTxt[64];
extern float toastT;

int petGoo(int k);
int petFlies(int k);
int petFlap(float t);
int ownsPet(int i);
void givePet(int i);

#endif
