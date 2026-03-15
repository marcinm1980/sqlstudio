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

#include "grtpp_util.h"
#include "wbpublic_public_interface.h"

#include "base/string_utilities.h"
#include "grt/common.h"
#include "grtdb/charset_utils.h"
#include "grtsqlparser/sql_facade.h"
#include "db_object_helpers.h"

namespace sql {
  class DatabaseMetaData;
}

namespace grt {

  struct WBPUBLICBACKEND_PUBLIC_FUNC DbObjectMatchAlterOmf : public Omf {
    virtual auto less(const ValueRef&, const ValueRef&) const -> bool;
    virtual auto equal(const ValueRef&, const ValueRef&) const -> bool;
  };

  typedef std::function<bool(const ValueRef obj1, const ValueRef obj2, const std::string name)> comparison_rule;
  class WBPUBLICBACKEND_PUBLIC_FUNC NormalizedComparer {
  protected:
    auto comment_compare(const ValueRef obj1, const ValueRef obj2, const std::string& name) const -> bool;
    std::map<std::string, std::list<comparison_rule> > rules;
    int _maxTableCommentLength;
    int _maxIndexCommentLength;
    int _maxColumnCommentLength;

    bool _case_sensitive;
    bool _skip_routine_definer;
    auto load_rules() -> void;

  public:
    auto init_omf(Omf* omf) -> void;
    auto load_db_options(sql::DatabaseMetaData* dbc_meta) -> void;
    NormalizedComparer(const grt::DictRef options = grt::DictRef());
    auto add_comparison_rule(const std::string& name, comparison_rule rule) -> void {
      rules[name].push_back(rule);
    };
    auto normalizedComparison(const ValueRef obj1, const ValueRef obj2, const std::string name) -> bool;
    auto get_options_dict() const -> grt::DictRef;
    auto is_case_sensitive() const -> bool {
      return _case_sensitive;
    };
    auto skip_routine_definer() const -> bool {
      return _skip_routine_definer;
    };
  };

} // namespace grt
