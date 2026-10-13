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

import { useState } from 'react';
import { cleanup, fireEvent, render, screen } from '@testing-library/react';
import { afterEach, describe, expect, it } from 'vitest';
import { SqlPrompt } from '../src/components/sql-console/SqlPrompt';
import { SqlSyntax } from '../src/components/sql-console/SqlSyntax';

afterEach(cleanup);
function Prompt({ history = ['SELECT 1', 'SELECT 2'], initial = 'SELECT draft' }: { history?: string[]; initial?: string }) {
  const [value, setValue] = useState(initial);
  return <SqlPrompt history={history} value={value} onChange={setValue} disabled={false} />;
}
const input = () => screen.getByRole('textbox') as HTMLTextAreaElement;
const arrow = (key: string, caret = input().value.length) => {
  input().setSelectionRange(caret, caret);
  fireEvent.keyDown(input(), { key });
};

describe('SQL editing', () => {
  it('recalls older/newer commands, stops at oldest, and restores the draft', () => {
    render(<Prompt />);
    arrow('ArrowUp'); expect(input().value).toBe('SELECT 2');
    arrow('ArrowUp'); expect(input().value).toBe('SELECT 1');
    arrow('ArrowUp'); expect(input().value).toBe('SELECT 1');
    arrow('ArrowDown'); expect(input().value).toBe('SELECT 2');
    arrow('ArrowDown'); expect(input().value).toBe('SELECT draft');
    arrow('ArrowDown'); expect(input().value).toBe('SELECT draft');
  });

  it('preserves multiline navigation and selections', () => {
    render(<Prompt initial={'SELECT\n  10\nFROM example'} />);
    arrow('ArrowUp'); expect(input().value).toContain('FROM example');
    arrow('ArrowDown', 0); expect(input().value).toContain('FROM example');
    input().setSelectionRange(0, 6);
    fireEvent.keyDown(input(), { key: 'ArrowUp' });
    expect(input().value).toContain('FROM example');
    arrow('ArrowUp', 0); expect(input().value).toBe('SELECT 2');
    arrow('ArrowDown'); expect(input().value).toBe('SELECT\n  10\nFROM example');
  });

  it('treats edits to recalled SQL as a new draft without changing history', () => {
    render(<Prompt />);
    arrow('ArrowUp');
    fireEvent.change(input(), { target: { value: 'SELECT 20' } });
    arrow('ArrowUp'); expect(input().value).toBe('SELECT 2');
    arrow('ArrowDown'); expect(input().value).toBe('SELECT 20');
  });

  it('resets navigation when history is cleared or a command is executed', () => {
    const view = render(<Prompt />);
    arrow('ArrowUp');
    view.rerender(<Prompt history={[]} />);
    arrow('ArrowUp'); expect(input().value).toBe('SELECT 2');
    view.rerender(<Prompt history={['SELECT 3']} />);
    arrow('ArrowUp'); expect(input().value).toBe('SELECT 3');
    arrow('ArrowDown'); expect(input().value).toBe('SELECT 2');
  });

  it('keeps SQL text intact while highlighting tokens and escaping HTML', () => {
    const sql = "select `field`, @foo, 12.5, 'it''s <script>bad</script>' FROM t -- comment\n/* block */";
    const { container } = render(<pre><SqlSyntax sql={sql} /></pre>);
    expect(container.textContent).toBe(sql);
    expect(container.querySelector('script')).toBeNull();
    for (const kind of ['keyword', 'identifier', 'variable', 'number', 'string', 'comment']) {
      expect(container.querySelector('.sql-token-' + kind)).not.toBeNull();
    }
  });
});
