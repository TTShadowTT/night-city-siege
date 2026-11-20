# Changelog

What changed in Night City Siege, newest first. The game itself is always the latest version at
<https://ttshadowtt.github.io/night-city-siege/>; there are no downloads to keep up to date.

## November 2026

- **Install guides** for Windows and Linux in the README, with step-by-step instructions for the browser app and
  for building the game yourself
- **Linux: build from source** with one script, `scripts/linux_from_source.sh`, on Debian, Ubuntu, Fedora and Arch
- **Windows:** your progress and songs are saved in `%LOCALAPPDATA%\NeonEdge`, so a copy of the game on the
  desktop never leaves files next to itself
- New README with screenshots, gameplay GIFs and pictures of every boss, character, enemy and pet

## October 2026

### Enemy crews
- Every enemy skin is now its own crew with its own bodies, not just new colours: thugs, gunners, brutes and
  drones are taller, wider or completely different per crew
- **Night City factions:** Arasaka, MaxTac, Tyger Claws, Cyberpsychos, Scavs and Adam Smasher
- **Classic crews:** Street Gang, Zombies, Robots, Clowns and Aliens

### Native Linux game
- Linux runs a small native program (X11 and PulseAudio) instead of a browser window, using about 20 MB of RAM
- Controllers work through the kernel's joystick driver; MY SONGS works when `ffmpeg` is installed

### Characters
- The Edgerunners crew redrawn closer to the anime, each with their own build
- New skins: Falco, Gloria, Dorio and Pilar
- Name your pet; the name floats above it

### Gameplay and options
- Achievements, hard mode, boss cards and controller support (Xbox and PlayStation pads)
- Sound effects on / off in the options, next to the music volume
- Fullscreen fills the whole screen without black bars

### Under the hood
- The game core is split into small modules (`src/core`) with one platform layer per system
