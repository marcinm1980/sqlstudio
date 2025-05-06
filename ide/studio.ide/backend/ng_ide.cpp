/*
* Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; version 2 of the
* License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
* 02110-1301  USA
*/

#define BOOST_ERROR_CODE_HEADER_ONLY
#include <boost/system/error_code.hpp>

#include "ng_ide.h"

//#include "studio/wb_context_ui.h"

#include "base/file_utilities.h"
#include "grt/grt_manager.h"
//#include "studio/wb_command_ui.h"

#include "code-completion/mysql-code-completion.h"

#include "../editors/code_editor_base.h"

//using namespace wb;
using namespace ng;
using namespace bec;

// Some forward declarations to avoid having to put all that before the main code.
//static bool validateTriggerCodeCompletion(NgIDE *ide);

//----------------- NgIDE --------------------------------------------------------------------------

NgIDE::NgIDE()
{
  std::string grammar_path = base::makePath(bec::GRTManager::get().get_basedir(), "data/MySQL.g");
  initializeMySQLCodeCompletionIfNeeded(grammar_path);

//  // Setup some builtin commands handled by ourselves for the SQL IDE.
//  CommandUI *cmdui = WBContextUI::get()->get_command_ui();
//
//  cmdui->add_builtin_command("list-members-ng", std::bind(&NgIDE::callInEditorPage, this,
//    &NgEditorContainer::triggerCodeCompletion), std::bind(validateTriggerCodeCompletion, this));

}

//--------------------------------------------------------------------------------------------------

/**
 * Creates a new ng IDE instance (ng sheet).
 */
mforms::View* NgIDE::openInstance(const dataTypes::NodeConnection &connection, EditorLanguage language)
{
  auto ngSheet = mforms::manage(new NgSheet(language), false);

  try
  {
    if (connection.isValid())
      ngSheet->doConnect(connection, language);
  } catch (...)
  {
    ngSheet->release();
    throw;
  }

  return ngSheet;
}

//--------------------------------------------------------------------------------------------------

void NgIDE::executeQueryInCurrentInstance()
{
//  if (_ngSheet != nullptr)
//    _ngSheet->execute(0);
}

void NgIDE::executeCurrentStatementInCurrentInstance()
{
//  if (_ngSheet != nullptr)
//    _ngSheet->execute(1);
}

//--------------------------------------------------------------------------------------------------
mforms::View *NgIDE::startup()
{
  mforms::View *view = mforms::manage(new mforms::Box(false));
  return view;
}

//--------------------------------------------------------------------------------------------------
/*
static bool validateTriggerCodeCompletion(NgIDE *ide)
{
  return bec::GRTManager::get().get_app_option_int("DbSqlEditor:CodeCompletionEnabled") != 0;
}
*/
//--------------------------------------------------------------------------------------------------

void NgIDE::callInEditorPage(void (NgEditorContainer::*method)())
{
  mforms::AppView *view = mforms::App::get()->selected_view();
  NgSheet *sheet = dynamic_cast<NgSheet*>(view);
  if (sheet != nullptr)
  {
    NgEditorContainer *page = sheet->editorContainer();
    if (page != nullptr)
      (page->*method)();
  }
}

//--------------------------------------------------------------------------------------------------
