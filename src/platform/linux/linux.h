/*
 * linux.h - what the Linux platform files share.
 *
 * The Linux game is a small native program, like the Windows exe: it shows the game core's 480x270 pixels scaled up
 * in an X11 window (also under Wayland, through XWayland), plays its sounds through PulseAudio / PipeWire, and
 * forwards input to the core (see game.h). No browser needed. The sound library is opened at run time, so a computer
 * without it still runs the game, just silently.
 */
#ifndef NCS_LINUX_H
#define NCS_LINUX_H

#define _GNU_SOURCE
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"

#include "linux_storage.h"
#include "linux_audio.h"
#include "linux_main.h"
#include "linux_pad.h"

#endif
