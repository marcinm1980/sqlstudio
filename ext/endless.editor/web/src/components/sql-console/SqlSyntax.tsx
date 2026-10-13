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

import { memo } from 'react';

const keywords = new Set(('SELECT FROM WHERE JOIN INNER LEFT RIGHT FULL OUTER CROSS ON AS AND OR NOT NULL IS IN EXISTS BETWEEN LIKE REGEXP LIMIT OFFSET ORDER BY GROUP HAVING DISTINCT ALL UNION WITH RECURSIVE INSERT INTO VALUES UPDATE SET DELETE CREATE ALTER DROP TRUNCATE TABLE DATABASE SCHEMA INDEX VIEW IF DEFAULT PRIMARY FOREIGN KEY REFERENCES UNIQUE AUTO_INCREMENT USE SHOW DESCRIBE EXPLAIN BEGIN START TRANSACTION COMMIT ROLLBACK SAVEPOINT RELEASE END CASE WHEN THEN ELSE ASC DESC TRUE FALSE UNKNOWN GRANT REVOKE CALL PROCEDURE FUNCTION RETURN RETURNS DECLARE WHILE DO LOOP INT INTEGER BIGINT VARCHAR CHAR TEXT DATE DATETIME TIMESTAMP DECIMAL FLOAT DOUBLE BOOLEAN BINARY BLOB UNSIGNED CONSTRAINT ENGINE COUNT SUM AVG MIN MAX COALESCE CAST CONVERT NOW CURRENT_TIMESTAMP').split(' '));

// Presentation only: highlight incomplete SQL and let React escape every token.
const tokens = /(--(?=\s|$)[^\r\n]*|#[^\r\n]*|\/\*[\s\S]*?(?:\*\/|$)|'(?:\\[\s\S]|''|[^'\\])*(?:'|$)|"(?:\\[\s\S]|""|[^"\\])*(?:"|$)|`(?:``|[^`])*(?:`|$)|@@?[a-zA-Z_][\w$]*(?:\.[\w$]+)?|\b(?:0x[\da-fA-F]+|\d+(?:\.\d*)?(?:e[+-]?\d+)?)\b|\b[a-zA-Z_][\w$]*\b|[<>=!:+*/%|&^~-]+)/gi;

export const SqlSyntax = memo(function SqlSyntax({ sql }: { sql: string }) {
  return <>{sql.split(tokens).map((text, index) => {
    let kind = '';
    if (index % 2) {
      if (/^(--|#|\/\*)/.test(text)) kind = 'comment';
      else if (/^['"]/.test(text)) kind = 'string';
      else if (text.charCodeAt(0) === 96) kind = 'identifier';
      else if (text.startsWith('@')) kind = 'variable';
      else if (/^\d/.test(text)) kind = 'number';
      else if (keywords.has(text.toUpperCase())) kind = 'keyword';
      else if (/^[<>=!:+*/%|&^~-]/.test(text)) kind = 'operator';
    }
    return kind ? <span className={'sql-token-' + kind} key={index}>{text}</span> : text;
  })}</>;
});
