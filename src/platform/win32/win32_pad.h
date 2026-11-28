/* win32_pad.h - Game controllers on Windows: Xbox pads through XInput, PlayStation pads through the joystick API. */
#ifndef NCS_WIN32_PAD_H
#define NCS_WIN32_PAD_H

/* reads the first controller found and hands it to the game (call once per frame) */
void padPoll(void);

#endif
