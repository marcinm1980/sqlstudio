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

#ifndef _WB_MODULE_H_
#define _WB_MODULE_H_

#include "grtpp_module_cpp.h"

#include "wb_context.h"
#include "grts/structs.db.mgmt.h"
#include "interfaces/plugin.h"

#ifdef _MSC_VER
#include "wmi.h"
#endif

#define WBModule_VERSION "5.2.27"

namespace wb {

  class MySqlStudioImpl : public grt::ModuleImplBase, public PluginInterfaceImpl {
    using super = grt::ModuleImplBase;

  public:
    MySqlStudioImpl(grt::CPPModuleLoader *);
    virtual ~MySqlStudioImpl();

    auto set_context(WBContext *wb) -> void;
    auto getSystemInfo(bool indent) -> std::string;
    auto getSystemInfoMap() -> std::map<std::string, std::string>;
    auto isOsSupported(const std::string &os) -> int;

    DEFINE_INIT_MODULE(
      WBModule_VERSION, "Oracle and/or its affiliates", grt::ModuleImplBase,
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::getPluginInfo),

      // Non-plugin functions
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::copyToClipboard),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::hasUnsavedChanges),

      // Model
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::newDocument),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::newDocumentFromDB), DECLARE_MODULE_FUNCTION(MySqlStudioImpl::openModel),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::openRecentModel), DECLARE_MODULE_FUNCTION(MySqlStudioImpl::saveModel),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::saveModelAs), DECLARE_MODULE_FUNCTION(MySqlStudioImpl::exit),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::exportPNG), DECLARE_MODULE_FUNCTION(MySqlStudioImpl::exportPDF),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::exportPS), DECLARE_MODULE_FUNCTION(MySqlStudioImpl::exportSVG),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::activateDiagram),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::exportDiagramToPng),

      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::selectAll), DECLARE_MODULE_FUNCTION(MySqlStudioImpl::selectSimilar),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::selectConnected),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::goToNextSelected),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::goToPreviousSelected),

      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::highlightFigure),

      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::editSelectedFigure),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::editSelectedFigureInNewWindow),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::editObject),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::editObjectInNewWindow),

      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::raiseSelection),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::lowerSelection),

      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::newDiagram),

      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::toggleGrid), DECLARE_MODULE_FUNCTION(MySqlStudioImpl::togglePageGrid),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::toggleGridAlign),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::toggleFKHighlight),

      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::zoomIn), DECLARE_MODULE_FUNCTION(MySqlStudioImpl::zoomOut),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::zoomDefault),

      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::setFigureNotation),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::setRelationshipNotation),

      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::setMarker), DECLARE_MODULE_FUNCTION(MySqlStudioImpl::goToMarker),

      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::startTrackingUndo),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::finishTrackingUndo),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::cancelTrackingUndo),

      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::isOsSupported),

      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::addUndoListAdd),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::addUndoListRemove),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::addUndoObjectChange),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::addUndoDictSet),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::beginUndoGroup), DECLARE_MODULE_FUNCTION(MySqlStudioImpl::endUndoGroup),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::setUndoDescription),

      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::createAttachedFile),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::setAttachedFileContents),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::getAttachedFileContents),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::getAttachedFileTmpPath),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::exportAttachedFileContents),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::openModelFile), DECLARE_MODULE_FUNCTION(MySqlStudioImpl::closeModelFile),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::getDbFilePath), DECLARE_MODULE_FUNCTION(MySqlStudioImpl::getTempDir),

      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::debugValidateGRT),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::getVideoAdapter),

      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::runScriptFile),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::installModuleFile),

      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::showUserTypeEditor),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::showDocumentProperties),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::showModelOptions), DECLARE_MODULE_FUNCTION(MySqlStudioImpl::showOptions),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::showConnectionManager),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::showInstanceManagerFor),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::showInstanceManager),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::showQueryConnectDialog),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::saveConnections),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::saveInstances),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::refreshHomeConnections),

      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::showGRTShell), DECLARE_MODULE_FUNCTION(MySqlStudioImpl::newGRTFile),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::openGRTFile),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::showPluginManager), DECLARE_MODULE_FUNCTION(MySqlStudioImpl::reportBug),

      // Utilities
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::confirm), DECLARE_MODULE_FUNCTION(MySqlStudioImpl::requestFileOpen),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::requestFileSave),

      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::createConnectionsFromLocalServers),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::createInstancesFromLocalServers),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::create_connection),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::initializeOtherRDBMS),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::deleteConnection),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::deleteConnectionGroup),
      DECLARE_MODULE_FUNCTION(MySqlStudioImpl::createSSHSession));

  protected:
    virtual void initialization_done() override {
// Called after init_module (defined by DEFINE_INIT_MODULE above) is done.
// Here we register platform dependent functions.
#ifdef _MSC_VER
      register_functions(DECLARE_MODULE_FUNCTION(MySqlStudioImpl::wmiOpenSession),
                         DECLARE_MODULE_FUNCTION(MySqlStudioImpl::wmiCloseSession),
                         DECLARE_MODULE_FUNCTION(MySqlStudioImpl::wmiQuery),
                         DECLARE_MODULE_FUNCTION(MySqlStudioImpl::wmiServiceControl),
                         DECLARE_MODULE_FUNCTION(MySqlStudioImpl::wmiSystemStat),
                         DECLARE_MODULE_FUNCTION(MySqlStudioImpl::wmiStartMonitoring),
                         DECLARE_MODULE_FUNCTION(MySqlStudioImpl::wmiReadValue),
                         DECLARE_MODULE_FUNCTION(MySqlStudioImpl::wmiStopMonitoring), NULL);
#endif
    };

  private:
    WBContext *_wb;
#ifdef _MSC_VER
    std::map<int, wmi::WmiServices *> _wmi_sessions;
#ifdef DEBUG
    std::map<int, GThread *> _thread_for_wmi_session;
#endif
    int _last_wmi_session_id;

    std::map<int, wmi::WmiMonitor *> _wmi_monitors;
    int _last_wmi_monitor_id;
#endif

    virtual grt::ListRef<app_Plugin> getPluginInfo() override;

    auto copyToClipboard(const std::string &str) -> int;

    auto hasUnsavedChanges() -> int;

    // file
    auto newDocument() -> int;
    auto newDocumentFromDB() -> int;
    auto openModel(const std::string &path) -> int;
    auto openRecentModel(const std::string &index) -> int;
    auto saveModel() -> int;
    auto saveModelAs(const std::string &path) -> int;
    auto exportPNG(const std::string &filename) -> int;
    auto exportPDF(const std::string &filename) -> int;
    auto exportPS(const std::string &filename) -> int;
    auto exportSVG(const std::string &filename) -> int;
    auto activateDiagram(const model_DiagramRef &diagram) -> int;
    auto exportDiagramToPng(const model_DiagramRef &diagram, const std::string &filename) -> int;
    auto exit() -> int;

    // edit
    auto selectAll() -> int;
    auto selectSimilar() -> int;
    auto selectConnected() -> int;

    auto editSelectedFigure(const model_DiagramRef &view) -> int;
    auto editSelectedFigureInNewWindow(const model_DiagramRef &view) -> int;

    auto editObject(const GrtObjectRef &object) -> int;
    auto editObjectInNewWindow(const GrtObjectRef &object) -> int;

    // canvas manipulation
    auto raiseSelection(const model_DiagramRef &view) -> int;
    auto lowerSelection(const model_DiagramRef &view) -> int;

    // view
    auto newDiagram(const model_ModelRef &model) -> int;

    auto toggleGrid(const model_DiagramRef &view) -> int;
    auto togglePageGrid(const model_DiagramRef &view) -> int;
    auto toggleGridAlign(const model_DiagramRef &view) -> int;
    auto toggleFKHighlight(const model_DiagramRef &view) -> int;

    auto zoomIn() -> int;
    auto zoomOut() -> int;
    auto zoomDefault() -> int;

    auto goToNextSelected() -> int;
    auto goToPreviousSelected() -> int;

    auto setMarker(const std::string &marker) -> int;
    auto goToMarker(const std::string &marker) -> int;

    auto setFigureNotation(const std::string &name, studio_physical_ModelRef model) -> int;
    auto setRelationshipNotation(const std::string &name, studio_physical_ModelRef model) -> int;

    auto highlightFigure(const model_ObjectRef &figure) -> int;
    // undo
    auto startTrackingUndo() -> int;
    auto finishTrackingUndo(const std::string &description) -> int;
    auto cancelTrackingUndo() -> int;

    auto addUndoListAdd(const grt::BaseListRef &list) -> int;
    auto addUndoListRemove(const grt::BaseListRef &list, int index) -> int;
    auto addUndoObjectChange(const grt::ObjectRef &object, const std::string &member) -> int;
    auto addUndoDictSet(const grt::DictRef &dict, const std::string &key) -> int;
    auto beginUndoGroup() -> int;
    auto endUndoGroup() -> int;
    auto setUndoDescription(const std::string &text) -> int;

    // attached file management
    auto createAttachedFile(const std::string &group, const std::string &tmpl) -> std::string;
    auto setAttachedFileContents(const std::string &filename, const std::string &text) -> int;
    auto getAttachedFileContents(const std::string &filename) -> std::string;
    auto getAttachedFileTmpPath(const std::string &filename) -> std::string;
    auto exportAttachedFileContents(const std::string &filename, const std::string &export_to) -> int;
    auto openModelFile(const std::string &path) -> studio_DocumentRef;
    auto closeModelFile() -> int;
    auto getDbFilePath() -> std::string;
    auto getTempDir() -> std::string;

    auto runScriptFile(const std::string &filename) -> int;
    auto installModuleFile(const std::string &filename) -> int;

    // debugging
    auto debugValidateGRT() -> int;

    auto showUserTypeEditor(const studio_physical_ModelRef &model) -> int;
    auto showDocumentProperties() -> int;
    auto showModelOptions(const studio_physical_ModelRef &model) -> int;
    auto showOptions() -> int;
    auto showConnectionManager() -> int;
    auto showInstanceManagerFor(const db_mgmt_ConnectionRef &conn) -> int;
    auto showInstanceManager() -> int;
    auto showQueryConnectDialog() -> int;
    auto saveConnections() -> int;
    auto saveInstances() -> int;
    auto showGRTShell() -> int;
    auto showPluginManager() -> int;
    auto newGRTFile() -> int;
    auto openGRTFile() -> int;
    int reportBug(const std::string error_info = "");

    // UI
    auto refreshHomeConnections() -> int;
    auto confirm(const std::string &title, const std::string &caption) -> int;

    auto requestFileOpen(const std::string &caption, const std::string &extensions) -> std::string;
    auto requestFileSave(const std::string &caption, const std::string &extensions) -> std::string;
    bool _is_other_dbms_initialized;

#ifdef _MSC_VER
    auto wmiOpenSession(const std::string server, const std::string &user, const std::string &password) -> int;
    auto wmiCloseSession(int session) -> int;
    auto wmiQuery(int session, const std::string &query) -> grt::DictListRef;
    auto wmiServiceControl(int session, const std::string &service, const std::string &action) -> std::string;
    auto wmiSystemStat(int session, const std::string &what) -> std::string;

    auto wmiStartMonitoring(int session, const std::string &what) -> int;
    auto wmiReadValue(int monitor) -> std::string;
    auto wmiStopMonitoring(int monitor) -> int;

#endif

    auto create_connection(const std::string &host, const std::string &user,
                                            const std::string socket_or_pipe_name, int can_use_networking,
                                            int can_use_socket_or_pipe, int port, const std::string &name) -> db_mgmt_ConnectionRef;
    auto getLocalServerList() -> grt::DictListRef;
    auto createConnectionsFromLocalServers() -> int;
    auto createInstancesFromLocalServers() -> int;

    auto getVideoAdapter() -> std::string;
    auto getFullVideoAdapterInfo(bool indent) -> std::string;
    auto initializeOtherRDBMS() -> int;
    auto createSSHSession(const grt::ObjectRef &val) -> db_mgmt_SSHConnectionRef;
    auto deleteConnection(const db_mgmt_ConnectionRef &connection) -> int;
    auto deleteConnectionGroup(const std::string &group) -> int;
  };
}; // namespace wb

#endif
