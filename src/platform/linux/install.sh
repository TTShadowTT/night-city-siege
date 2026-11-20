#!/bin/sh
# Installs NIGHT CITY SIEGE for the current user (no root needed):
#   game files  -> ~/Games/NeonEdge
#   save, songs -> ~/.local/share/NeonEdge (kept when the game is updated or uninstalled)
#   menu entry  -> ~/.local/share/applications/neon-edge.desktop
#   command     -> ~/.local/bin/neon-edge
set -e
SRC=$(dirname "$(readlink -f "$0")")
DEST="$HOME/Games/NeonEdge"
APPS="${XDG_DATA_HOME:-$HOME/.local/share}/applications"
BIN="$HOME/.local/bin"
DATA="${XDG_DATA_HOME:-$HOME/.local/share}/NeonEdge" # the game finds it the same way

# The browser version kept its save inside the browser. The first time the native game is installed, copy it from
# there: a copy of the browser's Local Storage is opened by the same browser, headless, and asked for the save.
move_browser_save() {
    for pair in "google-chrome:$HOME/.config/google-chrome" "chromium:$HOME/.config/chromium" \
                "brave-browser:$HOME/.config/BraveSoftware/Brave-Browser" "microsoft-edge:$HOME/.config/microsoft-edge" \
                "vivaldi:$HOME/.config/vivaldi"; do
        b=${pair%%:*}; dir=${pair#*:}
        command -v "$b" >/dev/null 2>&1 && [ -d "$dir/Default/Local Storage" ] || continue
        T=$(mktemp -d)
        mkdir -p "$T/p/Default"
        cp -r "$dir/Default/Local Storage" "$T/p/Default/" 2>/dev/null || true
        rm -f "$T/p/Default/Local Storage/leveldb/LOCK"
        echo '<pre id=s></pre><script>document.getElementById("s").textContent=localStorage.getItem("slop_save_v1")||""</script>' > "$T/read.html"
        S=$(timeout 30 "$b" --headless=new --no-first-run --disable-gpu --user-data-dir="$T/p" --dump-dom "file://$T/read.html" 2>/dev/null |
            sed -n 's/.*<pre id="s">\(.*\)<\/pre>.*/\1/p' | sed 's/&lt;/</g; s/&gt;/>/g; s/&quot;/"/g; s/&amp;/\&/g') || true
        rm -rf "$T"
        case "$S" in SLOP*) printf '%s' "$S" > "$DATA/slop_save.txt"; echo "Your progress from the browser version was copied over."; return;; esac
    done
}

for f in neon-edge-bin neon-edge neon-edge.png uninstall.sh; do
    [ -f "$SRC/$f" ] || { echo "Missing $f - run install.sh from inside the unpacked NeonEdge folder." >&2; exit 1; }
done

mkdir -p "$DEST" "$APPS" "$BIN" "$DATA"
rm -f "$DEST/neon_edge.html" # the old browser version
cp "$SRC/neon-edge-bin" "$SRC/neon-edge" "$SRC/neon-edge.png" "$SRC/uninstall.sh" "$DEST/"
# the window icon and map pictures (only in builds made with build.ps1; a source build runs without them)
if [ -f "$SRC/neon-edge.dat" ]; then cp "$SRC/neon-edge.dat" "$DEST/"; else rm -f "$DEST/neon-edge.dat"; fi
chmod 755 "$DEST/neon-edge-bin" "$DEST/neon-edge" "$DEST/uninstall.sh"
[ -f "$DATA/slop_save.txt" ] || move_browser_save
# the icon gets a new file name whenever it changes, so desktops that cache icons by path pick up the new one
cp "$SRC/neon-edge.png" "$DEST/night-city-siege.png"
printf '#!/bin/sh\n# NeonEdge command\nexec "%s/neon-edge" "$@"\n' "$DEST" > "$BIN/neon-edge"
chmod 755 "$BIN/neon-edge"

cat > "$APPS/neon-edge.desktop" <<DESKTOP
[Desktop Entry]
Type=Application
Name=Night City Siege
GenericName=Platform Shooter
Comment=Pixel night-city platform shooter
Exec="$DEST/neon-edge"
Icon=$DEST/night-city-siege.png
StartupWMClass=neon-edge
Terminal=false
Categories=Game;ActionGame;
Keywords=shooter;cyberpunk;pixel;
DESKTOP
chmod 644 "$APPS/neon-edge.desktop"

# Optional: a desktop icon too, if the user has a Desktop folder.
DESK=$(xdg-user-dir DESKTOP 2>/dev/null || echo "$HOME/Desktop")
if [ -d "$DESK" ] && [ "$DESK" != "$HOME" ]; then
    cp "$APPS/neon-edge.desktop" "$DESK/neon-edge.desktop"
    chmod 755 "$DESK/neon-edge.desktop"
    gio set "$DESK/neon-edge.desktop" metadata::trusted true 2>/dev/null || true
fi

command -v update-desktop-database >/dev/null 2>&1 && update-desktop-database "$APPS" 2>/dev/null || true

echo "Night City Siege is installed."
echo "Start it from your app menu (search \"Night City Siege\"), the desktop icon, or by typing: neon-edge"
case ":$PATH:" in *":$BIN:"*) ;; *) echo "(The neon-edge command works after you log out and back in, or use: $DEST/neon-edge)";; esac
