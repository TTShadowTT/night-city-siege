/* win32_pad.c - Game controllers on Windows: Xbox pads through XInput, anything else (PlayStation pads) through
 * the old joystick API, read once per frame and handed to the game core (game_pad in game.h). */
#include "win32.h"
#include <xinput.h>

typedef DWORD(WINAPI *XInputGetStateFn)(DWORD, XINPUT_STATE *);
static XInputGetStateFn xget;
static int xtried, xslot = -1, joyId = -1;
static DWORD xNext, jNext; /* asking an empty slot is slow, so look for a new controller only now and then */

static float stick(SHORT v) { return v < 0 ? v / 32768.f : v / 32767.f; }

static int readXInput(void)
{
    XINPUT_STATE st;
    if (!xtried) { /* xinput1_4 ships with Windows 8 and up, 9_1_0 with every Windows since Vista */
        HMODULE m = LoadLibraryA("xinput1_4.dll");
        if (!m) m = LoadLibraryA("xinput9_1_0.dll");
        if (m) xget = (XInputGetStateFn)(void (*)(void))GetProcAddress(m, "XInputGetState");
        xtried = 1;
    }
    if (!xget) return 0;
    if (xslot < 0) {
        if (GetTickCount() < xNext) return 0;
        for (int i = 0; i < XUSER_MAX_COUNT && xslot < 0; i++) if (xget(i, &st) == ERROR_SUCCESS) xslot = i;
        if (xslot < 0) { xNext = GetTickCount() + 2000; return 0; }
    }
    if (xget(xslot, &st) != ERROR_SUCCESS) { xslot = -1; xNext = GetTickCount() + 2000; return 0; }
    const XINPUT_GAMEPAD *g = &st.Gamepad;
    WORD w = g->wButtons;
    unsigned b = (w & XINPUT_GAMEPAD_A ? PAD_A : 0) | (w & XINPUT_GAMEPAD_B ? PAD_B : 0) | (w & XINPUT_GAMEPAD_X ? PAD_X : 0) |
                 (w & XINPUT_GAMEPAD_Y ? PAD_Y : 0) | (w & XINPUT_GAMEPAD_LEFT_SHOULDER ? PAD_LB : 0) | (w & XINPUT_GAMEPAD_RIGHT_SHOULDER ? PAD_RB : 0) |
                 (w & XINPUT_GAMEPAD_BACK ? PAD_BACK : 0) | (w & XINPUT_GAMEPAD_START ? PAD_START : 0) | (w & XINPUT_GAMEPAD_DPAD_UP ? PAD_UP : 0) |
                 (w & XINPUT_GAMEPAD_DPAD_DOWN ? PAD_DOWN : 0) | (w & XINPUT_GAMEPAD_DPAD_LEFT ? PAD_LEFT : 0) | (w & XINPUT_GAMEPAD_DPAD_RIGHT ? PAD_RIGHT : 0);
    game_pad(b, stick(g->sThumbLX), -stick(g->sThumbLY), stick(g->sThumbRX), -stick(g->sThumbRY), g->bLeftTrigger / 255.f, g->bRightTrigger / 255.f);
    return 1;
}

/* a PlayStation pad seen as a plain joystick: square, cross, circle, triangle, L1, R1, L2, R2, share, options;
   the right stick on Z / R, the triggers also as buttons (L2, R2), the D-pad on the hat */
static int readJoystick(void)
{
    JOYINFOEX j;
    j.dwSize = sizeof j;
    j.dwFlags = JOY_RETURNALL;
    if (joyId < 0) {
        if (GetTickCount() < jNext) return 0;
        for (int i = 0; i < 16 && joyId < 0; i++) if (joyGetPosEx(i, &j) == JOYERR_NOERROR) joyId = i;
        if (joyId < 0) { jNext = GetTickCount() + 2000; return 0; }
    }
    if (joyGetPosEx(joyId, &j) != JOYERR_NOERROR) { joyId = -1; jNext = GetTickCount() + 2000; return 0; }
    DWORD k = j.dwButtons, pov = j.dwPOV;
    unsigned b = (k & 2 ? PAD_A : 0) | (k & 4 ? PAD_B : 0) | (k & 1 ? PAD_X : 0) | (k & 8 ? PAD_Y : 0) | (k & 16 ? PAD_LB : 0) |
                 (k & 32 ? PAD_RB : 0) | (k & 256 ? PAD_BACK : 0) | (k & 512 ? PAD_START : 0);
    if (pov != JOY_POVCENTERED) { /* hundredths of a degree, clockwise from up */
        if (pov > 27000 || pov < 9000) b |= PAD_UP;
        if (pov > 0 && pov < 18000) b |= PAD_RIGHT;
        if (pov > 9000 && pov < 27000) b |= PAD_DOWN;
        if (pov > 18000) b |= PAD_LEFT;
    }
    float ax = j.dwXpos / 32767.5f - 1, ay = j.dwYpos / 32767.5f - 1, rx = j.dwZpos / 32767.5f - 1, ry = j.dwRpos / 32767.5f - 1;
    game_pad(b, ax, ay, rx, ry, (k & 64) ? 1.f : 0, (k & 128) ? 1.f : 0);
    return 1;
}

void padPoll(void)
{
    static int had;
    int now = readXInput() || readJoystick(); /* an Xbox pad also shows up as a joystick: XInput wins */
    if (!now && had) game_pad(0, 0, 0, 0, 0, 0, 0); /* unplugged: let go of everything it was holding */
    had = now;
}
