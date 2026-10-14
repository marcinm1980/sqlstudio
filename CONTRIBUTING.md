# Contributing

Thanks for contributing to SqlStudio.

This project is independently maintained by dev4fun. Contributions, bug reports,
and feature ideas are handled through the GitHub repository.

## Before You Start

- Use [GitHub Discussions](https://github.com/dante-d4f/sqlstudio/discussions) for questions, design discussion,
  and general feedback.
- Use [GitHub Issues](https://github.com/dante-d4f/sqlstudio/issues) to report bugs or request features.
- Search existing issues and discussions before opening a new one.

## Submitting Changes

1. Fork the repository and create a branch from `develop`.
2. Keep changes focused and clearly scoped.
3. Add or update tests when your change affects behavior.
4. Verify the relevant build and tests pass before submitting.
5. Open a pull request on GitHub against the `develop` branch.

## C/C++ Formatting

Use **clang-format 20.1.8**, matching the version pinned for CI:

```sh
python -m pip install -r tools/format-requirements.txt
python tools/check_format.py
```

The check covers whole C/C++ files changed since `HEAD` (staged and unstaged),
plus untracked files. To include committed changes on your branch, use:

```sh
python tools/check_format.py --base origin/develop
```

Pull requests run the same check against their target branch. Existing files
outside the change are not checked. Fix a reported file with
`clang-format -i path/to/file.cpp`, then rerun the check. Point your editor at the
same formatter version; pass `--clang-format /path/to/clang-format` to the check
if the executable is not on PATH.

`.clang-format-ignore` excludes vendored dependencies and generated/build
output. The `build/` directory also contains maintained sources, so it is not
excluded wholesale. This check targets C/C++ files; Objective-C and Objective-C++
files are not included. Keep include ordering and per-file pointer alignment as
configured, and avoid unrelated repository-wide formatting changes.

## Build and Package Checks

Every pull request to `develop` or `master`, and every push to them, runs the
**Build** workflow (`.github/workflows/build.yml`). For each supported Linux
distribution (Ubuntu 22.04 and 24.04, Debian 12 and 13) it:

1. builds SqlStudio in a clean container of that distribution, because each one
   ships different library versions (libpython, GTK, ...),
2. packages the result as a `.deb` with `build/linux/make_deb.sh`,
3. installs the package in another clean container and checks that every
   binary and library resolves its shared libraries,
4. checks the package with that distribution's `lintian` (errors fail),
5. starts the application on a virtual display (`build/linux/smoke_test.sh`)
   and fails if it crashes, shows no main window, or logs a Python traceback.

The packages and the smoke test results (screenshot and logs) are attached to
the workflow run as artifacts for 14 days. To try a package by hand, see
[testing/docker/linux-desktop](testing/docker/linux-desktop/README.md).

The third-party libraries (MySQL client, Connector/C++, GDAL, ANTLR runtime,
...) take a long time to build, so they are cached per distribution and
rebuilt only when `build/linux/list.txt`, `build/linux/build_3rdparty.sh` or
`build/linux/install_deps.sh` change. A pull request can use caches saved by
its own earlier runs or on its target branch, so the first run after such a
change takes longer.

A daily `Cleanup` workflow deletes the caches of closed pull requests and the
artifacts of older runs of each pull request, keeping only the newest one. It
can also be started by hand from the Actions tab; by default it then only
lists what it would delete.

## Pull Request Guidelines

- Describe the problem and the change clearly.
- Link the related issue when applicable.
- Include screenshots for UI changes when helpful.
- Avoid unrelated refactoring in the same pull request.

## Discussions and Support

Project discussion happens in [GitHub Discussions](https://github.com/dante-d4f/sqlstudio/discussions).
If you are unsure whether something is a bug, feature request, or design topic,
start there.

## License

By submitting a contribution, you agree that your work will be distributed under
the same license as this project.
