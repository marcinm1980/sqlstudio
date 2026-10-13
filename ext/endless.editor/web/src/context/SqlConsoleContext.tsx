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

import { createContext, useCallback, useEffect, useMemo, useReducer, useRef, type Dispatch, type ReactNode } from 'react';
import { closeSqlSession, createSqlSession, executeSql } from '../actions/sqlConsoleActions';
import type { SqlSession } from '../models/sql';
import { initialSqlConsoleState, sqlConsoleReducer, type SqlConsoleAction, type SqlConsoleState } from '../reducers/sqlConsoleReducer';

interface SqlConsoleContextValue {
  state: SqlConsoleState;
  dispatch: Dispatch<SqlConsoleAction>;
  execute: () => Promise<void>;
  retryConnection: () => void;
}
export const SqlConsoleContext = createContext<SqlConsoleContextValue | null>(null);

export function SqlConsoleProvider({ children }: { children: ReactNode }) {
  const [state, dispatch] = useReducer(sqlConsoleReducer, initialSqlConsoleState);
  const connecting = useRef<Promise<SqlSession | null> | null>(null);
  const mounted = useRef(false);
  const executing = useRef(false);
  const disposal = useRef<ReturnType<typeof setTimeout> | undefined>(undefined);

  const retryConnection = useCallback(() => {
    if (connecting.current) return;
    connecting.current = createSqlSession((action) => {
      if (mounted.current) dispatch(action);
    });
    void connecting.current.then((session) => {
      if (!session) connecting.current = null;
    });
  }, []);

  useEffect(() => {
    mounted.current = true;
    clearTimeout(disposal.current);
    retryConnection();
    const close = () => { void connecting.current?.then((session) => session && closeSqlSession(session)); };
    window.addEventListener('pagehide', close);
    return () => {
      mounted.current = false;
      window.removeEventListener('pagehide', close);
      // Reuse the connection during StrictMode's immediate effect replay.
      // Dispose real unmounts, even if session creation is still pending.
      disposal.current = setTimeout(close, 0);
    };
  }, [retryConnection]);

  const execute = useCallback(async () => {
    // Guard calls occurring before React has rendered the pending state.
    if (executing.current) return;
    executing.current = true;
    try { await executeSql(state, dispatch); }
    finally { executing.current = false; }
  }, [state]);

  const value = useMemo(() => ({ state, dispatch, execute, retryConnection }), [state, execute, retryConnection]);
  return <SqlConsoleContext.Provider value={value}>{children}</SqlConsoleContext.Provider>;
}
