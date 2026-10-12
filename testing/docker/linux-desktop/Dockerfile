# Disposable Linux desktop for trying a SqlStudio .deb built by CI.
# The package itself is installed at container start (see entrypoint.sh), so
# a newer package only needs a container restart, not an image rebuild.
ARG BASE_IMAGE=ubuntu:22.04
FROM ${BASE_IMAGE}

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
 && apt-get install -y --no-install-recommends \
      xvfb openbox x11vnc novnc websockify xterm x11-utils xdotool \
      fonts-dejavu-core fonts-liberation \
      libgl1-mesa-dri \
      dbus-x11 gnome-keyring \
      ca-certificates procps \
 && rm -rf /var/lib/apt/lists/*

COPY entrypoint.sh /usr/local/bin/entrypoint.sh

ENV DISPLAY=:1 \
    SCREEN_GEOMETRY=1600x1000x24

EXPOSE 6080
ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
