/* linux_pad.c - Game controllers on Linux: the kernel's joystick devices (/dev/input/js0...), read once per frame
 * and handed to the game core (game_pad in game.h). The kernel's xpad and hid-playstation drivers give Xbox and
 * PlayStation pads the same layout: buttons A B X Y LB RB BACK START, axes left stick, LT, right stick, RT, D-pad. */
#include "linux.h"
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <linux/joystick.h>

static int fd = -1;
static unsigned btn;   /* button bits as the kernel numbers them */
static short ax[8];
static time_t next;    /* opening a missing device every frame is wasted work: look for one now and then */

static float stick(short v) { return v < 0 ? v / 32768.f : v / 32767.f; }

void padPoll(void)
{
    static int had;
    if (fd < 0) {
        if (time(NULL) < next) { if (had) { game_pad(0, 0, 0, 0, 0, 0, 0); had = 0; } return; }
        char dev[32];
        for (int i = 0; i < 4 && fd < 0; i++) {
            snprintf(dev, sizeof dev, "/dev/input/js%d", i);
            fd = open(dev, O_RDONLY | O_NONBLOCK);
        }
        if (fd < 0) { next = time(NULL) + 2; return; }
        btn = 0;
        memset(ax, 0, sizeof ax);
        ax[2] = ax[5] = -32767; /* triggers rest at the bottom of their range */
    }
    struct js_event e;
    ssize_t r;
    while ((r = read(fd, &e, sizeof e)) == (ssize_t)sizeof e) {
        int n = e.number;
        if (e.type & JS_EVENT_BUTTON) { if (n < 32) btn = e.value ? btn | 1u << n : btn & ~(1u << n); }
        else if ((e.type & JS_EVENT_AXIS) && n < 8) ax[n] = e.value;
    }
    if (r < 0 && errno != EAGAIN) { close(fd); fd = -1; next = time(NULL) + 2; if (had) game_pad(0, 0, 0, 0, 0, 0, 0); had = 0; return; }
    unsigned b = (btn & 1 ? PAD_A : 0) | (btn & 2 ? PAD_B : 0) | (btn & 4 ? PAD_X : 0) | (btn & 8 ? PAD_Y : 0) |
                 (btn & 16 ? PAD_LB : 0) | (btn & 32 ? PAD_RB : 0) | (btn & 64 ? PAD_BACK : 0) | (btn & 128 ? PAD_START : 0);
    if (ax[7] < -16000) b |= PAD_UP;
    if (ax[7] > 16000) b |= PAD_DOWN;
    if (ax[6] < -16000) b |= PAD_LEFT;
    if (ax[6] > 16000) b |= PAD_RIGHT;
    game_pad(b, stick(ax[0]), stick(ax[1]), stick(ax[3]), stick(ax[4]), (ax[2] + 32767) / 65534.f, (ax[5] + 32767) / 65534.f);
    had = 1;
}
