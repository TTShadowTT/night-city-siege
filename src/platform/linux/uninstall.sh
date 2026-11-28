#!/bin/sh
# Removes NIGHT CITY SIEGE. Your save (coins, unlocks) and songs in ~/.local/share/NeonEdge are kept.
DEST="$HOME/Games/NeonEdge"
APPS="${XDG_DATA_HOME:-$HOME/.local/share}/applications"
DESK=$(xdg-user-dir DESKTOP 2>/dev/null || echo "$HOME/Desktop")
rm -f "$APPS/neon-edge.desktop" "$DESK/neon-edge.desktop"
grep -q "NeonEdge command" "$HOME/.local/bin/neon-edge" 2>/dev/null && rm -f "$HOME/.local/bin/neon-edge"
rm -rf "$DEST"
rmdir "$HOME/Games" 2>/dev/null || true
command -v update-desktop-database >/dev/null 2>&1 && update-desktop-database "$APPS" 2>/dev/null || true
echo "Night City Siege was removed."
