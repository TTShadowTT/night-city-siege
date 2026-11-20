/* audio_sound.h - Sound effects, synthesised into sample arrays at start-up. */
#ifndef NCS_AUDIO_SOUND_H
#define NCS_AUDIO_SOUND_H

enum { S_PISTOL, S_SMG, S_SHOTGUN, S_LASER, S_LAUNCH, S_ROCKET, S_BOOM, S_HIT, S_KILL, S_HURT, S_PICKUP,
       S_WAVE, S_DASH, S_ESHOT, S_BOSS, S_SWITCH, S_JUMP, S_ROPE, S_RELOAD, S_WARP, S_CLANG, S_PERK, S_COUNT };

extern int sndOff[S_COUNT], sndLen[S_COUNT];

void genSounds(void);

#endif
