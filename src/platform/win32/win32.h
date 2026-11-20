/*
 * win32.h - what the Windows platform files share.
 *
 * The Windows exe shows the game core's 480x270 pixels scaled up to the window or the whole screen, plays its
 * sounds, decodes MY SONGS and the built-in map pictures, and forwards input to the core (see game.h).
 */
#ifndef NCS_WIN32_H
#define NCS_WIN32_H

#define COBJMACROS
#include <windows.h>
#include <mmsystem.h>
#include <commdlg.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <wincodec.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"

#include "win32_storage.h"
#include "win32_audio.h"
#include "win32_main.h"
#include "win32_pad.h"

#endif
