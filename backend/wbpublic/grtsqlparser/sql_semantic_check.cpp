/*
 * Copyright (c) 2007, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include "sql_semantic_check.h"

Sql_semantic_check::Sql_semantic_check() {
}

auto Sql_semantic_check::reset_context_objects() -> void {
  _context_schema = db_SchemaRef();
  _context_table = db_TableRef();
  _context_trigger = db_TriggerRef();
  _context_view = db_ViewRef();
  _context_routine = db_RoutineRef();
  _context_routine_group = db_RoutineGroupRef();
}

auto Sql_semantic_check::context_object(db_SchemaRef obj) -> void {
  _context_schema = obj;
}

auto Sql_semantic_check::context_object(db_TableRef obj) -> void {
  _context_table = obj;
}

auto Sql_semantic_check::context_object(db_TriggerRef obj) -> void {
  _context_trigger = obj;
}

auto Sql_semantic_check::context_object(db_ViewRef obj) -> void {
  _context_view = obj;
}

auto Sql_semantic_check::context_object(db_RoutineRef obj) -> void {
  _context_routine = obj;
}

auto Sql_semantic_check::context_object(db_RoutineGroupRef obj) -> void {
  _context_routine_group = obj;
}
