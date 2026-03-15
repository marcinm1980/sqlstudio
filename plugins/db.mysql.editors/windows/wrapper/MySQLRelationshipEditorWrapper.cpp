/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include "MySQLRelationshipEditorWrapper.h"

#include "ConvUtils.h"

using namespace MySQL::Grt::Db;

//--------------------------------------------------------------------------------------------------

MySQLRelationshipEditorWrapper::MySQLRelationshipEditorWrapper(MySQL::Grt::GrtValue ^ arglist)
  : BaseEditorWrapper(new RelationshipEditorBE(studio_physical_ConnectionRef::cast_from(
      grt::BaseListRef::cast_from(arglist->get_unmanaged_object()).get(0)))) {
}

//--------------------------------------------------------------------------------------------------

MySQLRelationshipEditorWrapper::~MySQLRelationshipEditorWrapper() {
  delete inner; // We created it.
}

//--------------------------------------------------------------------------------------------------

auto MySQLRelationshipEditorWrapper::get_unmanaged_object() -> RelationshipEditorBE * {
  return static_cast<::RelationshipEditorBE *>(inner);
}

//--------------------------------------------------------------------------------------------------

void MySQLRelationshipEditorWrapper::set_caption(String ^ caption) {
  get_unmanaged_object()->set_caption(NativeToCppString(caption));
}

//--------------------------------------------------------------------------------------------------

auto MySQLRelationshipEditorWrapper::get_caption() -> String ^ {
  return CppStringToNative(get_unmanaged_object()->get_caption());
}

//--------------------------------------------------------------------------------------------------

auto MySQLRelationshipEditorWrapper::get_caption_long() -> String ^ {
  return CppStringToNative(get_unmanaged_object()->get_caption_long());
}

//--------------------------------------------------------------------------------------------------

void MySQLRelationshipEditorWrapper::set_extra_caption(String ^ caption) {
  get_unmanaged_object()->set_extra_caption(NativeToCppString(caption));
}

//--------------------------------------------------------------------------------------------------

auto MySQLRelationshipEditorWrapper::get_extra_caption() -> String ^ {
  return CppStringToNative(get_unmanaged_object()->get_extra_caption());
}

//--------------------------------------------------------------------------------------------------

auto MySQLRelationshipEditorWrapper::get_extra_caption_long() -> String ^ {
  return CppStringToNative(get_unmanaged_object()->get_extra_caption_long());
}

//--------------------------------------------------------------------------------------------------

auto MySQLRelationshipEditorWrapper::get_right_table_name() -> String ^ {
  return CppStringToNative(get_unmanaged_object()->get_right_table_name());
}

//--------------------------------------------------------------------------------------------------

auto MySQLRelationshipEditorWrapper::get_left_table_name() -> String ^ {
  return CppStringToNative(get_unmanaged_object()->get_left_table_name());
}

//--------------------------------------------------------------------------------------------------

auto MySQLRelationshipEditorWrapper::get_right_table_info() -> String ^ {
  return CppStringToNative(get_unmanaged_object()->get_right_table_info());
}

//--------------------------------------------------------------------------------------------------

auto MySQLRelationshipEditorWrapper::get_left_table_info() -> String ^ {
  return CppStringToNative(get_unmanaged_object()->get_left_table_info());
}

//--------------------------------------------------------------------------------------------------

auto MySQLRelationshipEditorWrapper::get_left_table_fk() -> String ^ {
  return CppStringToNative(get_unmanaged_object()->get_left_table_fk());
}

//--------------------------------------------------------------------------------------------------

auto MySQLRelationshipEditorWrapper::set_left_mandatory(bool flag) -> void {
  get_unmanaged_object()->set_left_mandatory(flag);
}

//--------------------------------------------------------------------------------------------------

auto MySQLRelationshipEditorWrapper::get_left_mandatory() -> bool {
  return get_unmanaged_object()->get_left_mandatory();
}

//--------------------------------------------------------------------------------------------------

auto MySQLRelationshipEditorWrapper::set_right_mandatory(bool flag) -> void {
  get_unmanaged_object()->set_right_mandatory(flag);
}

//--------------------------------------------------------------------------------------------------

auto MySQLRelationshipEditorWrapper::get_right_mandatory() -> bool {
  return get_unmanaged_object()->get_right_mandatory();
}

//--------------------------------------------------------------------------------------------------

auto MySQLRelationshipEditorWrapper::set_to_many(bool flag) -> void {
  get_unmanaged_object()->set_to_many(flag);
}

//--------------------------------------------------------------------------------------------------

auto MySQLRelationshipEditorWrapper::get_to_many() -> bool {
  return get_unmanaged_object()->get_to_many();
}

//--------------------------------------------------------------------------------------------------

void MySQLRelationshipEditorWrapper::set_comment(String ^ comment) {
  get_unmanaged_object()->set_comment(NativeToCppString(comment));
}

//--------------------------------------------------------------------------------------------------

auto MySQLRelationshipEditorWrapper::get_comment() -> String ^ {
  return CppStringToNative(get_unmanaged_object()->get_comment());
}

//--------------------------------------------------------------------------------------------------

auto MySQLRelationshipEditorWrapper::get_visibility() -> RelationshipVisibilityType {
  return (RelationshipVisibilityType)get_unmanaged_object()->get_visibility();
}

//--------------------------------------------------------------------------------------------------

auto MySQLRelationshipEditorWrapper::get_is_identifying() -> bool {
  return get_unmanaged_object()->get_is_identifying();
}

//--------------------------------------------------------------------------------------------------

auto MySQLRelationshipEditorWrapper::set_visibility(RelationshipVisibilityType v) -> void {
  get_unmanaged_object()->set_visibility((RelationshipEditorBE::VisibilityType)v);
}

//--------------------------------------------------------------------------------------------------

auto MySQLRelationshipEditorWrapper::open_editor_for_left_table() -> void {
  get_unmanaged_object()->open_editor_for_left_table();
}

//--------------------------------------------------------------------------------------------------

auto MySQLRelationshipEditorWrapper::open_editor_for_right_table() -> void {
  get_unmanaged_object()->open_editor_for_right_table();
}

//--------------------------------------------------------------------------------------------------

auto MySQLRelationshipEditorWrapper::set_is_identifying(bool identifying) -> void {
  get_unmanaged_object()->set_is_identifying(identifying);
}

//--------------------------------------------------------------------------------------------------
