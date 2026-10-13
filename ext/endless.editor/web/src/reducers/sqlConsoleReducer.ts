/*
 * Copyright (c) 2026 dev4fun. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0,
 * as published by the Free Software Foundation.
 *
 * This program is designed to work with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms, as
 * designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an additional
 * permission to link the program and your derivative works with the
 * separately licensed software that they have either included with
 * the program or referenced in the documentation.
 * This program is distributed in the hope that it will be useful,  but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

import type { SqlExecution, SqlSession } from '../models/sql';

export interface SqlConsoleState {
  session: SqlSession | null; sessionStatus: 'connecting' | 'ready' | 'error'; sessionError: string | null;
  executions: SqlExecution[]; currentSql: string;
}
export type SqlConsoleAction =
  | { type: 'SESSION_CREATE_REQUEST' }
  | { type: 'SESSION_CREATE_SUCCESS'; session: SqlSession }
  | { type: 'SESSION_CREATE_FAILURE'; message: string }
  | { type: 'SQL_INPUT_CHANGED'; sql: string }
  | { type: 'EXECUTE_SQL_REQUEST'; execution: SqlExecution }
  | { type: 'EXECUTE_SQL_SUCCESS' | 'EXECUTE_SQL_FAILURE'; id: string; execution: SqlExecution }
  | { type: 'CLEAR_CONSOLE' };

export const initialSqlConsoleState: SqlConsoleState = {
  session: null, sessionStatus: 'connecting', sessionError: null, executions: [], currentSql: '',
};

export function sqlConsoleReducer(state: SqlConsoleState, action: SqlConsoleAction): SqlConsoleState {
  switch (action.type) {
    case 'SESSION_CREATE_REQUEST': return { ...state, sessionStatus: 'connecting', sessionError: null };
    case 'SESSION_CREATE_SUCCESS': return { ...state, session: action.session, sessionStatus: 'ready', sessionError: null };
    case 'SESSION_CREATE_FAILURE': return { ...state, sessionStatus: 'error', sessionError: action.message };
    case 'SQL_INPUT_CHANGED': return { ...state, currentSql: action.sql };
    case 'EXECUTE_SQL_REQUEST': return { ...state, currentSql: '', executions: [...state.executions, action.execution] };
    case 'EXECUTE_SQL_SUCCESS':
    case 'EXECUTE_SQL_FAILURE':
      return { ...state, executions: state.executions.map((item) => item.id === action.id ? action.execution : item) };
    case 'CLEAR_CONSOLE':
      if (state.executions.some((item) => item.status === 'pending')) return state;
      return { ...state, executions: [], currentSql: '' };
    default: return state;
  }
}