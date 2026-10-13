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

import { useEffect, useRef, type KeyboardEvent } from 'react';

export function useSqlHistory(history: string[], value: string, onChange: (sql: string) => void) {
  const position = useRef(history.length);
  const draft = useRef('');
  useEffect(() => { position.current = history.length; draft.current = ''; }, [history.length]);

  const edit = (sql: string) => {
    position.current = history.length;
    draft.current = sql;
    onChange(sql);
  };

  const navigate = (event: KeyboardEvent<HTMLTextAreaElement>) => {
    if (event.nativeEvent.isComposing || event.shiftKey || event.ctrlKey || event.metaKey || event.altKey) return;
    const up = event.key === 'ArrowUp';
    if (!up && event.key !== 'ArrowDown') return;
    const input = event.currentTarget;
    if (input.selectionStart !== input.selectionEnd) return;
    const caret = input.selectionStart;
    // Keep ordinary movement between lines in multiline SQL.
    if (up ? value.slice(0, caret).includes('\n') : value.slice(caret).includes('\n')) return;
    if (!history.length || (!up && position.current === history.length)) return;
    event.preventDefault();
    if (position.current === history.length) draft.current = value;
    position.current = Math.max(0, Math.min(history.length, position.current + (up ? -1 : 1)));
    const sql = position.current === history.length ? draft.current : history[position.current];
    onChange(sql);
    requestAnimationFrame(() => input.setSelectionRange(sql.length, sql.length));
  };
  return { edit, navigate };
}
