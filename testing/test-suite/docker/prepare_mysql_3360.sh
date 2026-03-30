#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
DOCKER_DIR="${ROOT_DIR}/testing/test-suite/docker/mysql"

CONTAINER_NAME="${WB_TEST_DB_CONTAINER:-mysqlstudio-test-mysql}"
IMAGE_NAME="${WB_TEST_DB_IMAGE:-mysqlstudio-test-mysql:local}"
HOST_PORT="${WB_TEST_DB_PORT:-3306}"
CONTAINER_PORT=3306

# These align with testing/test-suite/core/wb_connection_helpers.cpp defaults.
DB_HOST="${DB_HOST:-localhost}"
DB_PORT="${DB_PORT:-${HOST_PORT}}"
DB_USER="${DB_USER:-root}"
DB_PASSWORD="${DB_PASSWORD:-}"

echo "[INFO] Building image: ${IMAGE_NAME}"
docker build -t "${IMAGE_NAME}" "${DOCKER_DIR}" >/dev/null

if docker ps -a --format '{{.Names}}' | grep -qx "${CONTAINER_NAME}"; then
  echo "[INFO] Removing existing container: ${CONTAINER_NAME}"
  docker rm -f "${CONTAINER_NAME}" >/dev/null
fi

echo "[INFO] Starting MySQL on host port ${HOST_PORT} ..."

if [[ -z "${DB_PASSWORD}" ]]; then
  docker run -d \
    --name "${CONTAINER_NAME}" \
    -e MYSQL_ALLOW_EMPTY_PASSWORD=yes \
    -p "${HOST_PORT}:${CONTAINER_PORT}" \
    "${IMAGE_NAME}" >/dev/null
  ROOT_PASSWORD=""
else
  docker run -d \
    --name "${CONTAINER_NAME}" \
    -e MYSQL_ROOT_PASSWORD="${DB_PASSWORD}" \
    -p "${HOST_PORT}:${CONTAINER_PORT}" \
    "${IMAGE_NAME}" >/dev/null
  ROOT_PASSWORD="${DB_PASSWORD}"
fi

echo "[INFO] Waiting for MySQL readiness ..."
for i in {1..90}; do
  if [[ -z "${ROOT_PASSWORD}" ]]; then
    if docker exec "${CONTAINER_NAME}" mysqladmin ping -uroot --silent >/dev/null 2>&1; then
      break
    fi
  else
    if docker exec "${CONTAINER_NAME}" mysqladmin ping -uroot "-p${ROOT_PASSWORD}" --silent >/dev/null 2>&1; then
      break
    fi
  fi

  sleep 1
  if [[ "${i}" -eq 90 ]]; then
    echo "[ERROR] MySQL was not ready in time."
    docker logs "${CONTAINER_NAME}" | tail -n 200
    exit 1
  fi
done

echo "[OK] MySQL is ready on localhost:${HOST_PORT}"

cat <<EOF

[INFO] Credentials check (from test defaults + env)
  Expected by tests: testing/test-suite/core/wb_connection_helpers.cpp
  DB_HOST=${DB_HOST}
  DB_PORT=${DB_PORT}
  DB_USER=${DB_USER}
  DB_PASSWORD=${DB_PASSWORD}

[INFO] Export these before running tests:
  export DB_HOST=${DB_HOST}
  export DB_PORT=${DB_PORT}
  export DB_USER=${DB_USER}
  export DB_PASSWORD='${DB_PASSWORD}'

[INFO] Container:
  name=${CONTAINER_NAME}
  image=${IMAGE_NAME}
EOF
