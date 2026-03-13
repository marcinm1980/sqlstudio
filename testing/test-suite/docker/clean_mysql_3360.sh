#!/usr/bin/env bash
set -euo pipefail

CONTAINER_NAME="${WB_TEST_DB_CONTAINER:-mysqlstudio-test-mysql}"
IMAGE_NAME="${WB_TEST_DB_IMAGE:-mysqlstudio-test-mysql:local}"

if docker ps -a --format '{{.Names}}' | grep -qx "${CONTAINER_NAME}"; then
  echo "[INFO] Removing container: ${CONTAINER_NAME}"
  docker rm -f "${CONTAINER_NAME}" >/dev/null
else
  echo "[INFO] Container not found: ${CONTAINER_NAME}"
fi

if [[ "${1:-}" == "--remove-image" ]]; then
  if docker images --format '{{.Repository}}:{{.Tag}}' | grep -qx "${IMAGE_NAME}"; then
    echo "[INFO] Removing image: ${IMAGE_NAME}"
    docker rmi "${IMAGE_NAME}" >/dev/null
  else
    echo "[INFO] Image not found: ${IMAGE_NAME}"
  fi
fi

echo "[OK] Cleanup done."
