/* api_pad.h - Game controllers: the platform hands over the pad each frame, this turns it into keys, aim and fire. */
#ifndef NCS_API_PAD_H
#define NCS_API_PAD_H

extern int padSeen;  /* a controller has been used: the KEYS screen shows its buttons */
extern int padAim;   /* the right stick is aiming (the mouse takes over again when it moves) */

#endif
