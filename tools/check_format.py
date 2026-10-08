#!/usr/bin/env python3
"""Check changed C/C++ files, honoring .clang-format-ignore."""

import argparse
import os
from pathlib import Path
import re
import subprocess
import sys


ROOT = Path(__file__).resolve().parents[1]
EXTENSIONS = {".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".hxx", ".inl"}


def git(*args):
    return subprocess.check_output(["git", *args], cwd=ROOT)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--base", help="Check working-tree files changed since the merge base with this ref")
    parser.add_argument("--clang-format", default="clang-format", help="Formatter executable")
    args = parser.parse_args()

    expected = (ROOT / "tools/format-requirements.txt").read_text().strip().split("==")[1]
    version = subprocess.check_output([args.clang_format, "--version"], text=True)
    match = re.search(r"clang-format version (\d+\.\d+\.\d+)\b", version)
    if not match or match.group(1) != expected:
        parser.error(f"clang-format {expected} is required; found {version.strip()}")

    # Validate configuration even when the change contains no C/C++ files.
    subprocess.run(
        [args.clang_format, "--style=file", "--assume-filename=check.cpp", "--dump-config"],
        cwd=ROOT, stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, check=True,
    )
    base = git("merge-base", args.base, "HEAD").decode().strip() if args.base else "HEAD"
    changed = git("diff", "--name-only", "-z", "--diff-filter=ACMRT", base, "--")
    untracked = git("ls-files", "--others", "--exclude-standard", "-z")
    paths = sorted({os.fsdecode(p) for p in (changed + untracked).split(b"\0") if p})
    failed = False
    count = 0
    for path in paths:
        source = ROOT / path
        if source.suffix.lower() not in EXTENSIONS or source.is_symlink() or not source.is_file():
            continue
        count += 1
        # Pass filenames directly so clang-format applies its native ignore rules.
        result = subprocess.run(
            [args.clang_format, "--style=file", "--dry-run", "--Werror", "--", "./" + path],
            cwd=ROOT,
        )
        failed |= result.returncode != 0
    print(f"Checked {count} candidate C/C++ file(s); .clang-format-ignore exclusions apply.")
    return int(failed)


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, subprocess.CalledProcessError) as error:
        print(f"Formatting check failed: {error}", file=sys.stderr)
        sys.exit(1)
