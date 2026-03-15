/*
 * Copyright (c) 2007, 2019, Oracle and/or its affiliates. All rights reserved.
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

#include "grt.h"
#include "grt_reporter.h"

#include <gmodule.h>

using namespace grt;
using namespace bec;

Reporter::Reporter() : _error_count(0), _warning_count(0), _tracking(false) {
  flush();
};

auto Reporter::is_tracking() const -> bool {
  return _tracking;
}

auto Reporter::start_tracking() const -> void {
  _tracking = true;
}

auto Reporter::flush() const -> void {
  _warning_count = 0;
  _error_count = 0;
  _tracking = false;
}

auto Reporter::report_warning(const char *format, ...) const -> void {
  _warning_count++;

  va_list args;
  char *tmp;

  va_start(args, format);
  tmp = g_strdup_vprintf(format, args);
  va_end(args);

  if (tmp) {
    grt::GRT::get()->send_warning(tmp);
    g_free(tmp);
  } else if (format)
    grt::GRT::get()->send_warning(format);
}

auto Reporter::report_error(const char *format, ...) const -> void {
  _error_count++;

  va_list args;
  char *tmp = 0;

  va_start(args, format);
  tmp = g_strdup_vprintf(format, args);
  va_end(args);

  if (tmp) {
    grt::GRT::get()->send_error(tmp);
    g_free(tmp);
  } else if (format) {
    grt::GRT::get()->send_error(format);
  }
}

auto Reporter::report_info(const char *format, ...) const -> void {
  va_list args;
  char *tmp;

  va_start(args, format);
  tmp = g_strdup_vprintf(format, args);
  va_end(args);

  if (tmp) {
    grt::GRT::get()->send_info(tmp);
    g_free(tmp);
  } else if (format) {
    grt::GRT::get()->send_info(format);
  }
}

auto Reporter::report_heading(const char* format, ...) const -> void {
  va_list args;
  char *tmp;

  va_start(args, format);
  tmp = g_strdup_vprintf(format, args);
  va_end(args);

  if (tmp) {
    grt::GRT::get()->send_info("===========================");
    grt::GRT::get()->send_info(tmp);
    grt::GRT::get()->send_info("===========================");
    g_free(tmp);
  } else if (format) {
    grt::GRT::get()->send_info("===========================");
    grt::GRT::get()->send_info(format);
    grt::GRT::get()->send_info("===========================");
  }
}

auto Reporter::report_summary(const char *operation_name) const -> void {
  if (error_count() && warning_count())
    report_info("Operation '%s' finished with %d errors and %d warnings", operation_name, error_count(),
                warning_count());
  else if (error_count())
    report_info("Operation '%s' finished with %d errors", operation_name, error_count());
  else if (warning_count())
    report_info("Operation '%s' finished with %d warnings", operation_name, warning_count());
  else
    report_info("Operation '%s' finished successfully", operation_name);
  flush();
}

auto Reporter::error_count() const -> int {
  return _error_count;
}

auto Reporter::warning_count() const -> int {
  return _warning_count;
}

SummaryCentry::SummaryCentry(Reporter &parent, const std::string &operation) : _rep(NULL) {
  if (!parent.is_tracking()) {
    _rep = &parent;
    _rep->start_tracking();
    _operation = operation;
  }
}

SummaryCentry::~SummaryCentry() {
  if (_rep)
    _rep->report_summary(_operation.c_str());
}
