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

#pragma once

#include "wbpublic_public_interface.h"
#include "grt.h"
#include "grts/structs.app.h"
#include "tree_model.h"
#include "refresh_ui.h"
#include <deque>

// Common tag names
#define CHECK_NAME "name"
#define CHECK_SYNTAX "syntax"
#define CHECK_EFFICIENCY "efficiency"
#define CHECK_LOGIC "logic"

namespace bec {

  class GRTManager;

  class WBPUBLICBACKEND_PUBLIC_FUNC ValidationMessagesBE : public ListModel, public RefreshUI {
  public:
    enum ValidationMessageColumns { Description = 1 };

    ValidationMessagesBE();

    auto clear() -> void;
    virtual auto get_field(const NodeId& node, ColumnId column, std::string& value) -> bool;
    virtual auto get_field_icon(const NodeId& node, ColumnId column, IconSize size) -> IconId;
    virtual auto refresh() -> void {
    }
    virtual auto count() -> size_t;

    virtual auto get_node_popup_items(const NodeId& node, MenuItemList& menu) -> int;
    virtual auto activate_node_popup_item(const NodeId& node, const std::string& name) -> void;

  private:
    auto validation_message(const grt::Validator::Tag& tag, const grt::ObjectRef&, const std::string&, const int level) -> void;

    IconId _error_icon;
    IconId _warning_icon;
    IconId _info_icon;

    struct Message {
      Message() {
      }
      Message(const std::string& message, const grt::ObjectRef& object, const grt::Validator::Tag& _tag)
        : msg(message), obj(object), tag(_tag) {
      }
      std::string msg;
      grt::ObjectRef obj;
      grt::Validator::Tag tag;
    };

    using MessageList = std::deque<Message>;
    MessageList _errors;
    MessageList _warnings;

    static auto match_message(const Message& m, const grt::ObjectRef& obj, const grt::Validator::Tag& tag) -> bool;
    auto remove_messages(MessageList* ml, const grt::ObjectRef& obj, const grt::Validator::Tag& tag) -> void;
  };

  class WBPUBLICBACKEND_PUBLIC_FUNC ValidationManager {
  public:
    // const int parameter in MessageSignal is a grt::MessageType
    using MessageSignal =
      boost::signals2::signal<void(const grt::Validator::Tag&, const grt::ObjectRef&, const std::string&, const int)>;

    static auto scan() -> void;
    static auto register_validator(const std::string& type, grt::Validator* v) -> void;
    static auto validate_instance(const grt::ObjectRef& obj, const grt::Validator::Tag& tag) -> bool;

    static auto signal_notify() -> MessageSignal*;
    static auto message(const grt::Validator::Tag&, const grt::ObjectRef&, const std::string&,
                        const int level) -> void; // level is grt::MessageType
    static auto clear() -> void;

  private:
    static auto is_validation_plugin(const app_PluginRef& plugin) -> bool;

    static MessageSignal* _signal_notify;
  };

  //------------------------------------------------------------------------------
  inline auto bec::ValidationManager::signal_notify() -> bec::ValidationManager::MessageSignal* {
    if (!_signal_notify)
      _signal_notify = new ValidationManager::MessageSignal;

    return _signal_notify;
  }
} // namespace bec
