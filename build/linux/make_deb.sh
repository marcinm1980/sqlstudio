#!/usr/bin/env bash
#
# make_deb.sh — Package an installed SqlStudio tree as a .deb
#
# Takes the tree produced by `DESTDIR=<stage> cmake --install <build-dir>`
# (configured with CMAKE_INSTALL_PREFIX=/usr) and wraps it in a Debian
# package. Runtime dependencies on distro packages are computed with
# dpkg-shlibdeps, so the package must be built on the distro it targets.
#
# Usage:
#   ./make_deb.sh --stage DIR [--out DIR] [--release N] [--distro-tag TAG]
#
# Options:
#   --stage DIR       Installed tree (contains usr/)               (required)
#   --out DIR         Where to write the .deb                       (default: .)
#   --release N       Debian revision                               (default: 1)
#   --distro-tag TAG  Version suffix, e.g. ubuntu22.04              (default: from /etc/os-release)
#   -h, --help        Show this help message
#
# Environment:
#   DEB_MAINTAINER    Overrides the Maintainer field
#   DEB_HOMEPAGE      Overrides the Homepage field
#
set -euo pipefail

PACKAGE="sql-studio-community"
MAINTAINER="${DEB_MAINTAINER:-Marcin Marek <marcin@dev4fun.net>}"
HOMEPAGE="${DEB_HOMEPAGE:-https://www.dev4fun.net/}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"

STAGE_DIR=""
OUT_DIR="."
RELEASE="1"
DISTRO_TAG=""

usage() {
    sed -n '/^# Usage:/,/^#$/p;/^# Options:/,/^#$/p;/^# Environment:/,/^#$/p' "$0" | sed 's/^# \{0,1\}//'
    exit 0
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --stage)      STAGE_DIR="$2";  shift 2 ;;
        --out)        OUT_DIR="$2";    shift 2 ;;
        --release)    RELEASE="$2";    shift 2 ;;
        --distro-tag) DISTRO_TAG="$2"; shift 2 ;;
        -h|--help)    usage ;;
        *) echo "Unknown option: $1" >&2; exit 1 ;;
    esac
done

[[ -n "${STAGE_DIR}" && -d "${STAGE_DIR}/usr" ]] || {
    echo "--stage must point at an installed tree containing usr/" >&2
    exit 1
}

for tool in dpkg-deb dpkg-shlibdeps dpkg-architecture; do
    command -v "$tool" >/dev/null || { echo "$tool not found (install dpkg-dev)" >&2; exit 1; }
done

if [[ -z "${DISTRO_TAG}" ]]; then
    # shellcheck disable=SC1091
    DISTRO_TAG="$(. /etc/os-release && echo "${ID}${VERSION_ID}")"
fi

UPSTREAM_VERSION="$("${SOURCE_DIR}/tools/get_wb_version.sh" full_version)"
VERSION="${UPSTREAM_VERSION}-${RELEASE}~${DISTRO_TAG}"
ARCH="$(dpkg-architecture -qDEB_HOST_ARCH)"
LIB_DIR="usr/lib/sql-studio"

WORK_DIR="$(mktemp -d)"
trap 'rm -rf "${WORK_DIR}"' EXIT
PKG_DIR="${WORK_DIR}/${PACKAGE}"

cp -a "${STAGE_DIR}" "${PKG_DIR}"
mkdir -p "${PKG_DIR}/DEBIAN"

# ── Runtime dependencies ──────────────────────────────────────────────────
# dpkg-shlibdeps needs a debian/control in its working directory.
mkdir -p "${WORK_DIR}/shlibs/debian"
printf 'Source: %s\n\nPackage: %s\nArchitecture: any\n' "${PACKAGE}" "${PACKAGE}" \
    > "${WORK_DIR}/shlibs/debian/control"

mapfile -t ELF_FILES < <(
    find "${PKG_DIR}/usr" -type f -print0 |
    while IFS= read -r -d '' f; do
        [[ "$(head -c 4 "$f" | od -An -c | tr -d ' ')" == '177ELF' ]] && echo "$f"
    done
)
[[ ${#ELF_FILES[@]} -gt 0 ]] || { echo "No ELF files found in ${STAGE_DIR}" >&2; exit 1; }

# Libraries bundled in LIB_DIR satisfy each other and are not distro
# packages, so their missing package info is expected.
SHLIBS_DEPENDS="$(
    cd "${WORK_DIR}/shlibs" &&
    dpkg-shlibdeps -O --ignore-missing-info \
        -l"${PKG_DIR}/${LIB_DIR}" -l"${PKG_DIR}/${LIB_DIR}/plugins" -l"${PKG_DIR}/${LIB_DIR}/modules" \
        "${ELF_FILES[@]}" 2>"${WORK_DIR}/shlibdeps.log" |
    sed -n 's/^shlibs:Depends=//p'
)" || { cat "${WORK_DIR}/shlibdeps.log" >&2; exit 1; }
[[ -n "${SHLIBS_DEPENDS}" ]] || { cat "${WORK_DIR}/shlibdeps.log" >&2; exit 1; }

# ── Control files ─────────────────────────────────────────────────────────
INSTALLED_SIZE="$(du -sk --exclude=DEBIAN "${PKG_DIR}" | cut -f1)"

cat > "${PKG_DIR}/DEBIAN/control" <<EOF
Package: ${PACKAGE}
Version: ${VERSION}
Architecture: ${ARCH}
Maintainer: ${MAINTAINER}
Installed-Size: ${INSTALLED_SIZE}
Depends: ${SHLIBS_DEPENDS}
Suggests: gnome-keyring
Section: database
Priority: optional
Homepage: ${HOMEPAGE}
Description: Visual database design, administration and development tool
 SqlStudio is a development, administration and data modeling tool for
 MySQL and MariaDB. Besides editing and running SQL queries and scripts,
 it supports the design of databases through EER diagrams, server
 administration, performance monitoring and migration from other RDBMS
 products.
EOF

( cd "${PKG_DIR}" && find usr -type f -print0 | xargs -0 md5sum ) > "${PKG_DIR}/DEBIAN/md5sums"

mkdir -p "${OUT_DIR}"
DEB_FILE="${OUT_DIR}/${PACKAGE}_${VERSION}_${ARCH}.deb"
dpkg-deb --root-owner-group -Zxz --build "${PKG_DIR}" "${DEB_FILE}"

echo "Built ${DEB_FILE}"
echo "Depends: ${SHLIBS_DEPENDS}"
