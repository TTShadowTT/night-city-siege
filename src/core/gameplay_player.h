/* gameplay_player.h - The player, gravity-gun holes and every kind of shot. */
#ifndef NCS_GAMEPLAY_PLAYER_H
#define NCS_GAMEPLAY_PLAYER_H

void updatePlayer(float dt);
/* black holes pull enemies (and their bullets) in, crush whatever reaches the core, then collapse */
void updateHoles(float dt);
/* a black hole crushed an enemy (counted per hole) */
void holeKill(void);
void updateShots(float dt);

#endif
