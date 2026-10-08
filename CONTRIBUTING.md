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
