# Linux desktop for testing CI packages

A disposable Linux desktop, reachable from a browser, for trying the `.deb`
packages that the **Build** workflow produces. It runs on any machine with
Docker, including Apple Silicon Macs (the amd64 image runs through Rosetta in
OrbStack, or through QEMU in Docker Desktop, which is much slower).

The package is installed when the container starts, so each start is also a
fresh "install on a clean system" check.

## Usage

1. Start the test databases (they provide the `sqlstudio-databases` network):

   ```sh
   cd testing/docker/databases
   docker compose up -d mysql-8-2
   ```

2. Download a package from a CI run into `packages/` (exactly one `.deb`):

   ```sh
   cd testing/docker/linux-desktop
   gh run download <run-id> --name sql-studio-ubuntu22.04-<sha> --dir packages
   ```

3. Start the desktop:

   ```sh
   docker compose up --build
   ```

4. Open <http://localhost:6080/vnc.html?autoconnect=true&resize=scale>.

Connect to the databases by service name, e.g. host `mysql-8-2`, port `3306`,
user `sqlstudio`, password `sqlstudio` (see `../databases/README.md`).

Closing SqlStudio leaves the desktop running; start it again from the terminal
with `sql-studio`. Its output is shown by `docker compose logs -f` and kept in
`/var/log/sql-studio.log`.

## Other distributions

Use the package and base image of the same distribution:

```sh
BASE_IMAGE=ubuntu:24.04 DISTRO_TAG=ubuntu24.04 docker compose up --build
BASE_IMAGE=debian:12    DISTRO_TAG=debian12    docker compose up --build
```

## Clean up

```sh
docker compose down -v   # -v also removes the saved SqlStudio settings
```
