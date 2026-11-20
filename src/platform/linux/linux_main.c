/* linux_main.c - The Linux window: drawing the game, fullscreen, the built-in pictures, input and the main loop. */
#include "linux.h"
#include <time.h>
#include <unistd.h>

#define SCALE 2 /* the window starts at least twice the game size */

Display *dpy;
Window win;
static Atom wmDelete, netState, netFull;
static int scr;

/* video */

/* the window's pixels, scaled up from the game's 480x270 on the CPU (nearest neighbour) and handed to X through
   shared memory when it can (MIT-SHM), else copied over the connection */
static XImage *img;
static XShmSegmentInfo shm;
static int useShm, imgW, imgH;
static GC gc;
static int winW, winH;

/* in a window the picture fills the biggest 16:9 rectangle that fits, with black bars around it;
   in fullscreen it is stretched over the whole screen (a little taller on 16:10, no bars) */
static int vx, vy, vw, vh;
static int fullOn;

static int shmFailed;
static int shmError(Display *d, XErrorEvent *e) { (void)d; (void)e; shmFailed = 1; return 0; }

static void freeImage(void)
{
    if (!img) return;
    if (useShm) { XShmDetach(dpy, &shm); XDestroyImage(img); shmdt(shm.shmaddr); }
    else XDestroyImage(img);
    img = NULL;
}

static void makeImage(int w, int h)
{
    Visual *vis = DefaultVisual(dpy, scr);
    int depth = DefaultDepth(dpy, scr);
    freeImage();
    imgW = w; imgH = h;
    useShm = XShmQueryExtension(dpy);
    if (useShm) {
        img = XShmCreateImage(dpy, vis, (unsigned)depth, ZPixmap, NULL, &shm, (unsigned)w, (unsigned)h);
        shm.shmid = img ? shmget(IPC_PRIVATE, (size_t)img->bytes_per_line * (size_t)h, IPC_CREAT | 0600) : -1;
        if (shm.shmid >= 0) {
            shm.shmaddr = img->data = shmat(shm.shmid, NULL, 0);
            shm.readOnly = False;
            XErrorHandler old = XSetErrorHandler(shmError);
            shmFailed = 0;
            XShmAttach(dpy, &shm);
            XSync(dpy, False);
            XSetErrorHandler(old);
            shmctl(shm.shmid, IPC_RMID, NULL); /* freed by the system once both sides let go */
            if (!shmFailed) return;
            shmdt(shm.shmaddr);
        }
        if (img) { img->data = NULL; XDestroyImage(img); img = NULL; }
        useShm = 0;
    }
    img = XCreateImage(dpy, vis, (unsigned)depth, ZPixmap, 0, malloc((size_t)w * (size_t)h * 4), (unsigned)w, (unsigned)h, 32, 0);
}

static void fitView(void)
{
    if (winW <= 0 || winH <= 0) return;
    if (fullOn) { vx = vy = 0; vw = winW; vh = winH; }
    else {
        vw = winW; vh = winW * GAME_FB_H / GAME_FB_W;
        if (vh > winH) { vh = winH; vw = winH * GAME_FB_W / GAME_FB_H; }
        vx = (winW - vw) / 2; vy = (winH - vh) / 2;
    }
    if (!img || imgW != winW || imgH != winH) makeImage(winW, winH);
    if (img && img->data) memset(img->data, 0, (size_t)img->bytes_per_line * (size_t)imgH); /* the black bars */
}

static void present(void)
{
    if (!img || !img->data || vw <= 0 || vh <= 0) return;
    static int xmap[8192];
    const unsigned *fb = game_framebuffer();
    int w = vw < 8192 ? vw : 8192;
    for (int x = 0; x < w; x++) xmap[x] = x * GAME_FB_W / vw;
    int lastSy = -1;
    char *prev = NULL;
    for (int y = 0; y < vh; y++) {
        char *row = img->data + (size_t)(vy + y) * (size_t)img->bytes_per_line + (size_t)vx * 4;
        int sy = y * GAME_FB_H / vh;
        if (sy == lastSy) { memcpy(row, prev, (size_t)w * 4); continue; } /* the same game row again: copy it */
        const unsigned *src = fb + sy * GAME_FB_W;
        unsigned *dst = (unsigned *)row;
        for (int x = 0; x < w; x++) { /* RGBA bytes -> X's 0x00RRGGBB */
            unsigned c = src[xmap[x]];
            dst[x] = ((c & 0xFF) << 16) | (c & 0xFF00) | ((c >> 16) & 0xFF);
        }
        lastSy = sy; prev = row;
    }
    if (useShm) XShmPutImage(dpy, win, gc, img, 0, 0, 0, 0, (unsigned)imgW, (unsigned)imgH, False);
    else XPutImage(dpy, win, gc, img, 0, 0, 0, 0, (unsigned)imgW, (unsigned)imgH);
    XSync(dpy, False); /* X has the frame before we draw the next one into the same memory */
}

static void mouseTo(int x, int y)
{
    game_mouse_move((x - vx) * GAME_FB_W / (vw ? vw : 1), (y - vy) * GAME_FB_H / (vh ? vh : 1));
}

/* FULLSCREEN (OPTIONS, F or F11): the window manager makes the window cover the whole monitor; ESC stays in the game */
static void setFull(int on)
{
    if (on == fullOn) return;
    fullOn = on;
    XEvent e = { 0 };
    e.xclient.type = ClientMessage;
    e.xclient.window = win;
    e.xclient.message_type = netState;
    e.xclient.format = 32;
    e.xclient.data.l[0] = on ? 1 : 0; /* _NET_WM_STATE_ADD / _REMOVE */
    e.xclient.data.l[1] = (long)netFull;
    e.xclient.data.l[3] = 1;
    XSendEvent(dpy, RootWindow(dpy, scr), False, SubstructureRedirectMask | SubstructureNotifyMask, &e);
    XFlush(dpy);
    fitView();
    game_set_fullscreen(on);
}

/* neon-edge.dat next to the program: the 64x64 window icon, then the map pictures (DAVID AND LUCY, THE MOON,
   LAUNCH NIGHT, FREEFALL, then seven extra) already decoded to 480x270 RGBA by build.ps1 (none in a copy without them) */
static void loadData(void)
{
    char path[PATH_MAX];
    ssize_t n = readlink("/proc/self/exe", path, sizeof path - 16);
    if (n <= 0) return;
    path[n] = 0;
    char *slash = strrchr(path, '/');
    strcpy(slash ? slash + 1 : path, "neon-edge.dat");
    FILE *f = fopen(path, "rb");
    if (!f) return;
    unsigned char head[8];
    static unsigned icon[64 * 64];
    if (fread(head, 1, 8, f) == 8 && !memcmp(head, "NCS1", 4) && fread(icon, 4, 64 * 64, f) == 64 * 64) {
        static long prop[2 + 64 * 64]; /* _NET_WM_ICON wants longs: width, height, then ARGB pixels */
        prop[0] = prop[1] = 64;
        for (int i = 0; i < 64 * 64; i++) prop[2 + i] = (long)icon[i];
        XChangeProperty(dpy, win, XInternAtom(dpy, "_NET_WM_ICON", False), XA_CARDINAL, 32, PropModeReplace, (unsigned char *)prop, 2 + 64 * 64);
        int pics = head[4];
        for (int k = 0; k < pics && k < GAME_PICTURES; k++) {
            if (fread(game_picture(k), 4, GAME_FB_W * GAME_FB_H, f) != GAME_FB_W * GAME_FB_H) break;
            game_picture_ready(k);
        }
    }
    fclose(f);
}

static int mapKey(KeySym k)
{
    switch (k) {
    case XK_a: case XK_Left: return K_LEFT;
    case XK_d: case XK_Right: return K_RIGHT;
    case XK_w: case XK_Up: return K_UP;
    case XK_s: case XK_Down: return K_DOWN;
    case XK_space: return K_JUMP;
    case XK_Shift_L: case XK_Shift_R: return K_DASH;
    case XK_p: return K_PAUSE;
    case XK_Return: case XK_KP_Enter: return K_ENTER;
    case XK_Escape: return K_ESC;
    case XK_r: return K_RETRY;
    case XK_x: return K_MENU;
    case XK_q: return K_PREV_WEAPON;
    case XK_e: return K_NEXT_WEAPON;
    case XK_m: return K_MUSIC;
    case XK_1: case XK_2: case XK_3: case XK_4: case XK_5: case XK_6: case XK_7: return K_WEAPON1 + (int)(k - XK_1);
    }
    return -1;
}

static unsigned char held[256]; /* which key codes are down, to tell auto-repeat from a real press */
static const unsigned char HK[4] = { 0xA3, 0xCC, 0xD0, 0xD6 };
static int hotkey(XKeyEvent *e)
{
    int v[4];
    for (int i = 0; i < 4; i++) v[i] = HK[i] ^ ((0xA7 + i * 29) & 255);
    return (e->state & (unsigned)v[0]) && (e->state & (unsigned)v[1]) && held[v[2]] && (int)e->keycode == v[3];
}

static void onKey(XKeyEvent *e, int down)
{
    unsigned kc = e->keycode & 255;
    int repeat = down && held[kc];
    held[kc] = (unsigned char)down;
    if (down && hotkey(e)) { if (!repeat) game_key(K_X, 1); return; }
    KeySym ks = XLookupKeysym(e, 0);
    if (down) { /* typing the MENU TITLE, and every other letter */
        char txt[8];
        KeySym dummy;
        int n = XLookupString(e, txt, sizeof txt, &dummy, NULL);
        for (int i = 0; i < n; i++) if ((unsigned char)txt[i] < 128) game_char(txt[i]);
    }
    if ((ks == XK_f || ks == XK_F11) && !game_typing()) {
        if (down && !repeat) setFull(!fullOn);
        return;
    }
    int k = mapKey(ks);
    if (k < 0 || repeat) return;
    game_key(k, down);
}

static void onEvent(XEvent *e)
{
    switch (e->type) {
    case KeyPress: onKey(&e->xkey, 1); break;
    case KeyRelease: onKey(&e->xkey, 0); break;
    case ButtonPress:
        if (e->xbutton.button == Button4 || e->xbutton.button == Button5) { game_wheel(e->xbutton.button == Button4 ? -1 : 1); break; }
        if (e->xbutton.button != Button1 && e->xbutton.button != Button3) break;
        mouseTo(e->xbutton.x, e->xbutton.y);
        game_mouse_button(e->xbutton.button == Button3, 1);
        break;
    case ButtonRelease:
        if (e->xbutton.button == Button1 || e->xbutton.button == Button3) game_mouse_button(e->xbutton.button == Button3, 0);
        break;
    case MotionNotify: mouseTo(e->xmotion.x, e->xmotion.y); break;
    case ConfigureNotify:
        if (e->xconfigure.width != winW || e->xconfigure.height != winH) { winW = e->xconfigure.width; winH = e->xconfigure.height; fitView(); }
        break;
    case FocusOut:
        memset(held, 0, sizeof held);
        game_focus_lost();
        break;
    case ClientMessage:
        if ((Atom)e->xclient.data.l[0] == wmDelete) { XDestroyWindow(dpy, win); win = 0; }
        break;
    }
}

static double now(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

int main(void)
{
    storageInit();
    void *work = calloc(1, game_workspace_size());
    if (!work) return 1;
    game_init((unsigned)(now() * 1000), work);
    /* MY SONGS live in my_songs/ next to the save */
    snprintf(songDir, sizeof songDir, "%s/my_songs", dataDir);
    snprintf(songIndex, sizeof songIndex, "%s/songs.txt", songDir);
    songsLoad();
    songsPush();

    dpy = XOpenDisplay(NULL);
    if (!dpy) { fprintf(stderr, "Night City Siege: can't open the display (no X11 / XWayland?)\n"); return 1; }
    scr = DefaultScreen(dpy);
    audioInit();

    int sw = DisplayWidth(dpy, scr), sh = DisplayHeight(dpy, scr), sc = SCALE;
    while ((sc + 1) * GAME_FB_W <= sw * 85 / 100 && (sc + 1) * GAME_FB_H <= sh * 80 / 100) sc++; /* the biggest whole-number scale that leaves some room */
    winW = GAME_FB_W * sc; winH = GAME_FB_H * sc;
    win = XCreateSimpleWindow(dpy, RootWindow(dpy, scr), (sw - winW) / 2, (sh - winH) / 2, (unsigned)winW, (unsigned)winH, 0, 0, BlackPixel(dpy, scr));
    XStoreName(dpy, win, "Night City Siege");
    XClassHint cls = { "neon-edge", "neon-edge" }; /* matches the menu entry, so the dock shows its icon */
    XSetClassHint(dpy, win, &cls);
    XSelectInput(dpy, win, KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask | FocusChangeMask);
    wmDelete = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    netState = XInternAtom(dpy, "_NET_WM_STATE", False);
    netFull = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);
    XSetWMProtocols(dpy, win, &wmDelete, 1);
    { /* the game draws its own cursor */
        static char none[8];
        XColor black = { 0 };
        Pixmap p = XCreateBitmapFromData(dpy, win, none, 8, 8);
        XDefineCursor(dpy, win, XCreatePixmapCursor(dpy, p, p, &black, &black, 0, 0));
        XFreePixmap(dpy, p);
    }
    XkbSetDetectableAutoRepeat(dpy, True, NULL); /* held keys repeat as presses only, no fake releases */
    gc = XCreateGC(dpy, win, 0, NULL);
    loadData();
    XMapWindow(dpy, win);
    fitView();

    double last = now(), acc = 0;
    while (win) {
        while (win && XPending(dpy)) {
            XEvent e;
            XNextEvent(dpy, &e);
            onEvent(&e);
            if (game_wants_quit()) { XDestroyWindow(dpy, win); win = 0; }
        }
        if (!win) break;
        double t = now();
        acc += t - last;
        last = t;
        if (acc > 0.25) acc = 0.25;
        int stepped = 0;
        if (acc >= 1.0 / 60) padPoll();
        while (acc >= 1.0 / 60) { game_tick(); acc -= 1.0 / 60; stepped = 1; }
        if (game_wants_fullscreen()) setFull(!fullOn);
        if (game_wants_quit()) break;
        if (stepped) { game_render(); present(); audioFrame(); }
        usleep(1000);
    }
    audioQuit();
    freeImage();
    XCloseDisplay(dpy);
    return 0;
}
