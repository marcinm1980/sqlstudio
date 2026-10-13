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

import type { SqlCell, SqlResultSet } from '../../models/sql';

function displayCell(value: SqlCell): { text: string; kind: string } {
  if (value === null) return { text: 'NULL', kind: 'null' };
  if (typeof value === 'object') return { text: `[BINARY · ${value.size} bytes]`, kind: 'binary' };
  return { text: String(value), kind: typeof value };
}

export function SqlResultTable({ result }: { result: SqlResultSet }) {
  return <div className="result-grid-wrap"><table className="result-grid"><thead><tr>
    <th className="row-number">#</th>{result.columns.map((column, index) => <th key={`${column.name}-${index}`} title={column.type ? `${column.name} · ${column.type}` : column.name}>
      <span>{column.name}</span>{column.type && <small>{column.type}</small>}
    </th>)}
  </tr></thead><tbody>{result.rows.map((row, rowIndex) => <tr key={rowIndex}>
    <td className="row-number">{rowIndex + 1}</td>{row.map((value, columnIndex) => {
      const cell = displayCell(value);
      return <td key={columnIndex} className={`cell-${cell.kind}`} title={cell.text}>{cell.text}</td>;
    })}
  </tr>)}</tbody></table>{result.rows.length === 0 && <div className="empty-result">The query returned no rows.</div>}</div>;
}