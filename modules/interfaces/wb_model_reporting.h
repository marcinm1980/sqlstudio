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

#ifndef _WB_MODEL_REPORTING_IF_H_
#define _WB_MODEL_REPORTING_IF_H_

#include "grtpp_module_cpp.h"
#include "grts/structs.studio.physical.h"
#include "grts/structs.studio.model.reporting.h"

// schema report interface definition header

class WbModelReportingInterfaceImpl : public grt::InterfaceImplBase {
public:
  DECLARE_REGISTER_INTERFACE(WbModelReportingInterfaceImpl,
                             DECLARE_INTERFACE_FUNCTION(WbModelReportingInterfaceImpl::getAvailableReportingTemplates),
                             DECLARE_INTERFACE_FUNCTION(WbModelReportingInterfaceImpl::getTemplateDirFromName),
                             DECLARE_INTERFACE_FUNCTION(WbModelReportingInterfaceImpl::getReportingTemplateInfo),
                             DECLARE_INTERFACE_FUNCTION(WbModelReportingInterfaceImpl::generateReport));

  virtual auto getAvailableReportingTemplates(grt::StringListRef templates) -> ssize_t = 0;

  virtual auto getTemplateDirFromName(const std::string& template_name) -> std::string = 0;

  virtual auto getReportingTemplateInfo(
    const std::string& template_name) -> grt::Ref<studio_model_reporting_TemplateInfo> = 0;

  virtual auto generateReport(grt::Ref<studio_physical_Model> model, const grt::DictRef& options) -> ssize_t = 0;
};

#endif /* _WB_MODEL_REPORTING_IF_H_ */
