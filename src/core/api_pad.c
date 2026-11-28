/* api_pad.c - Game controllers: the platform hands over the pad each frame, this turns it into keys, aim and fire.
 *
 * Left stick / D-pad move (and walk the menus, repeating while held), A jumps / picks, B dashes / goes back,
 * X reloads, Y and the bumpers switch weapons (and flip menu pages), START pauses, the right stick aims,
 * the right trigger fires and the left trigger dashes. */
#include "core.h"

int padSeen;  /* a controller has been used: the KEYS screen shows its buttons */
int padAim;   /* the right stick is aiming (the mouse takes over again when it moves) */
static unsigned padPrev;
static int padKey[12];          /* the key each button sent when it went down, so its release matches */
static int dirOn[4], ltOn, rtOn;
static float dirNext[4], aimA;

/* what a button means right now: some do one thing in a run and another in the menus */
static int buttonKey(unsigned bit)
{
    int play = state == ST_PLAY;
    switch (bit) {
    case PAD_A: return K_JUMP;
    case PAD_B: return play ? K_DASH : K_ESC;
    case PAD_X: return K_RETRY;
    case PAD_Y: return play ? K_NEXT_WEAPON : -1;
    case PAD_LB: return K_PREV_WEAPON;
    case PAD_RB: return K_NEXT_WEAPON;
    case PAD_START: return play || state == ST_PAUSE ? K_PAUSE : K_ENTER;
    case PAD_BACK: return play ? K_PAUSE : K_MENU;
    }
    return -1;
}

void game_pad(unsigned b, float lx, float ly, float rx, float ry, float lt, float rt)
{
    static const int dirKey[4] = { K_LEFT, K_RIGHT, K_UP, K_DOWN };
    static const unsigned dirBit[4] = { PAD_LEFT, PAD_RIGHT, PAD_UP, PAD_DOWN };
    int play = state == ST_PLAY, used = b != 0 || lt > 0.5f || rt > 0.35f || rx * rx + ry * ry > 0.09f || lx * lx + ly * ly > 0.25f;
    float vert = play ? 0.7f : 0.5f; /* in a run, pushing the stick a little up or down shouldn't jump or drop */
    if (used) {
        padSeen = 1;
        if (play && !achDone(ACH_PAD)) achGive(ACH_PAD);
    }
    for (int i = 0; i < 12; i++) { /* the buttons */
        unsigned bit = 1u << i;
        if ((bit & (PAD_UP | PAD_DOWN | PAD_LEFT | PAD_RIGHT)) || (b & bit) == (padPrev & bit)) continue;
        if (b & bit) { padKey[i] = buttonKey(bit); if (padKey[i] >= 0) game_key(padKey[i], 1); }
        else if (padKey[i] >= 0) { game_key(padKey[i], 0); padKey[i] = -1; }
    }
    padPrev = b;
    for (int i = 0; i < 4; i++) { /* the D-pad and the left stick, as arrow keys */
        float v = i == 0 ? -lx : i == 1 ? lx : i == 2 ? -ly : ly;
        int on = (b & dirBit[i]) != 0 || v > (i < 2 ? 0.5f : vert);
        if (on != dirOn[i]) { dirOn[i] = on; dirNext[i] = gt + 0.4f; game_key(dirKey[i], on); }
        else if (on && !play && gt >= dirNext[i]) { dirNext[i] = gt + 0.12f; game_key(dirKey[i], 0); game_key(dirKey[i], 1); } /* held in a menu: repeat */
    }
    if ((lt > 0.5f) != ltOn) { ltOn = !ltOn; if (ltOn && play) game_key(K_DASH, 1); }
    if ((rt > 0.35f) != rtOn) { rtOn = !rtOn; if (play || !rtOn) mdown = rtOn && play; }
    if (rx * rx + ry * ry > 0.09f) { aimA = atan2f(ry, rx); padAim = 1; }
    else if (rtOn && !padAim) { aimA = facing > 0 ? 0 : PI; padAim = 1; } /* firing without aiming: straight ahead */
    if (padAim && state == ST_PLAY) { mx = (int)(px + cosf(aimA) * 70); my = (int)(py - 3 + sinf(aimA) * 70); }
}
