#!/usr/bin/env bash
#
# install_deps.sh — Install all system packages required to build
#                    MySQL Studio and its bundled 3rd-party dependencies.
#
# Usage:
#   sudo ./install_deps.sh          # install everything
#   ./install_deps.sh --check       # dry-run — only check what's missing
#
set -euo pipefail

###############################################################################
# Colours
###############################################################################
if [[ -t 1 ]]; then
    C_RESET='\033[0m'; C_BOLD='\033[1m'; C_RED='\033[1;31m'
    C_GREEN='\033[1;32m'; C_YELLOW='\033[1;33m'; C_CYAN='\033[1;36m'
    C_MAGENTA='\033[1;35m'; C_DIM='\033[2m'
else
    C_RESET='' C_BOLD='' C_RED='' C_GREEN='' C_YELLOW='' C_CYAN='' C_MAGENTA='' C_DIM=''
fi

info()    { echo -e "${C_CYAN}[INFO]${C_RESET}    $*"; }
ok()      { echo -e "${C_GREEN}[ OK ]${C_RESET}    $*"; }
warn()    { echo -e "${C_YELLOW}[WARN]${C_RESET}    $*"; }
fail()    { echo -e "${C_RED}[FAIL]${C_RESET}    $*" >&2; }
section() { echo -e "\n${C_MAGENTA}${C_BOLD}── $* ──${C_RESET}"; }

###############################################################################
# Parse arguments
###############################################################################
CHECK_ONLY=false
[[ "${1:-}" == "--check" ]] && CHECK_ONLY=true

###############################################################################
# Must be root (unless --check)
###############################################################################
if ! $CHECK_ONLY && [[ $EUID -ne 0 ]]; then
    fail "This script must be run as root (or with sudo)."
    info "Use ${C_BOLD}--check${C_RESET} to see what's missing without installing."
    exit 1
fi

echo -e "${C_CYAN}${C_BOLD}"
echo "╔══════════════════════════════════════════════════════╗"
echo "║    MySQL Studio — System Dependencies Installer     ║"
echo "╚══════════════════════════════════════════════════════╝"
echo -e "${C_RESET}"

###############################################################################
# Package lists
###############################################################################

# ── Build tools ───────────────────────────────────────────────────────────
BUILD_TOOLS=(
    build-essential         # gcc, g++, make, etc.
    cmake
    pkg-config
    autoconf                # needed for vsqlitepp autogen.sh
    automake                # needed for vsqlitepp autogen.sh
    libtool                 # needed for vsqlitepp autogen.sh
    swig                    # SWIG — Python bindings generator
    python3-dev             # Python 3 headers (PythonLibs)
    default-jre-headless    # Java runtime (optional — for ANTLR jar)
    git
    wget
    curl
    unzip
)

# ── Libraries required by the main MySQL Studio build ─────────────────────
STUDIO_LIBS=(
    # GTK 3 / GTKMM
    libgtk-3-dev            # gtk+-3.0, glib-2.0, gthread-2.0, gmodule-2.0
    libgtkmm-3.0-dev        # gtkmm-3.0

    # X11 / OpenGL
    libx11-dev              # X11
    libgl-dev               # OpenGL
    libglu1-mesa-dev        # GLU

    # XML / Crypto / Compression
    libxml2-dev             # LibXml2
    libssl-dev              # OpenSSL
    libzip-dev              # libzip
    libsecret-1-dev         # libsecret-1

    # Graphics
    libcairo2-dev           # cairo
    libcairomm-1.0-dev      # cairomm (used by mga tool)

    # UUID
    uuid-dev                # uuid
)

# ── Libraries required by bundled 3rd-party builds ────────────────────────
THIRDPARTY_LIBS=(
    # MySQL server build (client-only)
    libncurses-dev          # ncurses — needed by mysql's bundled editline
    bison                   # SQL parser generator for MySQL server

    # GDAL build
    libproj-dev             # PROJ — hard dependency for GDAL
    libcurl4-openssl-dev    # curl — used by GDAL for HTTP/S support
)

ALL_PACKAGES=( "${BUILD_TOOLS[@]}" "${STUDIO_LIBS[@]}" "${THIRDPARTY_LIBS[@]}" )

###############################################################################
# Check / Install
###############################################################################
section "Checking packages"

MISSING=()
INSTALLED=()

for pkg in "${ALL_PACKAGES[@]}"; do
    if dpkg -s "$pkg" &>/dev/null; then
        ok "$pkg"
        INSTALLED+=("$pkg")
    else
        if $CHECK_ONLY; then
            warn "${C_YELLOW}MISSING${C_RESET}  $pkg"
        else
            info "Need to install: $pkg"
        fi
        MISSING+=("$pkg")
    fi
done

echo
info "Installed : ${C_GREEN}${#INSTALLED[@]}${C_RESET}"
info "Missing   : ${C_YELLOW}${#MISSING[@]}${C_RESET}"

if [[ ${#MISSING[@]} -eq 0 ]]; then
    echo
    ok "${C_GREEN}${C_BOLD}All dependencies are already installed!${C_RESET}"
    exit 0
fi

if $CHECK_ONLY; then
    echo
    warn "Missing packages:"
    echo "  ${MISSING[*]}"
    echo
    info "Run ${C_BOLD}sudo $0${C_RESET} to install them."
    exit 1
fi

###############################################################################
# Install missing packages
###############################################################################
section "Installing ${#MISSING[@]} missing packages"

apt-get update -qq

if apt-get install -y "${MISSING[@]}"; then
    echo
    ok "${C_GREEN}${C_BOLD}All packages installed successfully!${C_RESET}"
else
    echo
    fail "Some packages failed to install. Check output above."
    exit 1
fi

###############################################################################
# Summary
###############################################################################
echo
echo -e "${C_GREEN}${C_BOLD}╔══════════════════════════════════════════════════════╗${C_RESET}"
echo -e "${C_GREEN}${C_BOLD}║         System dependencies are ready!               ║${C_RESET}"
echo -e "${C_GREEN}${C_BOLD}╚══════════════════════════════════════════════════════╝${C_RESET}"
echo
echo -e "  ${C_DIM}Next steps:${C_RESET}"
echo -e "    1. Build 3rd-party deps:  ${C_CYAN}./build_3rdparty.sh${C_RESET}"
echo -e "    2. Build MySQL Studio:    ${C_CYAN}../compile.sh${C_RESET}"
echo
