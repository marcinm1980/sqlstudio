# Copyright (c) 2026 dev4fun. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 2.0,
# as published by the Free Software Foundation.
#
# This program is designed to work with certain software (including
# but not limited to OpenSSL) that is licensed under separate terms, as
# designated in a particular file or component or in included license
# documentation.  The authors of MySQL hereby grant you an additional
# permission to link the program and your derivative works with the
# separately licensed software that they have either included with
# the program or referenced in the documentation.
# This program is distributed in the hope that it will be useful,  but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
# the GNU General Public License, version 2.0, for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

from contextlib import asynccontextmanager

from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware

from app.api.sql import create_sql_router
from app.database.config import DatabaseConnectionConfig
from app.database.session_manager import SessionManager

config = DatabaseConnectionConfig.from_environment()
session_manager = SessionManager(config)


@asynccontextmanager
async def lifespan(_: FastAPI):
    yield
    session_manager.close_all()


app = FastAPI(title="Endless Control SQL API", version="1.0.0", lifespan=lifespan)
app.add_middleware(CORSMiddleware, allow_origins=["http://localhost:5173", "http://127.0.0.1:5173", "https://appassets.example"],
                   allow_methods=["GET", "POST", "DELETE", "OPTIONS"], allow_headers=["Content-Type"])
app.include_router(create_sql_router(session_manager))


@app.get("/api/health")
async def health() -> dict[str, str]:
    return {"status": "ok"}