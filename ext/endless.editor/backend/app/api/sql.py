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

from fastapi import APIRouter, HTTPException, Response
from starlette.concurrency import run_in_threadpool

from app.database.session_manager import SessionManager
from app.models.responses import SqlExecuteRequest, SqlSessionResponse
from app.services.sql_executor import execute_sql


def create_sql_router(session_manager: SessionManager) -> APIRouter:
    router = APIRouter(prefix="/api/sessions", tags=["sql"])

    @router.post("", response_model=SqlSessionResponse)
    async def create_session() -> dict[str, object]:
        try:
            session = await run_in_threadpool(session_manager.create)
        except Exception as error:
            raise HTTPException(status_code=503, detail="Could not open the configured database session. Check the server and backend configuration.") from error
        return {"sessionId": session.session_id, "database": session.config.database,
                "host": session.config.host, "port": session.config.port, "type": session.config.type}

    @router.delete("/{session_id}", status_code=204)
    async def close_session(session_id: str) -> Response:
        await run_in_threadpool(session_manager.close, session_id)
        return Response(status_code=204)

    @router.post("/{session_id}/execute")
    async def execute(session_id: str, request: SqlExecuteRequest) -> dict[str, object]:
        session = session_manager.get(session_id)
        if session is None:
            raise HTTPException(status_code=404, detail="SQL session not found. Reload to start a new session.")
        if not request.sql.strip():
            raise HTTPException(status_code=400, detail="SQL statement must not be empty.")
        return await run_in_threadpool(execute_sql, session, request.sql)

    return router
