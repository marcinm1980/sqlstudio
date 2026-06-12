#!/usr/bin/env bash
#
# compile.sh — Build MySQL Studio (Workbench) on Linux using CMake
#
# Environment:
#   WB_BUNDLE_DIR   Path to the directory containing pre-built 3rd-party
#                   libraries (headers in include/, libs in lib/, binaries
#                   in bin/).  When set the script forwards the paths to
#                   CMake so that Find-modules can locate every bundled
#                   dependency (MySQL Connector/C++, vsqlitepp, ANTLR4,
#                   GDAL, libssh, iODBC, Rapidjson …).
#
# Usage:
#   ./compile.sh [options]
#
# Options:
#   -b, --build-type    Debug | Release | RelWithDebInfo  (default: Release)
#   -j, --jobs          Parallel jobs for make             (default: nproc)
#   -p, --prefix        CMAKE_INSTALL_PREFIX               (default: /usr/local)
#   -B, --build-dir     Out-of-source build directory      (default: <source>/build/output)
#       --bundled-mysql  Pass -DUSE_BUNDLED_MYSQL=ON
#       --unixodbc       Use unixODBC instead of iODBC
#       --cotire         Enable cotire pre-compiled headers
#       --gcov           Instrument for gcov coverage
#       --test           Enable TEST_BUILD (extra debug libs)
#       --antlr-jar PATH Explicit path to the ANTLR 4 complete jar
#       --clean          Remove build directory before configuring
#       --install        Run 'make install' after build
#   -h, --help          Show this help message
#
set -euo pipefail

###############################################################################
# Colours
###############################################################################
if [[ -t 1 ]]; then
    C_RESET='\033[0m'
    C_BOLD='\033[1m'
    C_RED='\033[1;31m'
    C_GREEN='\033[1;32m'
    C_YELLOW='\033[1;33m'
    C_BLUE='\033[1;34m'
    C_MAGENTA='\033[1;35m'
    C_CYAN='\033[1;36m'
    C_WHITE='\033[1;37m'
    C_DIM='\033[2m'
else
    C_RESET='' C_BOLD='' C_RED='' C_GREEN='' C_YELLOW=''
    C_BLUE='' C_MAGENTA='' C_CYAN='' C_WHITE='' C_DIM=''
fi

###############################################################################
# Logging helpers
###############################################################################
info()    { echo -e "${C_CYAN}[INFO]${C_RESET}    $*"; }
ok()      { echo -e "${C_GREEN}[OK]${C_RESET}      $*"; }
warn()    { echo -e "${C_YELLOW}[WARN]${C_RESET}    $*"; }
error()   { echo -e "${C_RED}[ERROR]${C_RESET}   $*" >&2; }
section() { echo -e "\n${C_MAGENTA}${C_BOLD}── $* ──${C_RESET}"; }
detail()  { echo -e "  ${C_DIM}$*${C_RESET}"; }

banner() {
    echo -e "${C_BLUE}${C_BOLD}"
    echo "╔══════════════════════════════════════════════════════╗"
    if [[ "${PLATFORM_NAME}" == "macOS" ]]; then
        echo "║          MySQL Studio — macOS Build Script          ║"
    else
        echo "║          MySQL Studio — Linux Build Script          ║"
    fi
    echo "╚══════════════════════════════════════════════════════╝"
    echo -e "${C_RESET}"
}

###############################################################################
# Resolve source root (parent of build/)
###############################################################################
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
SOURCE_PARENT="$(cd "${SOURCE_DIR}/.." && pwd)"

case "$(uname -s)" in
    Darwin)
        PLATFORM_NAME="macOS"
        LIB_EXT="dylib"
        ;;
    *)
        PLATFORM_NAME="Linux"
        LIB_EXT="so"
        ;;
esac

###############################################################################
# Defaults
###############################################################################
BUILD_TYPE="Release"
JOBS="$(nproc 2>/dev/null || echo 4)"
INSTALL_PREFIX="/usr/local"
BUILD_DIR="${SOURCE_DIR}/build/output"
USE_BUNDLED_MYSQL="OFF"
USE_UNIXODBC="OFF"
ENABLE_COTIRE="OFF"
BUILD_FOR_GCOV="OFF"
TEST_BUILD="OFF"
ANTLR_JAR=""
DO_CLEAN=false
DO_INSTALL=false

###############################################################################
# Parse arguments
###############################################################################
usage() {
    sed -n '/^# Usage:/,/^#$/p' "$0" | sed 's/^# \?//'
    sed -n '/^# Options:/,/^#$/{ /^#$/d; s/^# \?//; p }' "$0"
    exit 0
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        -b|--build-type)    BUILD_TYPE="$2";        shift 2 ;;
        -j|--jobs)          JOBS="$2";              shift 2 ;;
        -p|--prefix)        INSTALL_PREFIX="$2";    shift 2 ;;
        -B|--build-dir)     BUILD_DIR="$2";         shift 2 ;;
        --bundled-mysql)    USE_BUNDLED_MYSQL="ON";  shift   ;;
        --unixodbc)         USE_UNIXODBC="ON";       shift   ;;
        --cotire)           ENABLE_COTIRE="ON";      shift   ;;
        --gcov)             BUILD_FOR_GCOV="ON";     shift   ;;
        --test)             TEST_BUILD="ON";         shift   ;;
        --antlr-jar)        ANTLR_JAR="$2";         shift 2 ;;
        --clean)            DO_CLEAN=true;           shift   ;;
        --install)          DO_INSTALL=true;         shift   ;;
        -h|--help)          usage ;;
        *)
            error "Unknown option: ${C_BOLD}$1"
            echo "Run ${C_CYAN}$0 --help${C_RESET} for usage."
            exit 1
            ;;
    esac
done

###############################################################################
# Banner & summary
###############################################################################
banner

section "Configuration"
info "Source directory    : ${C_WHITE}${SOURCE_DIR}"
info "Build directory    : ${C_WHITE}${BUILD_DIR}"
info "Build type         : ${C_WHITE}${BUILD_TYPE}"
info "Install prefix     : ${C_WHITE}${INSTALL_PREFIX}"
info "Parallel jobs      : ${C_WHITE}${JOBS}"

if [[ -n "${WB_BUNDLE_DIR:-}" ]]; then
    ok   "WB_BUNDLE_DIR      : ${C_WHITE}${WB_BUNDLE_DIR}"
elif [[ -d "${SOURCE_PARENT}/bundle/lib" ]]; then
    export WB_BUNDLE_DIR="${SOURCE_PARENT}/bundle"
    ok   "WB_BUNDLE_DIR      : ${C_WHITE}${WB_BUNDLE_DIR} ${C_DIM}(auto-detected)"
else
    warn "WB_BUNDLE_DIR is ${C_BOLD}not set${C_RESET}${C_YELLOW} — system packages will be used for 3rd-party libs"
fi

[[ "${USE_BUNDLED_MYSQL}" == "ON" ]] && info "Bundled MySQL      : ${C_GREEN}ON"
[[ "${USE_UNIXODBC}"      == "ON" ]] && info "ODBC driver        : ${C_GREEN}unixODBC"
[[ "${ENABLE_COTIRE}"     == "ON" ]] && info "Cotire (PCH)       : ${C_GREEN}ON"
[[ "${BUILD_FOR_GCOV}"    == "ON" ]] && info "gcov coverage      : ${C_GREEN}ON"
[[ "${TEST_BUILD}"        == "ON" ]] && info "Test build         : ${C_GREEN}ON"
[[ -n "${ANTLR_JAR}" ]]              && info "ANTLR jar          : ${C_WHITE}${ANTLR_JAR}"

###############################################################################
# Quick dependency smoke-test
###############################################################################
section "Checking host tools"

MISSING_TOOLS=()
for tool in cmake make gcc g++ pkg-config swig python3; do
    if command -v "$tool" &>/dev/null; then
        ok "$(printf '%-14s' "$tool") $(command -v "$tool") ${C_DIM}($(${tool} --version 2>&1 | head -1))"
    else
        error "$(printf '%-14s' "$tool") ${C_RED}NOT FOUND"
        MISSING_TOOLS+=("$tool")
    fi
done

if [[ ${#MISSING_TOOLS[@]} -gt 0 ]]; then
    echo
    error "Missing required tools: ${C_BOLD}${MISSING_TOOLS[*]}"
    error "Install them and re-run."
    exit 1
fi

###############################################################################
# Clean (optional)
###############################################################################
if $DO_CLEAN && [[ -d "${BUILD_DIR}" ]]; then
    section "Cleaning previous build"
    warn "Removing ${C_WHITE}${BUILD_DIR}"
    rm -rf "${BUILD_DIR}"
    ok "Clean complete."
fi

mkdir -p "${BUILD_DIR}"

###############################################################################
# Build CMAKE_ARGS
###############################################################################
section "Preparing CMake arguments"

CMAKE_ARGS=(
    -G "Unix Makefiles"
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
    -DCMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}"
    -DUSE_BUNDLED_MYSQL="${USE_BUNDLED_MYSQL}"
    -DUSE_UNIXODBC="${USE_UNIXODBC}"
    -DENABLE_COTIRE="${ENABLE_COTIRE}"
    -DBUILD_FOR_GCOV="${BUILD_FOR_GCOV}"
    -DTEST_BUILD="${TEST_BUILD}"
)

if [[ "${PLATFORM_NAME}" == "macOS" ]]; then
    PYTHON_EXECUTABLE_PATH="$(command -v python3 || true)"
    PYTHON_INCLUDE_HINT="$(${PYTHON_EXECUTABLE_PATH:-python3} -c 'import sysconfig; print(sysconfig.get_config_var("INCLUDEPY") or "")' 2>/dev/null || true)"
    PYTHON_LIBRARY_HINT="$(${PYTHON_EXECUTABLE_PATH:-python3} -c 'import os,sysconfig; libdir=sysconfig.get_config_var("LIBDIR") or ""; ldlib=sysconfig.get_config_var("LDLIBRARY") or ""; print(os.path.normpath(os.path.join(libdir, ldlib)) if libdir and ldlib else "")' 2>/dev/null || true)"

    if [[ -n "${PYTHON_INCLUDE_HINT}" && -d "${PYTHON_INCLUDE_HINT}" ]]; then
        PYTHON_FRAMEWORK_DIR="$(cd "${PYTHON_INCLUDE_HINT}/.." 2>/dev/null && pwd || true)"
        PYTHON_FRAMEWORK_BIN="${PYTHON_FRAMEWORK_DIR}/Python3"
        if [[ -z "${PYTHON_LIBRARY_HINT}" || ! -e "${PYTHON_LIBRARY_HINT}" ]]; then
            if [[ -e "${PYTHON_FRAMEWORK_BIN}" ]]; then
                PYTHON_LIBRARY_HINT="${PYTHON_FRAMEWORK_BIN}"
            fi
        fi
    fi

    if [[ -n "${PYTHON_EXECUTABLE_PATH}" ]]; then
        CMAKE_ARGS+=( -DPYTHON_EXECUTABLE="${PYTHON_EXECUTABLE_PATH}" )
    fi
    if [[ -n "${PYTHON_INCLUDE_HINT}" && -d "${PYTHON_INCLUDE_HINT}" ]]; then
        CMAKE_ARGS+=( -DPYTHON_INCLUDE_DIR="${PYTHON_INCLUDE_HINT}" )
    fi
    if [[ -n "${PYTHON_LIBRARY_HINT}" && -e "${PYTHON_LIBRARY_HINT}" ]]; then
        CMAKE_ARGS+=( -DPYTHON_LIBRARY="${PYTHON_LIBRARY_HINT}" )
    fi

    if [[ -z "${ANTLR_JAR}" ]] && command -v brew &>/dev/null; then
        BREW_ANTLR_PREFIX="$(brew --prefix antlr 2>/dev/null || true)"
        if [[ -n "${BREW_ANTLR_PREFIX}" ]]; then
            BREW_ANTLR_JAR="$(find -L "${BREW_ANTLR_PREFIX}" -type f -name 'antlr-4*-complete.jar' | head -n 1)"
            if [[ -n "${BREW_ANTLR_JAR}" && -f "${BREW_ANTLR_JAR}" ]]; then
                ANTLR_JAR="${BREW_ANTLR_JAR}"
            fi
        fi
    fi
fi

# ------- WB_BUNDLE_DIR integration -------
# When WB_BUNDLE_DIR is set we expose its sub-directories via CMAKE_PREFIX_PATH
# and also set individual hint variables so that Find-modules locate every
# bundled dependency.
if [[ -n "${WB_BUNDLE_DIR:-}" ]]; then
    if [[ ! -d "${WB_BUNDLE_DIR}" ]]; then
        error "WB_BUNDLE_DIR points to a non-existent directory: ${WB_BUNDLE_DIR}"
        exit 1
    fi

    BUNDLE_LIB="${WB_BUNDLE_DIR}/lib"
    BUNDLE_INC="${WB_BUNDLE_DIR}/include"
    BUNDLE_BIN="${WB_BUNDLE_DIR}/bin"

    CMAKE_ARGS+=(
        -DCMAKE_PREFIX_PATH="${WB_BUNDLE_DIR}"
        # MySQL client (libmysqlclient)
        -DMYSQL_LIBRARY="${BUNDLE_LIB}/libmysqlclient.${LIB_EXT}"
        -DMYSQL_INCLUDE_DIR="${BUNDLE_INC}"
        # MySQL Connector/C++ (JDBC API — headers in include/jdbc/)
        -DMYSQLCPPCONN_LIBRARY="${BUNDLE_LIB}/libmysqlcppconn.${LIB_EXT}"
        -DMYSQLCPPCONN_INCLUDE_DIR="${BUNDLE_INC}/jdbc"
        # vsqlite++ (headers in include/sqlite/)
        -DVSQLITE_LIBRARY="${BUNDLE_LIB}/libvsqlitepp.${LIB_EXT}"
        -DVSQLITE_INCLUDE_DIR="${BUNDLE_INC}"
        # GDAL
        -DGDAL_LIBRARY="${BUNDLE_LIB}/libgdal.${LIB_EXT}"
        -DGDAL_INCLUDE_DIR="${BUNDLE_INC}"
        # ANTLR4 runtime (headers in include/antlr4-runtime/)
        -DANTLR4_LIBRARY="${BUNDLE_LIB}/libantlr4-runtime.${LIB_EXT}"
        -DANTLR4_INCLUDE_DIR="${BUNDLE_INC}/antlr4-runtime"
        # libssh (cmake config in lib/cmake/libssh/)
        -Dlibssh_DIR="${BUNDLE_LIB}/cmake/libssh"
        # iODBC
        -DIODBC_LIBRARY="${BUNDLE_LIB}/libiodbc.${LIB_EXT}"
        -DIODBC_INCLUDE_DIR="${BUNDLE_INC}"
        # RapidJSON (header-only, in include/rapidjson/)
        -DRAPIDJSON_INCLUDE_DIR="${BUNDLE_INC}"
        # Boost (header-only, in include/boost/)
        -DBOOST_ROOT="${WB_BUNDLE_DIR}"
        -DBoost_NO_SYSTEM_PATHS=ON
        # OpenSSL — prefer system
        # SQLite (for pkg-config)
        -DCMAKE_LIBRARY_PATH="${BUNDLE_LIB}"
    )

    # Help pkg-config find bundled .pc files
    export PKG_CONFIG_PATH="${BUNDLE_LIB}/pkgconfig:${PKG_CONFIG_PATH:-}"

    detail "CMAKE_PREFIX_PATH = ${WB_BUNDLE_DIR}"
fi

# ANTLR jar override
if [[ -n "${ANTLR_JAR}" ]]; then
    CMAKE_ARGS+=(-DWITH_ANTLR_JAR="${ANTLR_JAR}")
fi

# Print all CMake arguments for transparency
for arg in "${CMAKE_ARGS[@]}"; do
    detail "$arg"
done

###############################################################################
# CMake configure
###############################################################################
section "Configuring (CMake)"

info "Running cmake in ${C_WHITE}${BUILD_DIR}"
echo

cmake_log="${BUILD_DIR}/cmake_configure.log"

if cmake -S "${SOURCE_DIR}" -B "${BUILD_DIR}" "${CMAKE_ARGS[@]}" 2>&1 | tee "${cmake_log}"; then
    echo
    ok "CMake configuration succeeded."
else
    echo
    error "CMake configuration ${C_RED}FAILED${C_RESET}.  Log: ${C_WHITE}${cmake_log}"
    exit 1
fi

###############################################################################
# Build
###############################################################################
section "Building (make -j${JOBS})"

build_start=$SECONDS

if cmake --build "${BUILD_DIR}" -- -j"${JOBS}" 2>&1 | \
    while IFS= read -r line; do
        # Colourise compiler output on the fly
        if [[ "$line" =~ ^.*error:.* ]]; then
            echo -e "${C_RED}${line}${C_RESET}"
        elif [[ "$line" =~ ^.*warning:.* ]]; then
            echo -e "${C_YELLOW}${line}${C_RESET}"
        elif [[ "$line" =~ ^\[\ *[0-9]+%\] ]]; then
            echo -e "${C_GREEN}${line}${C_RESET}"
        elif [[ "$line" =~ ^Linking ]]; then
            echo -e "${C_CYAN}${line}${C_RESET}"
        elif [[ "$line" =~ ^Scanning|^Building ]]; then
            echo -e "${C_BLUE}${line}${C_RESET}"
        else
            echo "$line"
        fi
    done
then
    build_elapsed=$(( SECONDS - build_start ))
    echo
    ok "Build completed in ${C_BOLD}$(printf '%dm %ds' $((build_elapsed/60)) $((build_elapsed%60)))${C_RESET}"
else
    echo
    error "Build ${C_RED}FAILED${C_RESET}"
    exit 1
fi

###############################################################################
# Install (optional)
###############################################################################
if $DO_INSTALL; then
    section "Installing"
    info "Installing to ${C_WHITE}${INSTALL_PREFIX}"

    if cmake --install "${BUILD_DIR}" 2>&1 | tee "${BUILD_DIR}/cmake_install.log"; then
        ok "Installation completed."
    else
        error "Installation ${C_RED}FAILED${C_RESET}"
        exit 1
    fi
fi

###############################################################################
# Done
###############################################################################
echo
echo -e "${C_GREEN}${C_BOLD}╔══════════════════════════════════════════════════════╗${C_RESET}"
echo -e "${C_GREEN}${C_BOLD}║              Build finished successfully!            ║${C_RESET}"
echo -e "${C_GREEN}${C_BOLD}╚══════════════════════════════════════════════════════╝${C_RESET}"
echo
detail "Build artefacts : ${BUILD_DIR}"
detail "CMake log       : ${cmake_log}"
echo
