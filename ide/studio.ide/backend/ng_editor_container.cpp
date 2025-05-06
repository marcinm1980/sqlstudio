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

#include "ng_editor_container.h"
#include "base/log.h"
#include "mforms/jsonview.h"
#include "mforms/splitter.h"
#include "ng_recordset_view.h"
#include "grtdb/db_helpers.h"

#include "modules/mod_mysql_resultset.h"
#include "modules/mod_mysqlx_schema.h"
#include "modules/mod_mysqlx_session.h"
#include "utils/utils_general.h"
#include <functional>

using namespace ng;

//DEFAULT_LOG_DOMAIN("NG")

static JsonParser::JsonValue parse(const shcore::Value &value)
{
 switch(value.type)
 {
 case shcore::Undefined:
   //TODO: check what will be done with it
   return JsonParser::JsonValue();
 case shcore::Null:
   return JsonParser::JsonValue();
 case shcore::Bool:
   return JsonParser::JsonValue(value.as_bool());
 case shcore::Integer:
   return JsonParser::JsonValue(value.as_int());
 case shcore::UInteger:
   return JsonParser::JsonValue(value.as_uint());
 case shcore::Float:
   return JsonParser::JsonValue(value.as_double());
 case shcore::String:
   return JsonParser::JsonValue(value.as_string());
 case shcore::Object:
 {
   JsonParser::JsonObject obj;
   boost::shared_ptr<mysh::Row> row = value.as_object<mysh::Row>();
   if (row->class_name() == "Date")
     return JsonParser::JsonValue(value.descr(false));
   else
   {
     for (auto it = row->values.begin(); it != row->values.end(); ++it)
       obj[it->first] = parse(it->second);
   }
   return JsonParser::JsonValue(obj);
 }
 case shcore::Array:
 {
   shcore::Value::Array_type_ref array = value.as_array();
   shcore::Value::Array_type::const_iterator index, end = array->end();
   JsonParser::JsonArray jsar;
   for (index = array->begin(); index != end; ++index)
     jsar.pushBack(parse(*index));
   return JsonParser::JsonValue(jsar);
 }

 case shcore::Map:
 {
   shcore::Value::Map_type_ref map = value.as_map();
   shcore::Value::Map_type::const_iterator index, end = map->end();

    JsonParser::JsonObject obj;
    for (index = map->begin(); index != end; ++index)
    {
      obj[index->first] = parse(index->second);
    }
    return JsonParser::JsonValue(obj);
 }
 case shcore::MapRef:
  // TODO: define what to do with this too
   break;
 case shcore::Function:
  // TODO:

   break;
 }
 return JsonParser::JsonValue();
}

//----------------- NgEditorTab --------------------------------------------------------------------

NgEditorContainer::NgEditorContainer(EditorLanguage type)
  : Box(false), _type(type), _output(mforms::BothScrollBars), _resultTab(mforms::TabViewSystemStandard),
   _isScratch(true)
{

  _delegate.user_data = this;
  _delegate.print = &NgEditorContainer::print;
  _delegate.print_error = &NgEditorContainer::printError;
  _delegate.password = &NgEditorContainer::getPassword;
  _delegate.source = &NgEditorContainer::source;
  //_delegate.on_create_session = &NgEditorContainer::onCreateSession;
  _delegate.prompt = &NgEditorContainer::prompt;

  _shell.reset(new shcore::Shell_core(&_delegate));

  setupUI(type);
}

//--------------------------------------------------------------------------------------------------

NgEditorContainer::~NgEditorContainer()
{
}

//--------------------------------------------------------------------------------------------------

void NgEditorContainer::setSession(sharedSessionRef session)
{
  _session = session;
  _shell->set_global("session", shcore::Value(boost::static_pointer_cast<shcore::Object_bridge>(session)));
}

//--------------------------------------------------------------------------------------------------

void NgEditorContainer::setupUI(EditorLanguage type)
{

  bool initializationDone;
  switch (type)
  {
  case EditorLanguage::MySQL:
  {
    _shell->switch_mode(shcore::Shell_core::Mode_SQL, initializationDone);
    break;
  }

  case EditorLanguage::ECMA:
  {
    _shell->switch_mode(shcore::Shell_core::Mode_JScript, initializationDone);
    break;
  }

  case EditorLanguage::Python:
  {
    _shell->switch_mode(shcore::Shell_core::Mode_Python, initializationDone);
    break;
  }
  }
  set_spacing(0);
  set_padding(0);

  _endlessControl.setLanguage(type);
  add(&_endlessControl, true, true);
  _endlessControl.setExecuteCallback(std::bind(&NgEditorContainer::executeCommand, this, std::placeholders::_1));
}

//--------------------------------------------------------------------------------------------------

void NgEditorContainer::appendOutput(const std::string &text)
{
  _endlessControl.appendText(text);
}

//--------------------------------------------------------------------------------------------------

EditorLanguage NgEditorContainer::getType()
{
  return _type;
}

//--------------------------------------------------------------------------------------------------

void NgEditorContainer::execute(bool currentStatementOnly)
{
  auto spt = _session.lock();
  if (_type == EditorLanguage::MySQL && (!spt || !spt->is_connected()))
    return;

  std::string editorText = _endlessControl.getCurrentStatement(false);
  if (editorText.empty())
  {
    _shell->print_help("");
    return;
  }

  shcore::Interactive_input_state state = shcore::Input_ok;

  try {
    _shell->handle_input(editorText, state, boost::bind(&NgEditorContainer::processResult, this, _1));

    std::string executed = _shell->get_handled_input();
  } catch (std::exception &exc)
  {
    _shell->print_error(exc.what());
  }
}

//--------------------------------------------------------------------------------------------------

void NgEditorContainer::executeCommand(std::string script)
{
  if (script.empty())
  {
    _shell->print_help("");
    return;
  }

  shcore::Interactive_input_state state = shcore::Input_ok;
  try 
  {
    _shell->handle_input(script, state, boost::bind(&NgEditorContainer::processResult, this, _1));

    std::string executed = _shell->get_handled_input();
  }
  catch (std::exception &exc)
  {
    _shell->print_error(exc.what());
  }
}

//--------------------------------------------------------------------------------------------------


void NgEditorContainer::processResult(shcore::Value result)
{
  if (result)
  {
    if (result.type == shcore::Object && result.as_object<mysh::mysql::ClassicResult>())
      this->processSql(result);
    else
      this->processJSON(result);
  }
  else
    _endlessControl.processText();
}

//--------------------------------------------------------------------------------------------------

void NgEditorContainer::processJSON(const shcore::Value &result)
{
  _endlessControl.processJson(parse(result));
}

//--------------------------------------------------------------------------------------------------

void NgEditorContainer::processSql(const shcore::Value &result)
{
  _endlessControl.processSql(result);
}

//--------------------------------------------------------------------------------------------------

void NgEditorContainer::triggerCodeCompletion()
{

}

//--------------------------------------------------------------------------------------------------

void NgEditorContainer::print(void *self, const char *text)
{
  NgEditorContainer *tab = static_cast<NgEditorContainer*>(self);
  tab->appendOutput(text);
}

//--------------------------------------------------------------------------------------------------

void NgEditorContainer::printError(void *self, const char *text)
{
  auto tab = static_cast<NgEditorContainer*>(self);
  tab->_endlessControl.printError(std::string("[ERROR] " + std::string(text)));
}

//--------------------------------------------------------------------------------------------------

bool NgEditorContainer::getPassword(void *self, const char *text, std::string &ret)
{
  return false;
}

//--------------------------------------------------------------------------------------------------

void NgEditorContainer::source(void *self, const char *module)
{

}

//--------------------------------------------------------------------------------------------------
bool NgEditorContainer::prompt(void *self, const char *text, std::string &ret)
{
  return false;
}

//--------------------------------------------------------------------------------------------------

void NgEditorContainer::onCreateSession(void *self, const shcore::Value &session)
{
  NgEditorContainer *tab = static_cast<NgEditorContainer*>(self);
  if (tab->_onCreateSession)
    tab->_onCreateSession(session);

}

//--------------------------------------------------------------------------------------------------

void NgEditorContainer::focus()
{
  _endlessControl.focus();
}
