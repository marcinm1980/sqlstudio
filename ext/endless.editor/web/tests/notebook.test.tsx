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

import { StrictMode } from 'react';
import { act, cleanup, fireEvent, render, screen, waitFor } from '@testing-library/react';
import { afterEach, beforeEach, describe, expect, it, vi } from 'vitest';
import App from '../src/App';
import { SqlConsoleProvider } from '../src/context/SqlConsoleContext';
import { sqlApi } from '../src/api/sqlApi';
import type { SqlExecutionResponse } from '../src/models/sql';

vi.mock('../src/api/sqlApi', () => ({ sqlApi: {
  createSession: vi.fn(), execute: vi.fn(), closeSession: vi.fn(),
} }));
const session = { sessionId: 'one', database: 'sakila', host: 'localhost', port: 3306, type: 'mysql' as const };
const result: SqlExecutionResponse = { type: 'resultset', columns: [{ name: 'value' }], rows: [[null], [42]], returnedRows: 2, truncated: false, warningCount: 0, executionTimeMs: 12 };
const mount = () => render(<StrictMode><SqlConsoleProvider><App /></SqlConsoleProvider></StrictMode>);
const historyHas = (sql: string) => Array.from(document.querySelectorAll('.executed-prompt pre')).some((entry) => entry.textContent === sql);

beforeEach(() => {
  vi.clearAllMocks();
  vi.mocked(sqlApi.createSession).mockResolvedValue(session);
  vi.mocked(sqlApi.closeSession).mockResolvedValue(undefined);
  Element.prototype.scrollIntoView = vi.fn();
});
afterEach(cleanup);

describe('SQL notebook', () => {
  it('opens one session in StrictMode and closes it on unmount', async () => {
    const view = mount();
    await screen.findByText('CONNECTED');
    expect(sqlApi.createSession).toHaveBeenCalledTimes(1);
    expect(document.activeElement).toBe(screen.getByRole('textbox', { name: 'SQL command' }));
    view.unmount();
    await waitFor(() => expect(sqlApi.closeSession).toHaveBeenCalledWith('one'));
  });

  it('preserves history, shows a skeleton, blocks duplicates, and permits drafting the next query', async () => {
    let finish!: (result: SqlExecutionResponse) => void;
    vi.mocked(sqlApi.execute).mockImplementationOnce(() => new Promise((resolve) => { finish = resolve; }));
    mount();
    await screen.findByText('CONNECTED');
    const input = screen.getByRole('textbox') as HTMLTextAreaElement;
    fireEvent.change(input, { target: { value: 'SELECT 1' } });
    fireEvent.keyDown(input, { key: 'Enter', ctrlKey: true });
    expect(screen.getByRole('status', { name: 'Query result loading' })).toBeTruthy();
    expect(input.value).toBe('');
    expect(input.disabled).toBe(false);
    fireEvent.change(input, { target: { value: 'SELECT 2' } });
    fireEvent.keyDown(input, { key: 'Enter', metaKey: true });
    fireEvent.click(screen.getByRole('button', { name: 'Clear console' }));
    expect(sqlApi.execute).toHaveBeenCalledTimes(1);
    expect(historyHas('SELECT 1')).toBe(true);
    await act(async () => finish(result));
    expect(screen.getByText('NULL')).toBeTruthy();
    expect(screen.getByText('42')).toBeTruthy();
    expect(input.value).toBe('SELECT 2');
    vi.mocked(sqlApi.execute).mockResolvedValueOnce({ type: 'error', error: { message: 'Table does not exist', code: 1146, executionTimeMs: 1 } });
    fireEvent.keyDown(input, { key: 'Enter', metaKey: true });
    await screen.findByText('Table does not exist');
    expect(historyHas('SELECT 1')).toBe(true);
    expect(historyHas('SELECT 2')).toBe(true);
    expect(screen.getAllByRole('textbox')).toHaveLength(1);
    expect(document.activeElement).toBe(input);
    vi.mocked(sqlApi.execute).mockResolvedValueOnce(result);
    fireEvent.change(input, { target: { value: 'SELECT 3' } });
    fireEvent.keyDown(input, { key: 'Enter', ctrlKey: true });
    await waitFor(() => expect(screen.getAllByText('42')).toHaveLength(2));
    expect(sqlApi.execute).toHaveBeenLastCalledWith('one', 'SELECT 3');
  });

  it('retries a failed connection and ignores blank input', async () => {
    vi.mocked(sqlApi.createSession).mockRejectedValueOnce(new Error('Connection refused'));
    mount();
    await screen.findByText('Connection refused');
    fireEvent.click(screen.getByRole('button', { name: 'Reconnect' }));
    await screen.findByText('CONNECTED');
    fireEvent.change(screen.getByRole('textbox'), { target: { value: '   ' } });
    fireEvent.keyDown(screen.getByRole('textbox'), { key: 'Enter', ctrlKey: true });
    expect(sqlApi.execute).not.toHaveBeenCalled();
  });
});
