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

import unittest
from decimal import Decimal
from unittest.mock import Mock, patch

import pymysql
from fastapi import FastAPI
from fastapi.testclient import TestClient

from app.api.sql import create_sql_router
from app.database.config import DatabaseConnectionConfig
from app.database.session import SqlSession
from app.database.session_manager import SessionManager
from app.models.responses import cell_to_json
from app.services.sql_executor import execute_sql


class StreamingCursor:
    def __init__(self):
        self.description = [("value", 3)]
        self.warning_count = 0
        self.rowcount = 0
        self.unread = False
        self.position = 0
        self.total = 0

    def execute(self, sql):
        if self.unread:
            raise RuntimeError("Unread result found")
        if sql == "INVALID":
            raise pymysql.ProgrammingError(1064, "Invalid SQL")
        self.total = 1001 if sql == "LARGE" else 1
        self.position = 0
        self.unread = True

    def fetchmany(self, size):
        end = min(self.total, self.position + size)
        rows = [(i,) for i in range(self.position, end)]
        self.position = end
        if not rows:
            self.unread = False
        return rows

    def nextset(self):
        if self.unread:
            raise RuntimeError("Unread result found")
        return None

    def close(self):
        if self.unread:
            raise RuntimeError("Unread result found")


class SqlTests(unittest.TestCase):
    def setUp(self):
        self.cursor = StreamingCursor()
        self.connection = Mock(open=True)
        self.connection.cursor.return_value = self.cursor
        self.config = DatabaseConnectionConfig(result_limit=10)
        self.session = SqlSession("test", self.connection, self.config)

    def test_truncation_drains_result_and_next_query_works(self):
        result = execute_sql(self.session, "LARGE")
        self.assertEqual(result["returnedRows"], 10)
        self.assertTrue(result["truncated"])
        self.assertEqual(self.cursor.position, 1001)
        self.assertEqual(execute_sql(self.session, "SMALL")["rows"], [[0]])

    def test_exact_limit_is_not_truncated(self):
        self.session.config = DatabaseConnectionConfig(result_limit=1)
        self.assertFalse(execute_sql(self.session, "SMALL")["truncated"])

    def test_sql_error_preserves_connection(self):
        result = execute_sql(self.session, "INVALID")
        self.assertEqual(result["error"]["code"], 1064)
        self.assertEqual(execute_sql(self.session, "SMALL")["type"], "resultset")
        self.connection.close.assert_not_called()

    def test_cursor_creation_error_is_normalized(self):
        self.connection.cursor.side_effect = pymysql.OperationalError(2006, "Server gone away")
        self.assertEqual(execute_sql(self.session, "SELECT 1")["error"]["code"], 2006)

    def test_exact_values_and_binary(self):
        self.assertEqual(cell_to_json(Decimal("1234567890.123456789")), "1234567890.123456789")
        self.assertEqual(cell_to_json(2**63 - 1), str(2**63 - 1))
        self.assertEqual(cell_to_json(b"abc"), {"binary": True, "size": 3})
        self.assertIsNone(cell_to_json(None))
        self.assertEqual(cell_to_json(float("inf")), "inf")

    def test_multiple_results_and_commands(self):
        cursor = Mock(description=[("n", 3)], warning_count=2, rowcount=1)
        cursor.fetchmany.side_effect = [[(1,)], []]
        def advance():
            if cursor.description:
                cursor.description = None
                return True
            return None
        cursor.nextset.side_effect = advance
        self.connection.cursor.return_value = cursor
        result = execute_sql(self.session, "CALL demo()")
        self.assertEqual(result["type"], "batch")
        self.assertEqual([r["type"] for r in result["results"]], ["resultset", "command"])
        self.assertEqual(result["results"][0]["warningCount"], 2)

    def test_api_session_lifecycle_and_validation(self):
        manager = SessionManager(self.config)
        app = FastAPI()
        app.include_router(create_sql_router(manager))
        with patch.object(SqlSession, "connect", return_value=self.session), TestClient(app) as client:
            created = client.post("/api/sessions")
            self.assertEqual(created.status_code, 200)
            sid = created.json()["sessionId"]
            # Use the generated manager key: the fake session has a fixed id.
            key = next(iter(manager._sessions))
            self.assertEqual(sid, "test")
            endpoint = f"/api/sessions/{key}"
            self.assertEqual(client.post(endpoint + "/execute", json={"sql": "  "}).status_code, 400)
            self.assertEqual(client.post(endpoint + "/execute", json={"sql": "SMALL"}).json()["rows"], [[0]])
            self.assertEqual(client.delete(endpoint).status_code, 204)
            self.assertEqual(client.post(endpoint + "/execute", json={"sql": "SMALL"}).status_code, 404)
            self.connection.close.assert_called_once()

    def test_connection_failure_does_not_expose_password(self):
        manager = SessionManager(self.config)
        app = FastAPI()
        app.include_router(create_sql_router(manager))
        with patch.object(SqlSession, "connect", side_effect=RuntimeError("secret-password")), TestClient(app) as client:
            response = client.post("/api/sessions")
            self.assertEqual(response.status_code, 503)
            self.assertNotIn("secret-password", response.text)
        self.assertNotIn("secret-password", repr(DatabaseConnectionConfig(password="secret-password")))


if __name__ == "__main__":
    unittest.main()
