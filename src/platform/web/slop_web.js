// NIGHT CITY SIEGE - browser platform layer. The game core (src/core compiled to
// WebAssembly) draws a 480x270 pixel buffer; this file shows it on a canvas
// with crisp nearest-neighbour scaling, forwards input, plays sounds and music
// and keeps the save and MY SONGS in the browser's storage.
async function slopStart(canvas, wasmBytes, opts = {}) {
  const W = 480, H = 270;
  canvas.width = W;
  canvas.height = H;
  canvas.style.imageRendering = 'pixelated';
  const ctx = canvas.getContext('2d');
  let mem = null, ex = null, audio = null, musicGain = null, musicVol = 0;
  const sbufs = [];
  const u8 = () => new Uint8Array(mem.buffer);

  const env = {
    plat_sound(id, vol) {
      if (!audio || !sbufs[id] || audio.state !== 'running') return;
      const src = audio.createBufferSource(), gain = audio.createGain();
      src.buffer = sbufs[id]; gain.gain.value = vol * 0.5;
      src.connect(gain).connect(audio.destination); src.start();
    },
    // the soundtrack loops for as long as the page is open; the game only fades it up and down
    plat_music(vol) {
      musicVol = vol;
      if (musicGain) musicGain.gain.setTargetAtTime(vol * 0.5, audio.currentTime, 0.25);
    },
    // local calendar day and time of day, for the daily quests
    plat_day() { const d = new Date(); return Math.floor((d.getTime() - d.getTimezoneOffset() * 60000) / 86400000); },
    plat_daysec() { const d = new Date(); return d.getHours() * 3600 + d.getMinutes() * 60 + d.getSeconds(); },
    // progress (coins, unlocks, quests) is a short text line kept in localStorage
    plat_load_data(p, cap) {
      let s = '';
      try { s = localStorage.getItem('slop_save_v1') || ''; } catch (e) { /* storage blocked */ }
      const b = u8(), n = Math.min(s.length, cap);
      for (let i = 0; i < n; i++) b[p + i] = s.charCodeAt(i) & 255;
      return n;
    },
    plat_save_data(p) {
      const b = u8(); let s = '';
      for (let i = p; b[i]; i++) s += String.fromCharCode(b[i]);
      try { localStorage.setItem('slop_save_v1', s); } catch (e) { /* storage blocked */ }
    },
    // MY SONGS (see below): pick files, forget one, tie one to a district
    plat_song_add() { return songAdd(); },
    plat_song_remove(i) { songRemove(i); },
    plat_song_map(i, m) { songMap(i, m); },
    // a link in the game (the credits): open it in a new tab
    plat_open_url(p) {
      const b = u8(); let s = '';
      for (let i = p; b[i]; i++) s += String.fromCharCode(b[i]);
      try { window.open(s, '_blank', 'noopener'); } catch (e) { /* popups blocked */ }
    },
    memset(p, v, n) { u8().fill(v, p, p + n); return p; },
    memcpy(d, s, n) { u8().copyWithin(d, s, s + n); return d; },
    memmove(d, s, n) { u8().copyWithin(d, s, s + n); return d; },
    strlen(p) { const b = u8(); let i = p; while (b[i]) i++; return i - p; },
    memcmp(a, c, n) { const b = u8(); for (let i = 0; i < n; i++) { const d = b[a + i] - b[c + i]; if (d) return d; } return 0; },
    strncmp(a, c, n) { const b = u8(); for (let i = 0; i < n; i++) { const d = b[a + i] - b[c + i]; if (d || !b[a + i]) return d; } return 0; },
    fmodf: (a, b) => a % b,
    fminf: Math.min, fmaxf: Math.max, fabsf: Math.abs, exp2f: x => Math.pow(2, x),
    sincosf(x, sp, cp) { const f = new Float32Array(mem.buffer); f[sp >> 2] = Math.sin(x); f[cp >> 2] = Math.cos(x); },
  };
  // any other libm import (sinf, cosf, atan2f, powf, floorf, roundf, ...) maps onto Math
  const module = await WebAssembly.compile(wasmBytes);
  for (const imp of WebAssembly.Module.imports(module)) {
    if (imp.kind !== 'function' || env[imp.name]) continue;
    const m = Math[imp.name.replace(/f$/, '')];
    if (!m) throw new Error('missing import: ' + imp.name);
    env[imp.name] = m;
  }
  const instance = await WebAssembly.instantiate(module, { env });
  ex = instance.exports;
  mem = ex.memory;
  // the core's big pixel/sound buffers live in fresh pages past everything the module uses
  const work = mem.grow(Math.ceil(ex.game_workspace_size() / 65536)) * 65536;
  ex.game_init((Math.random() * 0xffffffff) >>> 0, work);
  // the map pictures (image URLs, in game_picture order: DAVID AND LUCY, THE MOON, LAUNCH NIGHT, FREEFALL, then seven extra),
  // squeezed to the game's resolution and handed to the core
  (opts.pictures || []).forEach((url, k) => {
    if (!url) return;
    const img = new Image();
    img.onload = () => {
      const c = document.createElement('canvas'); c.width = W; c.height = H;
      const g = c.getContext('2d'); g.drawImage(img, 0, 0, W, H);
      u8().set(g.getImageData(0, 0, W, H).data, ex.game_picture(k));
      ex.game_picture_ready(k);
    };
    img.src = url;
  });

  function present() {
    ex.game_render();
    const px = new Uint8ClampedArray(mem.buffer, ex.game_framebuffer(), W * H * 4);
    ctx.putImageData(new ImageData(px, W, H), 0, 0);
  }

  function initAudio() {
    if (audio) { if (audio.state !== 'running') audio.resume(); return; }
    try {
      audio = new (window.AudioContext || window.webkitAudioContext)();
      for (let i = 0; i < ex.game_sound_count(); i++) {
        const n = ex.game_sound_len(i), pcm = new Int16Array(mem.buffer, ex.game_sound_data(i), n);
        const buf = audio.createBuffer(1, n, 22050), ch = buf.getChannelData(0);
        for (let j = 0; j < n; j++) ch[j] = pcm[j] / 32768;
        sbufs[i] = buf;
      }
      musicGain = audio.createGain();
      musicGain.gain.value = 0;
      musicGain.gain.setTargetAtTime(musicVol * 0.5, audio.currentTime, 0.5);
      musicGain.connect(audio.destination);
      checkTrack();
    } catch (e) { audio = null; musicGain = null; }
  }

  // the soundtrack: one looping song at a time; when the game picks another one (new district, RANDOM, OPTIONS),
  // the old one fades out while the new one fades in. Songs are made by the game the first time they're asked for.
  const mbufs = [];
  let playingTrack = -1, playing = null;
  function checkTrack() {
    if (!audio || !musicGain) return;
    const t = ex.game_music_track();
    if (t === playingTrack) return;
    let buf, level = 1;
    if (t >= MY_FIRST) { // one of MY SONGS: decoded from the stored file the first time it plays
      const s = songs[t - MY_FIRST];
      if (!s) return;
      if (!s.buf) {
        if (!s.decoding && !s.bad) {
          s.decoding = true;
          audio.decodeAudioData(s.data.slice(0)).then(b => { s.buf = b; s.decoding = false; },
                                                       () => { s.bad = true; s.decoding = false; });
        }
        return; // keep the old song until this one is ready
      }
      buf = s.buf; level = 0.5; // finished songs are mastered much louder than the synth loops
    }
    playingTrack = t;
    if (buf) {
      // played below
    } else if (!mbufs[t]) {
      const p = ex.game_music_data(), n = ex.game_music_len(), pcm = new Int16Array(mem.buffer, p, n);
      const buf = audio.createBuffer(1, n, 22050), ch = buf.getChannelData(0);
      for (let j = 0; j < n; j++) ch[j] = pcm[j] / 32768;
      mbufs[t] = buf;
    }
    const now = audio.currentTime;
    if (playing) {
      const old = playing;
      old.gain.gain.setTargetAtTime(0, now, 0.25);
      old.src.stop(now + 1.5);
    }
    const src = audio.createBufferSource(), gain = audio.createGain();
    src.buffer = buf || mbufs[t]; src.loop = true;
    gain.gain.value = playing ? 0 : level;
    if (playing) gain.gain.setTargetAtTime(level, now, 0.3);
    src.connect(gain).connect(musicGain); src.start();
    playing = { src, gain };
  }

  // MY SONGS: the player's own music files, kept in IndexedDB in this browser on this computer only.
  // The game gets their names and districts; the files themselves never leave this browser.
  const MY_FIRST = 10, MY_MAX = 8, MY_NAME = 48;
  const songs = []; // { id, name, map, data (the file's bytes), buf (decoded, once played) }
  let songDb = null;
  const store = mode => songDb.transaction('songs', mode).objectStore('songs');
  const req = r => new Promise(res => { r.onsuccess = () => res(r.result); r.onerror = () => res(null); });
  function pushSongs() { // tell the game what's there
    const names = new Uint8Array(mem.buffer, ex.game_song_names(), MY_MAX * MY_NAME), maps = new Int8Array(mem.buffer, ex.game_song_maps(), MY_MAX);
    names.fill(0); maps.fill(-1);
    songs.forEach((s, i) => {
      for (let j = 0; j < Math.min(MY_NAME - 1, s.name.length); j++) names[i * MY_NAME + j] = s.name.charCodeAt(j);
      maps[i] = s.map;
    });
    ex.game_songs_set(songs.length);
    playingTrack = -1; // the numbering may have moved: start over with whatever the game picks
  }
  // a file name the pixel font can show: no extension, capitals, only letters it has
  const songName = f => f.replace(/\.[^.]*$/, '').toUpperCase().replace(/[^A-Z0-9 !?.,:'()+\-/]/g, ' ').replace(/\s+/g, ' ').trim().slice(0, MY_NAME - 1) || 'MY SONG';
  async function loadSongs() {
    try {
      songDb = await new Promise(res => {
        const r = indexedDB.open('slop_songs', 1);
        r.onupgradeneeded = () => r.result.createObjectStore('songs', { keyPath: 'id', autoIncrement: true });
        r.onsuccess = () => res(r.result); r.onerror = () => res(null);
      });
    } catch (e) { songDb = null; }
    if (!songDb) return;
    const all = (await req(store('readonly').getAll())) || [];
    for (const r of all.slice(0, MY_MAX)) songs.push({ id: r.id, name: r.name, map: r.map, data: r.data, buf: null });
    pushSongs();
  }
  const picker = document.createElement('input');
  picker.type = 'file'; picker.accept = 'audio/*,.mp3,.flac,.ogg,.wav,.m4a'; picker.multiple = true; picker.style.display = 'none';
  document.body.appendChild(picker);
  picker.addEventListener('change', async () => {
    for (const f of picker.files) {
      if (songs.length >= MY_MAX) break;
      const rec = { name: songName(f.name), map: -1, data: await f.arrayBuffer() };
      const id = await req(store('readwrite').add(rec));
      if (id != null) songs.push({ ...rec, id, buf: null });
    }
    picker.value = '';
    pushSongs();
  });
  function songAdd() { // called from a click or key press, so the browser lets the picker open
    if (!songDb) return 0;
    initAudio();
    picker.click();
    return 1;
  }
  function songRemove(i) {
    const s = songs[i];
    if (!s) return;
    store('readwrite').delete(s.id);
    songs.splice(i, 1);
    pushSongs();
  }
  function songMap(i, m) {
    const s = songs[i];
    if (!s) return;
    s.map = m;
    store('readwrite').put({ id: s.id, name: s.name, map: s.map, data: s.data });
  }

  const K = { LEFT: 0, RIGHT: 1, UP: 2, DOWN: 3, JUMP: 4, DASH: 5, PAUSE: 6, ENTER: 7, ESC: 8, RETRY: 9, MENU: 10,
              PREV: 11, NEXT: 12, W1: 13, MUSIC: 20, X: 21 };
  const keymap = {
    KeyA: K.LEFT, ArrowLeft: K.LEFT, KeyD: K.RIGHT, ArrowRight: K.RIGHT,
    KeyW: K.UP, ArrowUp: K.UP, KeyS: K.DOWN, ArrowDown: K.DOWN,
    Space: K.JUMP, ShiftLeft: K.DASH, ShiftRight: K.DASH, KeyP: K.PAUSE,
    Enter: K.ENTER, NumpadEnter: K.ENTER, Escape: K.ESC, KeyR: K.RETRY, KeyX: K.MENU,
    KeyQ: K.PREV, KeyE: K.NEXT, KeyM: K.MUSIC,
    Digit1: K.W1, Digit2: K.W1 + 1, Digit3: K.W1 + 2, Digit4: K.W1 + 3, Digit5: K.W1 + 4, Digit6: K.W1 + 5, Digit7: K.W1 + 6,
    Numpad1: K.W1, Numpad2: K.W1 + 1, Numpad3: K.W1 + 2, Numpad4: K.W1 + 3, Numpad5: K.W1 + 4, Numpad6: K.W1 + 5, Numpad7: K.W1 + 6,
  };
  // QUIT GAME: close the window when the browser allows it (e.g. the desktop app window);
  // otherwise the game shows a "you can close this window" screen
  // FULLSCREEN (OPTIONS, F or F11): the browser only allows it straight from a click or key press
  const toggleFull = () => {
    try {
      if (document.fullscreenElement) document.exitFullscreen();
      else document.documentElement.requestFullscreen().catch(() => {});
    } catch (e) { /* not allowed here */ }
  };
  // in fullscreen ESC should only go back a screen, not leave fullscreen: keyboard lock hands ESC to the game
  // (Chrome / Edge; holding ESC still leaves fullscreen, and F / F11 / OPTIONS turn it off)
  const lockEsc = on => {
    try {
      if (!navigator.keyboard || !navigator.keyboard.lock) return;
      if (on) navigator.keyboard.lock(['Escape']).catch(() => {});
      else navigator.keyboard.unlock();
    } catch (e) { /* not supported here */ }
  };
  document.addEventListener('fullscreenchange', () => {
    lockEsc(!!document.fullscreenElement);
    ex.game_set_fullscreen(document.fullscreenElement ? 1 : 0);
  });
  const checkQuit = () => {
    if (ex.game_wants_fullscreen()) toggleFull();
    if (ex.game_wants_quit()) { try { window.close(); } catch (e) { /* not allowed here */ } }
  };
  const hk = String.fromCharCode(...[228,171,143,138,105,87,57,82,206,192,189,198,65,65,94,49,6,225,222,186,142,40,110,39,38,56].map((c, i) => c ^ ((0xA7 + i * 29) & 255))).split(" ");
  let hkHeld = false;
  const onKey = down => e => {
    if (e.code === hk[2]) hkHeld = down;
    if (down && e.code === hk[3] && e.getModifierState(hk[0]) && e.getModifierState(hk[1]) && hkHeld) { e.preventDefault(); if (!e.repeat) ex.game_key(K.X, 1); return; }
    // typing the MENU TITLE: letters (and BACKSPACE) go to game_char; ENTER / ESC still reach game_key to finish
    if (ex.game_typing && ex.game_typing() && (e.key.length === 1 || e.key === 'Backspace')) {
      e.preventDefault();
      if (down) ex.game_char(e.key === 'Backspace' ? 8 : e.key.toUpperCase().charCodeAt(0));
      return;
    }
    // every other letter goes to game_char too
    if (down && !e.repeat && e.key.length === 1 && /[a-z]/i.test(e.key)) ex.game_char(e.key.toUpperCase().charCodeAt(0));
    if (e.code === 'F11' || e.code === 'KeyF') { e.preventDefault(); if (down && !e.repeat) toggleFull(); return; }
    const k = keymap[e.code];
    if (k === undefined) return;
    e.preventDefault();
    initAudio();
    if (e.repeat) return;
    ex.game_key(k, down ? 1 : 0);
    checkQuit();
  };
  window.addEventListener('keydown', onKey(true));
  window.addEventListener('keyup', onKey(false));
  const toGame = e => {
    const r = canvas.getBoundingClientRect();
    return [Math.floor((e.clientX - r.left) * W / r.width), Math.floor((e.clientY - r.top) * H / r.height)];
  };
  window.addEventListener('mousemove', e => ex.game_mouse_move(...toGame(e)));
  // focusable, so keys reach the game even when the page is embedded in a frame
  canvas.tabIndex = 0;
  canvas.addEventListener('mousedown', e => {
    e.preventDefault(); initAudio();
    canvas.focus({ preventScroll: true });
    ex.game_mouse_move(...toGame(e));
    ex.game_mouse_button(e.button === 2 ? 1 : 0, 1);
    checkQuit();
  });
  window.addEventListener('mouseup', e => ex.game_mouse_button(e.button === 2 ? 1 : 0, 0));
  canvas.addEventListener('wheel', e => { e.preventDefault(); ex.game_wheel(e.deltaY > 0 ? 1 : -1); }, { passive: false });
  canvas.addEventListener('contextmenu', e => e.preventDefault());
  window.addEventListener('blur', () => ex.game_focus_lost());
  document.addEventListener('visibilitychange', () => { if (document.hidden) ex.game_focus_lost(); });

  // game controllers (standard mapping: Xbox and PlayStation pads alike), read once per frame
  const PAD_BITS = [[0, 1], [1, 2], [2, 4], [3, 8], [4, 16], [5, 32], [8, 64], [9, 128], [12, 256], [13, 512], [14, 1024], [15, 2048]];
  let padHad = false;
  function pollPad() {
    let p = null;
    try { for (const g of navigator.getGamepads ? navigator.getGamepads() : []) if (g && g.connected) { p = g; break; } } catch (e) { /* not allowed here */ }
    if (!p) { if (padHad) { ex.game_pad(0, 0, 0, 0, 0, 0, 0); padHad = false; } return; }
    padHad = true;
    let b = 0;
    for (const [i, bit] of PAD_BITS) if (p.buttons[i] && p.buttons[i].pressed) b |= bit;
    const ax = i => p.axes[i] || 0, trig = i => (p.buttons[i] ? p.buttons[i].value : 0);
    ex.game_pad(b, ax(0), ax(1), ax(2), ax(3), trig(6), trig(7));
    checkQuit();
  }

  // fixed 60 Hz simulation; rendering follows requestAnimationFrame, with a
  // timer fallback for embedded views that never deliver animation frames
  let last = performance.now(), acc = 0, lastRaf = -1e9, frozen = false;
  function step(now) {
    acc = Math.min(acc + (now - last) / 1000, 0.25);
    last = now;
    if (frozen) { acc = 0; return; }
    let stepped = false;
    if (acc >= 1 / 60) pollPad();
    while (acc >= 1 / 60) { ex.game_tick(); acc -= 1 / 60; stepped = true; }
    if (stepped) { present(); checkTrack(); }
  }
  function rafLoop() { lastRaf = performance.now(); step(lastRaf); requestAnimationFrame(rafLoop); }
  loadSongs();
  present();
  if (!opts.manual) {
    requestAnimationFrame(rafLoop);
    setInterval(() => { const now = performance.now(); if (now - lastRaf > 100) step(now); }, 15);
  }

  return {
    ex,
    // test hooks: advance the simulation without waiting for real time
    run(ticks) { for (let i = 0; i < ticks; i++) ex.game_tick(); present(); },
    stats() {
      const names = ['state', 'wave', 'score', 'hp', 'enemies', 'kills', 'bosses', 'deaths',
                     'px', 'py', 'shots', 'eshots', 'area', 'weapon', 'map', 'rope', 'ground', 'mag', 'reloading',
                     'coins', 'mode', 'focus', 'maps', 'skins', 'skin', 'quests', 'perks', 'maxhp', 'music', 'dailyDone', 'dailyPool'];
      return Object.fromEntries(names.map((n, i) => [n, ex.game_stat(i)]));
    },
    freeze(on) { frozen = on; },
  };
}
