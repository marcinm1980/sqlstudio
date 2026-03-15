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

#include "wb_tag_editor.h"
#include "grtpp_util.h"
#include "grtpp_undo_manager.h"
#include "grt/common.h"
#include "grtdb/db_object_helpers.h"
#include "grtui/string_list_editor.h"

class GrtObjectListEditor : public grtui::StringListEditor {
  GrtObjectRef _owner;
  grt::ListRef<GrtObject> _list;
  std::list<std::string> _deleted_objects;

  virtual auto del() -> void {
    int row = _tree.get_selected();
    if (row >= 0) {
      std::string tag = _tree.get_row_tag(row);
      if (!tag.empty())
        _deleted_objects.push_back(tag);
      _tree.delete_row(row);
    }
  }

public:
  GrtObjectListEditor(grt::GRT *grt, const GrtObjectRef &owner, const grt::ListRef<GrtObject> &objects)
    : grtui::StringListEditor(grt, false), _owner(owner), _list(objects) {
    _tree.clear_rows();
    for (grt::ListRef<GrtObject>::const_iterator r = objects.begin(); r != objects.end(); ++r) {
      int row = _tree.add_row();
      _tree.set(row, 0, (*r)->name().c_str());

      _tree.set_row_tag(row, (*r)->id());
    }
  }

  auto update_list(const sigc::slot<bool, GrtObjectRef> &check_if_deletable) -> bool {
    bool changed = false;

    for (std::list<std::string>::const_iterator iter = _deleted_objects.begin(); iter != _deleted_objects.end();
         ++iter) {
      GrtObjectRef object(grt::find_object_in_list(_list, *iter));

      if (object.is_valid() && check_if_deletable(object)) {
        _list.gremove_value(object);
        changed = true;
      }
    }

    for (int c = _tree.count(), i = 0; i < c; i++) {
      std::string name = _tree.get_string(i, 0);
      std::string tag = _tree.get_row_tag(i);

      if (tag.empty()) {
        GrtObjectRef object(_grt);
        object->name(name);
        object->owner(_owner);

        _list.insert(object, i);

        changed = true;
      } else {
        GrtObjectRef object(grt::find_object_in_list(_list, tag));

        if (object.is_valid() && object->name() != name) {
          changed = true;
          object->name(name);
        }
      }
    }

    return changed;
  }
};

TagObjectListBE::TagObjectListBE(const db_CatalogRef &catalog) : _catalog(catalog) {
}

auto TagObjectListBE::set_tag(const meta_TagRef &tag) -> void {
  _tag = tag;
}

auto TagObjectListBE::get_field(const bec::NodeId &node, int column, std::string &value) -> bool {
  if (_tag.is_valid()) {
    switch (column) {
      case Name:
        value = _content[node[0]].object->name();
        break;
      case Documentation:
        value = _content[node[0]].doc;
        break;
    }
  }
  return true;
}

auto TagObjectListBE::set_field(const bec::NodeId &node, int column, const std::string &value) -> bool {
  if (_tag.is_valid()) {
    if (column == Documentation) {
      _content[node[0]].doc = value;
      return true;
    }
  }
  return false;
}

auto TagObjectListBE::get_field_icon(const bec::NodeId &node, int column, bec::IconSize size) -> bec::IconId {
  if (_tag.is_valid())
    return bec::IconManager::get_instance()->get_icon_id(_content[node[0]].object, size);
  return 0;
}

auto TagObjectListBE::refresh() -> void {
  _content.clear();
  if (_tag.is_valid()) {
    for (grt::ListRef<meta_TaggedObject>::const_iterator end = _tag->objects().end(), iter = _tag->objects().begin();
         iter != end; ++iter) {
      Node node;
      node.owner = *iter;
      node.object = (*iter)->object();
      node.doc = (*iter)->description();

      _content.push_back(node);
    }
  }
}

static auto is_item_deleted(const grt::ObjectRef &object, const std::vector<TagObjectListBE::Node> &list) -> bool {
  for (std::vector<TagObjectListBE::Node>::const_iterator end = list.end(), iter = list.begin(); iter != end; ++iter) {
    if ((*iter).object == meta_TaggedObjectRef::cast_from(object)->object())
      return false;
  }
  return true;
}

auto TagObjectListBE::commit() -> bool {
  bool changed = false;

  if (_tag.is_valid()) {
    // remove deleted items
    size_t ocount = _tag->objects().count();
    grt::remove_list_items_matching(_tag->objects(), sigc::bind(sigc::ptr_fun(is_item_deleted), _content));
    changed = ocount != _tag->objects().count();

    // update items and add new ones
    for (std::vector<TagObjectListBE::Node>::iterator end = _content.end(), iter = _content.begin(); iter != end;
         ++iter) {
      if ((*iter).owner.is_valid()) {
        if ((*iter).doc != *(*iter).owner->description()) {
          changed = true;
          (*iter).owner->description((*iter).doc);
        }
      } else {
        meta_TaggedObjectRef to(_tag.get_grt());

        to->owner(_tag);
        to->object((*iter).object);
        to->description((*iter).doc);
        _tag->objects().insert(to);

        (*iter).owner = to;

        changed = true;
      }
    }
  }
  return changed;
}

auto TagObjectListBE::count() -> int {
  if (_tag.is_valid())
    return (int)_content.size();
  return 0;
}

auto TagObjectListBE::add_dropped_objectdata(const std::string &data) -> bool {
  if (_tag.is_valid()) {
    db_DatabaseObjectRef object;

    object = bec::CatalogHelper::dragdata_to_dbobject(_catalog, data);
    if (object.is_valid()) {
      for (std::vector<Node>::const_iterator end = _content.end(), iter = _content.begin(); iter != end; ++iter) {
        if ((*iter).object == object)
          return false;
      }

      Node node;
      node.object = object;

      _content.push_back(node);

      return true;
    }
  }
  return false;
}

TagEditorBE::TagEditorBE(const studio_physical_ModelRef &model) : _model(model), _object_list(model->catalog()) {
  _selected_tag = -1;
  _selected_category = 0;
}

auto TagEditorBE::set_selected_category(int category) -> void {
  _selected_category = category;
}

auto TagEditorBE::get_categories() const -> std::vector<std::string> {
  std::vector<std::string> categories;

  for (grt::ListRef<GrtObject>::const_iterator end = _model->tagCategories().end(),
                                               iter = _model->tagCategories().begin();
       iter != end; ++iter) {
    categories.push_back((*iter)->name().c_str());
  }

  return categories;
}

auto TagEditorBE::get_tags() const -> std::vector<std::string> {
  std::vector<std::string> tags;

  GrtObjectRef category;

  if (_selected_category >= 0 && _selected_category < _model->tagCategories().count())
    category = _model->tagCategories()[_selected_category];

  for (grt::ListRef<meta_Tag>::const_iterator end = _model->tags().end(), iter = _model->tags().begin(); iter != end;
       ++iter) {
    if ((*iter)->category() == category)
      tags.push_back((*iter)->name().c_str());
  }

  return tags;
}

static auto check_if_category_unused(const GrtObjectRef &category, const grt::ListRef<meta_Tag> &tags,
                                     int *deny_count) -> bool {
  GRTLIST_FOREACH(meta_Tag, tags, iter) {
    if ((*iter)->category() == category) {
      (*deny_count)++;
      return false;
    }
  }
  return true;
}

auto TagEditorBE::edit_categories() -> void {
  GrtObjectListEditor editor(_model.get_grt(), _model, _model->tagCategories());

  if (editor.run()) {
    int deny_count = 0;
    grt::AutoUndo undo(_model.get_grt());

    editor.update_list(sigc::bind(sigc::ptr_fun(check_if_category_unused), _model->tags(), &deny_count));

    undo.end_or_cancel_if_empty(_("Edit Tag Categories"));

    if (deny_count > 0) {
      mforms::Utilities::show_warning(_("Edit Tag Categories"),
                                      _("Some of the categories could not be deleted because they are in use by tags."),
                                      _("OK"));
    }
  }
}

auto TagEditorBE::add_tag() -> void {
  meta_TagRef tag(_model.get_grt());

  tag->owner(_model);
  tag->name(grt::get_name_suggestion_for_list_object(_model->tags(), "BR", true));
  tag->label(tag->name());
  tag->color("#3377aa");
  if (_selected_category >= 0 && _selected_category < _model->tagCategories().count())
    tag->category(_model->tagCategories()[_selected_category]);

  grt::AutoUndo undo(_model.get_grt());

  _model->tags().insert(tag);

  undo.end(_("Add New Tag"));
}

auto TagEditorBE::delete_tag() -> bool {
  if (_selected_tag >= 0 && _selected_tag < _model->tags().count()) {
    grt::AutoUndo undo(_model.get_grt());

    std::string name = get_tag_name();
    _model->tags().remove(_selected_tag);

    undo.end(strfmt(_("Delete Tag %s"), name.c_str()));

    _selected_tag = -1;

    return true;
  }
  return false;
}

auto TagEditorBE::set_selected_tag(int index) -> void {
  if (index >= 0 && index < (int)_model->tags().count()) {
    _object_list.set_tag(_model->tags()[index]);
  } else {
    _object_list.set_tag(meta_TagRef());
  }

  _selected_tag = index;
  _object_list.refresh();
}

auto TagEditorBE::set_tag_name(const std::string &name) -> void {
  if (_selected_tag >= 0 && _selected_tag < _model->tags().count()) {
    if (name != *_model->tags()[_selected_tag]->name()) {
      _changed = true;
      _model->tags()[_selected_tag]->name(name);
    }
  }
}

auto TagEditorBE::set_tag_label(const std::string &label) -> void {
  if (_selected_tag >= 0 && _selected_tag < _model->tags().count()) {
    if (label != *_model->tags()[_selected_tag]->label()) {
      _changed = true;
      _model->tags()[_selected_tag]->label(label);
    }
  }
}

auto TagEditorBE::set_tag_color(const std::string &color) -> void {
  if (_selected_tag >= 0 && _selected_tag < _model->tags().count()) {
    if (color != *_model->tags()[_selected_tag]->color()) {
      _changed = true;
      _model->tags()[_selected_tag]->color(color);
    }
  }
}

auto TagEditorBE::set_tag_comment(const std::string &comment) -> void {
  if (_selected_tag >= 0 && _selected_tag < _model->tags().count()) {
    if (comment != *_model->tags()[_selected_tag]->description()) {
      _changed = true;
      _model->tags()[_selected_tag]->description(comment);
    }
  }
}

auto TagEditorBE::get_tag_name() -> std::string {
  if (_selected_tag >= 0 && _selected_tag < _model->tags().count())
    return _model->tags()[_selected_tag]->name();
  return "";
}

auto TagEditorBE::get_tag_label() -> std::string {
  if (_selected_tag >= 0 && _selected_tag < _model->tags().count())
    return _model->tags()[_selected_tag]->label();
  return "";
}

auto TagEditorBE::get_tag_color() -> std::string {
  if (_selected_tag >= 0 && _selected_tag < _model->tags().count())
    return _model->tags()[_selected_tag]->color();
  return "";
}

auto TagEditorBE::get_tag_comment() -> std::string {
  if (_selected_tag >= 0 && _selected_tag < _model->tags().count())
    return _model->tags()[_selected_tag]->description();
  return "";
}

auto TagEditorBE::begin_save() -> void {
  _changed = false;
  _model.get_grt()->begin_undoable_action();
}

auto TagEditorBE::end_save() -> void {
  _changed |= _object_list.commit();

  if (_changed)
    _model.get_grt()->end_undoable_action(_("Edit Tag"));
  else
    _model.get_grt()->cancel_undoable_action();
}
