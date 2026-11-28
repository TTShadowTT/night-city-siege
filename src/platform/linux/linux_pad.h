/* linux_pad.h - Game controllers on Linux, through the kernel's joystick devices (/dev/input/js0...). */
#ifndef NCS_LINUX_PAD_H
#define NCS_LINUX_PAD_H

/* reads the first controller found and hands it to the game (call once per frame) */
void padPoll(void);

#endif
