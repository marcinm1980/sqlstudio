#!/usr/bin/env bash
#
# Installs the SqlStudio .deb from /packages, starts a virtual X display that
# is reachable through noVNC on port 6080, and launches SqlStudio on it.
#
set -euo pipefail

shopt -s nullglob
packages=(/packages/*.deb)
if [[ ${#packages[@]} -ne 1 ]]; then
    echo "Expected exactly one .deb in /packages, found ${#packages[@]}." >&2
    echo "Copy the package downloaded from CI into the packages/ directory." >&2
    exit 1
fi

# Installing on every start also upgrades to a newly copied package.
echo "Installing ${packages[0]##*/}"
apt-get update -qq
apt-get install -y -qq "${packages[0]}"
dpkg -s sql-studio-community | grep -E '^(Version|Depends):'

# ── Display ───────────────────────────────────────────────────────────────
rm -f /tmp/.X1-lock /tmp/.X11-unix/X1
Xvfb "${DISPLAY}" -screen 0 "${SCREEN_GEOMETRY}" -nolisten tcp &
for _ in $(seq 50); do xdpyinfo >/dev/null 2>&1 && break; sleep 0.2; done

openbox &
x11vnc -display "${DISPLAY}" -forever -shared -nopw -quiet -rfbport 5900 &
websockify --web /usr/share/novnc 6080 localhost:5900 >/dev/null 2>&1 &

# A session bus and an unlocked keyring, so "Store in Keychain" works.
eval "$(dbus-launch --sh-syntax)"
printf '' | gnome-keyring-daemon --unlock --components=secrets >/dev/null

# A terminal to relaunch SqlStudio (`sql-studio`) after closing it.
xterm -geometry 100x20+0+0 &

echo
echo "Desktop ready: http://localhost:${NOVNC_PORT:-6080}/vnc.html?autoconnect=true&resize=scale"
echo "SqlStudio output follows (also in /var/log/sql-studio.log)."
echo

sql-studio "$@" 2>&1 | tee /var/log/sql-studio.log &

# Keep the container alive while the desktop is up.
wait
