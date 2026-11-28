/* win32_main.c - The Windows window: drawing the game, fullscreen, the built-in pictures, input and the main loop. */
#include "win32.h"

#define SCALE 2 /* the window starts at least twice the game size */
#define WW (GAME_FB_W * SCALE)
#define WH (GAME_FB_H * SCALE)

HWND hwnd;
static unsigned bgra[GAME_FB_W * GAME_FB_H]; /* the frame in the byte order GDI wants */

/* video */

/* in a window the picture fills the biggest 16:9 rectangle that fits, with black bars around it;
   in fullscreen it is stretched over the whole screen (a little taller on 16:10, no bars) */
static int vx, vy, vw = WW, vh = WH;
static int fullOn;

static void fitView(void)
{
    RECT rc;
    GetClientRect(hwnd, &rc);
    int cw = rc.right, ch = rc.bottom;
    if (cw <= 0 || ch <= 0) return;
    if (fullOn) { vx = vy = 0; vw = cw; vh = ch; return; }
    vw = cw; vh = cw * GAME_FB_H / GAME_FB_W;
    if (vh > ch) { vh = ch; vw = ch * GAME_FB_W / GAME_FB_H; }
    vx = (cw - vw) / 2; vy = (ch - vh) / 2;
}

static void present(HDC dc)
{
    const unsigned *fb = game_framebuffer();
    for (int i = 0; i < GAME_FB_W * GAME_FB_H; i++) { /* RGBA bytes -> BGRA */
        unsigned c = fb[i];
        bgra[i] = (c & 0xFF00FF00u) | ((c & 0xFF) << 16) | ((c >> 16) & 0xFF);
    }
    BITMAPINFO bi = { 0 };
    bi.bmiHeader.biSize = sizeof bi.bmiHeader;
    bi.bmiHeader.biWidth = GAME_FB_W;
    bi.bmiHeader.biHeight = -GAME_FB_H; /* top-down */
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    RECT rc;
    GetClientRect(hwnd, &rc);
    if (vx > 0) { PatBlt(dc, 0, 0, vx, rc.bottom, BLACKNESS); PatBlt(dc, vx + vw, 0, rc.right - vx - vw, rc.bottom, BLACKNESS); }
    if (vy > 0) { PatBlt(dc, 0, 0, rc.right, vy, BLACKNESS); PatBlt(dc, 0, vy + vh, rc.right, rc.bottom - vy - vh, BLACKNESS); }
    SetStretchBltMode(dc, COLORONCOLOR);
    StretchDIBits(dc, vx, vy, vw, vh, 0, 0, GAME_FB_W, GAME_FB_H, bgra, &bi, DIB_RGB_COLORS, SRCCOPY);
}

static void mouseTo(LPARAM lp)
{
    game_mouse_move(((short)LOWORD(lp) - vx) * GAME_FB_W / (vw ? vw : 1), ((short)HIWORD(lp) - vy) * GAME_FB_H / (vh ? vh : 1));
}

/* FULLSCREEN (OPTIONS, F or F11): a borderless window over the whole monitor; ESC stays in the game */
static WINDOWPLACEMENT oldPlace = { .length = sizeof oldPlace };
#define WIN_STYLE (WS_OVERLAPPEDWINDOW | WS_VISIBLE)

static void setFull(int on)
{
    if (on == fullOn) return;
    fullOn = on;
    if (on) {
        MONITORINFO mi = { .cbSize = sizeof mi };
        GetWindowPlacement(hwnd, &oldPlace);
        GetMonitorInfoA(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &mi);
        SetWindowLongA(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
        SetWindowPos(hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left,
                     mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
    } else {
        SetWindowLongA(hwnd, GWL_STYLE, WIN_STYLE);
        SetWindowPlacement(hwnd, &oldPlace);
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
    fitView();
    InvalidateRect(hwnd, NULL, FALSE);
    game_set_fullscreen(on);
}

/* the map pictures (DAVID AND LUCY, THE MOON, LAUNCH NIGHT, FREEFALL, then seven extra): JPEGs built into the exe (game.rc, ids 2-12), decoded with WIC */
static void loadPicture(HINSTANCE inst, int k)
{
    HRSRC r = FindResourceA(inst, MAKEINTRESOURCEA(2 + k), MAKEINTRESOURCEA(10) /* RT_RCDATA */);
    HGLOBAL g = r ? LoadResource(inst, r) : NULL;
    if (!g) return;
    IWICImagingFactory *wic = NULL;
    IWICStream *st = NULL;
    IWICBitmapDecoder *dec = NULL;
    IWICBitmapFrameDecode *fr = NULL;
    IWICBitmapScaler *sc = NULL;
    IWICFormatConverter *cv = NULL;
    if (SUCCEEDED(CoCreateInstance(&CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, &IID_IWICImagingFactory, (void **)&wic)) &&
        SUCCEEDED(IWICImagingFactory_CreateStream(wic, &st)) &&
        SUCCEEDED(IWICStream_InitializeFromMemory(st, (BYTE *)LockResource(g), SizeofResource(inst, r))) &&
        SUCCEEDED(IWICImagingFactory_CreateDecoderFromStream(wic, (IStream *)st, NULL, WICDecodeMetadataCacheOnDemand, &dec)) &&
        SUCCEEDED(IWICBitmapDecoder_GetFrame(dec, 0, &fr)) &&
        SUCCEEDED(IWICImagingFactory_CreateBitmapScaler(wic, &sc)) &&
        SUCCEEDED(IWICBitmapScaler_Initialize(sc, (IWICBitmapSource *)fr, GAME_FB_W, GAME_FB_H, WICBitmapInterpolationModeFant)) &&
        SUCCEEDED(IWICImagingFactory_CreateFormatConverter(wic, &cv)) &&
        SUCCEEDED(IWICFormatConverter_Initialize(cv, (IWICBitmapSource *)sc, &GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, NULL, 0, WICBitmapPaletteTypeCustom)) &&
        SUCCEEDED(IWICFormatConverter_CopyPixels(cv, NULL, GAME_FB_W * 4, GAME_FB_W * GAME_FB_H * 4, (BYTE *)game_picture(k))))
        game_picture_ready(k);
    if (cv) IWICFormatConverter_Release(cv);
    if (sc) IWICBitmapScaler_Release(sc);
    if (fr) IWICBitmapFrameDecode_Release(fr);
    if (dec) IWICBitmapDecoder_Release(dec);
    if (st) IWICStream_Release(st);
    if (wic) IWICImagingFactory_Release(wic);
}

static int mapKey(WPARAM vk)
{
    switch (vk) {
    case 'A': case VK_LEFT: return K_LEFT;
    case 'D': case VK_RIGHT: return K_RIGHT;
    case 'W': case VK_UP: return K_UP;
    case 'S': case VK_DOWN: return K_DOWN;
    case VK_SPACE: return K_JUMP;
    case VK_SHIFT: return K_DASH;
    case 'P': return K_PAUSE;
    case VK_RETURN: return K_ENTER;
    case VK_ESCAPE: return K_ESC;
    case 'R': return K_RETRY;
    case 'X': return K_MENU;
    case 'Q': return K_PREV_WEAPON;
    case 'E': return K_NEXT_WEAPON;
    case 'M': return K_MUSIC;
    case '1': case '2': case '3': case '4': case '5': case '6': case '7': return K_WEAPON1 + (int)(vk - '1');
    }
    return -1;
}

/* the developer hotkey: the game knows the combination, this only tells it which keys are down */
static unsigned char hkDown[512]; /* scan codes, without CTRL / ALT / SHIFT / WIN */
static int hotkey(UINT msg, WPARAM wp, LPARAM lp)
{
    int sc = (int)((lp >> 16) & 0x1FF), down = msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN, h[2] = { 0, 0 }, n = 0, hit = 0;
    if (wp == VK_SHIFT || wp == VK_CONTROL || wp == VK_MENU || wp == VK_LWIN || wp == VK_RWIN) return 0;
    if (down) {
        int mods = (GetKeyState(VK_CONTROL) < 0) | (GetKeyState(VK_MENU) < 0) << 1 | (GetKeyState(VK_SHIFT) < 0) << 2;
        for (int i = 0; i < 512; i++)
            if (hkDown[i] && i != sc) { if (n < 2) h[n] = i; n++; }
        hit = n <= 2 && game_hotkey(1, mods, h[0], h[1], sc);
    }
    hkDown[sc] = (unsigned char)down;
    return hit;
}

static LRESULT CALLBACK WndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
    case WM_SYSKEYDOWN: /* with ALT held, keys can come as system keys */
    case WM_SYSKEYUP:
        if (hotkey(msg, wp, lp)) { if (!(lp & (1 << 30))) game_key(K_X, 1); return 0; }
        break;
    case WM_KEYDOWN:
    case WM_KEYUP: {
        if (hotkey(msg, wp, lp)) { if (!(lp & (1 << 30))) game_key(K_X, 1); return 0; }
        int k = mapKey(wp), down = msg == WM_KEYDOWN;
        if ((wp == 'F' || wp == VK_F11) && !game_typing()) {
            if (down && !(lp & (1 << 30))) setFull(!fullOn);
            return 0;
        }
        if (k < 0) break;
        if (down && (lp & (1 << 30))) return 0; /* ignore auto-repeat */
        game_key(k, down);
        if (game_wants_quit()) DestroyWindow(h);
        return 0;
    }
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
        SetCapture(h);
        mouseTo(lp);
        game_mouse_button(msg == WM_RBUTTONDOWN, 1);
        if (game_wants_quit()) DestroyWindow(h);
        return 0;
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
        ReleaseCapture();
        game_mouse_button(msg == WM_RBUTTONUP, 0);
        return 0;
    case WM_MOUSEMOVE:
        mouseTo(lp);
        return 0;
    case WM_SIZE:
        fitView();
        InvalidateRect(h, NULL, FALSE);
        return 0;
    case WM_ERASEBKGND:
        return 1; /* present() paints every pixel */
    case WM_CHAR: /* typing the MENU TITLE, and every other letter */
        game_char((int)wp);
        return 0;
    case WM_MOUSEWHEEL:
        game_wheel(GET_WHEEL_DELTA_WPARAM(wp) > 0 ? -1 : 1);
        return 0;
    case WM_KILLFOCUS:
        ZeroMemory(hkDown, sizeof hkDown);
        game_focus_lost();
        return 0;
    case WM_SETCURSOR:
        if (LOWORD(lp) == HTCLIENT) { SetCursor(NULL); return TRUE; }
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(h, &ps);
        present(dc);
        EndPaint(h, &ps);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(h, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE prev, LPSTR cmd, int show)
{
    (void)prev; (void)cmd; (void)show;
    /* the save and MY SONGS live in %LOCALAPPDATA%\NeonEdge (the usual place for app data), so a copy of
       the exe on the desktop never drops files next to itself; without LOCALAPPDATA they go next to the exe */
    char *slash;
    DWORD n = GetEnvironmentVariableA("LOCALAPPDATA", savePath, MAX_PATH - 32);
    if (n && n < MAX_PATH - 32) {
        strcat(savePath, "\\NeonEdge");
        CreateDirectoryA(savePath, NULL);
        strcat(savePath, "\\");
    } else GetModuleFileNameA(NULL, savePath, MAX_PATH);
    slash = strrchr(savePath, '\\');
    strcpy(slash ? slash + 1 : savePath, "slop_save.txt");

    void *work = calloc(1, game_workspace_size());
    if (!work) return 1;
    game_init(GetTickCount(), work);
    /* MY SONGS live in my_songs\ next to the save */
    snprintf(songDir, sizeof songDir, "%.*s", (int)(slash ? slash - savePath : 0), savePath);
    strcat(songDir, slash ? "\\my_songs" : "my_songs");
    CreateDirectoryA(songDir, NULL);
    snprintf(songIndex, sizeof songIndex, "%s\\songs.txt", songDir);
    songsLoad();
    songsPush();
    timeBeginPeriod(1);
    audioInit();

    WNDCLASSA wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "SlopShooter";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(1)); /* game.rc */
    if (!wc.hIcon) wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassA(&wc);
    SetProcessDPIAware(); /* real screen pixels, so Windows' display scaling doesn't blur the pixel art */
    RECT wa;
    SystemParametersInfoA(SPI_GETWORKAREA, 0, &wa, 0);
    int sc = SCALE; /* the biggest whole-number scale that leaves some room on the screen */
    while ((sc + 1) * GAME_FB_W <= (wa.right - wa.left) * 85 / 100 && (sc + 1) * GAME_FB_H <= (wa.bottom - wa.top) * 85 / 100) sc++;
    RECT r = { 0, 0, GAME_FB_W * sc, GAME_FB_H * sc };
    AdjustWindowRect(&r, WIN_STYLE, FALSE);
    int ww = r.right - r.left, wh = r.bottom - r.top;
    hwnd = CreateWindowA("SlopShooter", "Night City Siege", WIN_STYLE,
                         (GetSystemMetrics(SM_CXSCREEN) - ww) / 2, (GetSystemMetrics(SM_CYSCREEN) - wh) / 2,
                         ww, wh, NULL, NULL, hInst, NULL);
    ShowWindow(hwnd, SW_SHOW);
    fitView();
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    for (int k = 0; k < GAME_PICTURES; k++) loadPicture(hInst, k);

    LARGE_INTEGER freq, last, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&last);
    double acc = 0;
    MSG msg;
    for (;;) {
        while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) goto done;
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
        QueryPerformanceCounter(&now);
        acc += (double)(now.QuadPart - last.QuadPart) / freq.QuadPart;
        last = now;
        if (acc > 0.25) acc = 0.25;
        int stepped = 0;
        if (acc >= 1.0 / 60) padPoll();
        while (acc >= 1.0 / 60) { game_tick(); acc -= 1.0 / 60; stepped = 1; }
        if (game_wants_fullscreen()) setFull(!fullOn);
        if (game_wants_quit()) DestroyWindow(hwnd);
        if (stepped) {
            game_render();
            HDC dc = GetDC(hwnd);
            present(dc);
            ReleaseDC(hwnd, dc);
        }
        audioPump();
        Sleep(1);
    }
done:
    if (audioOk) { waveOutReset(hwo); waveOutClose(hwo); }
    timeEndPeriod(1);
    return 0;
}
