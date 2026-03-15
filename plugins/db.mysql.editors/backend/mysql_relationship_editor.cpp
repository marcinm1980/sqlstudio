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

#include "base/string_utilities.h"
#include "base/util_functions.h"
#include "mysql_relationship_editor.h"

using namespace bec;
using namespace base;

RelationshipEditorBE::RelationshipEditorBE(const studio_physical_ConnectionRef &relationship)
  : BaseEditor(relationship), _relationship(relationship) {
}

auto RelationshipEditorBE::should_close_on_delete_of(const std::string &oid) -> bool {
  if (_relationship.id() == oid)
    return true;

  if (_relationship->owner().id() == oid) // diagram deleted
    return true;

  if (!_relationship->foreignKey().is_valid() || _relationship->foreignKey().id() == oid)
    return true;

  db_TableRef table(db_TableRef::cast_from(_relationship->foreignKey()->owner()));
  if (!table.is_valid() || table.id() == oid)
    return true;

  db_SchemaRef schema(db_SchemaRef::cast_from(table->owner()));
  if (!schema.is_valid() || schema.id() == oid)
    return true;

  return false;
}

auto RelationshipEditorBE::set_model_only(bool flag) -> void {
  if (flag != model_only()) {
    AutoUndoEdit undo(this, _relationship, "caption");
    _relationship->foreignKey()->modelOnly(flag);
    undo.end(_("Change Relationship Caption"));
  }
}

auto RelationshipEditorBE::set_caption(const std::string &caption) -> void {
  if (*_relationship->caption() != caption) {
    AutoUndoEdit undo(this, _relationship, "caption");
    _relationship->caption(caption);
    undo.end(_("Change Relationship Caption"));
  }
}

auto RelationshipEditorBE::get_caption() -> std::string {
  return _relationship->caption();
}

auto RelationshipEditorBE::get_caption_long() -> std::string {
  return strfmt("'%s'  (%s)  '%s'", get_left_table_name().c_str(), get_caption().c_str(),
                get_right_table_name().c_str());
}

auto RelationshipEditorBE::set_extra_caption(const std::string &caption) -> void {
  if (*_relationship->extraCaption() != caption) {
    AutoUndoEdit undo(this, _relationship, "extraCaption");
    _relationship->extraCaption(caption);
    undo.end(_("Change Relationship 2nd Caption"));
  }
}

auto RelationshipEditorBE::get_extra_caption() -> std::string {
  return _relationship->extraCaption();
}

auto RelationshipEditorBE::get_extra_caption_long() -> std::string {
  return strfmt("'%s' (%s) '%s'", get_right_table_name().c_str(), get_extra_caption().c_str(),
                get_left_table_name().c_str());
}

auto RelationshipEditorBE::set_left_mandatory(bool flag) -> void {
  if (flag != (*_relationship->foreignKey()->mandatory() == 1)) {
    AutoUndoEdit undo(this);
    _relationship->foreignKey()->mandatory(flag ? 1 : 0);

    undo.end(_("Change Mandatory"));
  }
}

auto RelationshipEditorBE::get_left_mandatory() -> bool {
  if (_relationship->foreignKey().is_valid())
    return _relationship->foreignKey()->mandatory() != 0;

  return false;
}

auto RelationshipEditorBE::set_right_mandatory(bool flag) -> void {
  if (flag != (*_relationship->foreignKey()->referencedMandatory() == 1)) {
    AutoUndoEdit undo(this);
    _relationship->foreignKey()->referencedMandatory(flag ? 1 : 0);
    GRTLIST_FOREACH(db_Column, _relationship->foreignKey()->columns(), column)
    (*column)->isNotNull(flag);

    db_TableRef table(db_TableRef::cast_from(_relationship->foreignKey()->owner()));

    table.set_member("lastChangeDate", grt::StringRef(fmttime(0, DATETIME_FMT)));
    (*table->signal_refreshDisplay())("column");

    undo.end(_("Change Referred Mandatory"));
  }
}

auto RelationshipEditorBE::get_right_mandatory() -> bool {
  if (_relationship->foreignKey().is_valid())
    return _relationship->foreignKey()->referencedMandatory() != 0;

  return false;
}

auto RelationshipEditorBE::set_to_many(bool flag) -> void {
  if (flag != (*_relationship->foreignKey()->many() == 1)) {
    AutoUndoEdit undo(this);
    _relationship->foreignKey()->many(flag ? 1 : 0);
    undo.end(_("Change Relationship Cardinality"));
  }
}

auto RelationshipEditorBE::get_to_many() -> bool {
  if (_relationship->foreignKey().is_valid())
    return _relationship->foreignKey()->many() != 0;

  return false;
}

auto RelationshipEditorBE::set_comment(const std::string &comment) -> void {
  if (comment != *_relationship->comment()) {
    AutoUndoEdit undo(this, _relationship, "comment");
    _relationship->comment(comment);
    undo.end(_("Change Relationship Comment"));
  }
}

auto RelationshipEditorBE::get_comment() -> std::string {
  return _relationship->comment();
}

auto RelationshipEditorBE::get_visibility() -> RelationshipEditorBE::VisibilityType {
  if (*_relationship->drawSplit() && *_relationship->visible())
    return Splitted;
  else if (!*_relationship->drawSplit() && *_relationship->visible())
    return Visible;
  else
    return Hidden;
}

auto RelationshipEditorBE::set_visibility(VisibilityType type) -> void {
  if (get_visibility() == type)
    return;

  AutoUndoEdit undo(this);

  switch (type) {
    case Visible:
      _relationship->visible(1);
      _relationship->drawSplit(0);
      break;
    case Splitted:
      _relationship->visible(1);
      _relationship->drawSplit(1);
      break;
    case Hidden:
      _relationship->visible(0);
      break;
  }

  undo.end(_("Change Relationship Visibility"));
}

auto RelationshipEditorBE::get_left_table_name() -> std::string {
  if (_relationship->foreignKey().is_valid())
    return *_relationship->foreignKey()->owner()->name();
  return std::string();
}

auto RelationshipEditorBE::get_right_table_name() -> std::string {
  if (_relationship->foreignKey().is_valid())
    return *_relationship->foreignKey()->referencedTable()->name();
  return std::string();
}

auto RelationshipEditorBE::get_left_table_info() -> std::string {
  std::string text;
  db_ForeignKeyRef fk(_relationship->foreignKey());
  if (fk.is_valid())
    for (size_t c = fk->columns().count(), i = 0; i < c; i++) {
      text.append(strfmt("%s: %s%s\n", fk->columns()[i]->name().c_str(), fk->columns()[i]->formattedRawType().c_str(),
                         fk->owner()->isPrimaryKeyColumn(fk->columns()[i]) ? " (PK)" : ""));
    }
  return text;
}

auto RelationshipEditorBE::get_right_table_info() -> std::string {
  std::string text;
  db_ForeignKeyRef fk(_relationship->foreignKey());
  if (fk.is_valid())
    for (size_t c = fk->columns().count(), i = 0; i < c; i++) {
      text.append(strfmt("%s: %s%s\n", fk->referencedColumns()[i]->name().c_str(),
                         fk->referencedColumns()[i]->formattedRawType().c_str(),
                         fk->referencedTable()->isPrimaryKeyColumn(fk->referencedColumns()[i]) ? " (PK)" : ""));
    }
  return text;
}

auto RelationshipEditorBE::get_left_table_fk() -> std::string {
  std::string text;
  db_ForeignKeyRef fk(_relationship->foreignKey());
  if (fk.is_valid())
    text = "Foreign Key: " + *fk->name();
  else
    text = "Foreign Key: NOT SET";
  return text;
}

auto RelationshipEditorBE::open_editor_for_table(const db_TableRef &table) -> void {
  if (table.is_valid()) {
    grt::BaseListRef args(grt::AnyType);
    args.ginsert(table);

    bec::GUIPluginFlags flags = bec::NoFlags;
    bec::PluginManager *pm = bec::GRTManager::get()->get_plugin_manager();

    app_PluginRef plugin(pm->select_plugin_for_input("catalog/Editors", args));
    if (!plugin.is_valid())
      plugin = pm->select_plugin_for_input("model/Editors", args);

    if (plugin.is_valid())
      pm->open_gui_plugin(plugin, args, flags);
  }
}

auto RelationshipEditorBE::open_editor_for_left_table() -> void {
  open_editor_for_table(_relationship->foreignKey()->owner());
}

auto RelationshipEditorBE::open_editor_for_right_table() -> void {
  open_editor_for_table(_relationship->foreignKey()->referencedTable());
}

auto RelationshipEditorBE::edit_left_table() -> void {
  open_editor_for_table(_relationship->foreignKey()->owner());
}

auto RelationshipEditorBE::edit_right_table() -> void {
  open_editor_for_table(_relationship->foreignKey()->referencedTable());
}

auto RelationshipEditorBE::invert_relationship() -> void {
}

auto RelationshipEditorBE::get_is_identifying() -> bool {
  if (_relationship->foreignKey().is_valid()) {
    db_TableRef table(_relationship->foreignKey()->owner());

    GRTLIST_FOREACH(db_Column, _relationship->foreignKey()->columns(), column) {
      if (!table->isPrimaryKeyColumn(*column))
        return false;
    }
    return true;
  }
  return false;
}

auto RelationshipEditorBE::set_is_identifying(bool flag) -> void {
  db_TableRef table(_relationship->foreignKey()->owner());

  if (get_is_identifying() != flag) {
    // grt::AutoUndo undo;
    AutoUndoEdit undo(this);

    GRTLIST_FOREACH(db_Column, _relationship->foreignKey()->columns(), column) {
      if ((*table->isPrimaryKeyColumn(*column) == 1) != flag) {
        if (flag)
          table->addPrimaryKeyColumn(*column);
        else
          table->removePrimaryKeyColumn(*column);
      }
    }

    if (flag)
      undo.end(_("Make Relationship Identifying (Set PK)"));
    else
      undo.end(_("Make Relationship Non-Identifying (Unset PK)"));
  }
}

auto RelationshipEditorBE::get_title() -> std::string {
  return base::strfmt("Relationship");
}
