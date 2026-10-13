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

import { useEffect, useImperativeHandle, useRef, type Ref } from 'react';
import { useSqlHistory } from '../../hooks/useSqlHistory';
import { SqlSyntax } from './SqlSyntax';

export interface SqlPromptHandle { focus: () => void }
interface SqlPromptProps { ref?: Ref<SqlPromptHandle>; value: string; history: string[]; disabled: boolean; onChange: (value: string) => void }

export function SqlPrompt({ value, history, disabled, onChange, ref }: SqlPromptProps) {
  const textareaRef = useRef<HTMLTextAreaElement>(null);
  const highlightRef = useRef<HTMLPreElement>(null);
  const { edit, navigate } = useSqlHistory(history, value, onChange);
  const syncScroll = () => {
    if (highlightRef.current && textareaRef.current) {
      highlightRef.current.scrollTop = textareaRef.current.scrollTop;
      highlightRef.current.scrollLeft = textareaRef.current.scrollLeft;
    }
  };
  useImperativeHandle(ref, () => ({ focus: () => textareaRef.current?.focus() }), []);
  useEffect(() => {
    const textarea = textareaRef.current;
    if (textarea) { textarea.style.height = 'auto'; textarea.style.height = `${Math.max(42, Math.min(textarea.scrollHeight, 220))}px`; }
    syncScroll();
  }, [value]);
  return <div className={`prompt-editor ${disabled ? 'prompt-disabled' : ''}`}>
    <span className="sql-prompt-label">sql<span>&gt;</span></span>
    <div className="sql-editor-surface">
    <pre ref={highlightRef} className="sql-editor-highlight" aria-hidden="true"><SqlSyntax sql={value} />{'\n'}</pre>
    <textarea ref={textareaRef} aria-label="SQL command" autoCapitalize="off" autoComplete="off" autoCorrect="off"
      wrap="off" onScroll={syncScroll} aria-description="Up on the first line recalls older SQL. Down on the last line recalls newer SQL and restores your draft. Ctrl or Command plus Enter executes."
      disabled={disabled} placeholder={disabled ? 'Waiting for query to finish…' : 'Enter a SQL statement'} rows={1}
      spellCheck={false} value={value} onChange={(event) => edit(event.target.value)}
      onKeyDown={(event) => {
        navigate(event);
        if (event.key === 'Tab') {
          event.preventDefault(); const target = event.currentTarget; const start = target.selectionStart;
          const next = `${value.slice(0, start)}  ${value.slice(target.selectionEnd)}`;
          edit(next); requestAnimationFrame(() => target.setSelectionRange(start + 2, start + 2));
        }
      }} />
    </div>
  </div>;
}
