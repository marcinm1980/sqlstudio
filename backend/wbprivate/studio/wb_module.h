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

  class SqlStudioImpl : public grt::ModuleImplBase, public PluginInterfaceImpl {
    typedef grt::ModuleImplBase super;

  public:
    SqlStudioImpl(grt::CPPModuleLoader *);
    virtual ~SqlStudioImpl();

    void set_context(WBContext *wb);
    std::string getSystemInfo(bool indent);
    std::map<std::string, std::string> getSystemInfoMap();
    int isOsSupported(const std::string &os);

    DEFINE_INIT_MODULE(
      WBModule_VERSION, "Oracle and/or its affiliates", grt::ModuleImplBase,
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::getPluginInfo),

      // Non-plugin functions
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::copyToClipboard),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::hasUnsavedChanges),

      // Model
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::newDocument),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::newDocumentFromDB), DECLARE_MODULE_FUNCTION(SqlStudioImpl::openModel),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::openRecentModel), DECLARE_MODULE_FUNCTION(SqlStudioImpl::saveModel),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::saveModelAs), DECLARE_MODULE_FUNCTION(SqlStudioImpl::exit),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::exportPNG), DECLARE_MODULE_FUNCTION(SqlStudioImpl::exportPDF),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::exportPS), DECLARE_MODULE_FUNCTION(SqlStudioImpl::exportSVG),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::activateDiagram),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::exportDiagramToPng),

      DECLARE_MODULE_FUNCTION(SqlStudioImpl::selectAll), DECLARE_MODULE_FUNCTION(SqlStudioImpl::selectSimilar),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::selectConnected),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::goToNextSelected),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::goToPreviousSelected),

      DECLARE_MODULE_FUNCTION(SqlStudioImpl::highlightFigure),

      DECLARE_MODULE_FUNCTION(SqlStudioImpl::editSelectedFigure),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::editSelectedFigureInNewWindow),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::editObject),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::editObjectInNewWindow),

      DECLARE_MODULE_FUNCTION(SqlStudioImpl::raiseSelection),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::lowerSelection),

      DECLARE_MODULE_FUNCTION(SqlStudioImpl::newDiagram),

      DECLARE_MODULE_FUNCTION(SqlStudioImpl::toggleGrid), DECLARE_MODULE_FUNCTION(SqlStudioImpl::togglePageGrid),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::toggleGridAlign),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::toggleFKHighlight),

      DECLARE_MODULE_FUNCTION(SqlStudioImpl::zoomIn), DECLARE_MODULE_FUNCTION(SqlStudioImpl::zoomOut),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::zoomDefault),

      DECLARE_MODULE_FUNCTION(SqlStudioImpl::setFigureNotation),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::setRelationshipNotation),

      DECLARE_MODULE_FUNCTION(SqlStudioImpl::setMarker), DECLARE_MODULE_FUNCTION(SqlStudioImpl::goToMarker),

      DECLARE_MODULE_FUNCTION(SqlStudioImpl::startTrackingUndo),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::finishTrackingUndo),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::cancelTrackingUndo),

      DECLARE_MODULE_FUNCTION(SqlStudioImpl::isOsSupported),

      DECLARE_MODULE_FUNCTION(SqlStudioImpl::addUndoListAdd),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::addUndoListRemove),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::addUndoObjectChange),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::addUndoDictSet),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::beginUndoGroup), DECLARE_MODULE_FUNCTION(SqlStudioImpl::endUndoGroup),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::setUndoDescription),

      DECLARE_MODULE_FUNCTION(SqlStudioImpl::createAttachedFile),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::setAttachedFileContents),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::getAttachedFileContents),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::getAttachedFileTmpPath),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::exportAttachedFileContents),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::openModelFile), DECLARE_MODULE_FUNCTION(SqlStudioImpl::closeModelFile),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::getDbFilePath), DECLARE_MODULE_FUNCTION(SqlStudioImpl::getTempDir),

      DECLARE_MODULE_FUNCTION(SqlStudioImpl::debugValidateGRT),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::getVideoAdapter),

      DECLARE_MODULE_FUNCTION(SqlStudioImpl::runScriptFile),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::installModuleFile),

      DECLARE_MODULE_FUNCTION(SqlStudioImpl::showUserTypeEditor),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::showDocumentProperties),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::showModelOptions), DECLARE_MODULE_FUNCTION(SqlStudioImpl::showOptions),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::showConnectionManager),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::showInstanceManagerFor),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::showInstanceManager),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::showQueryConnectDialog),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::saveConnections),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::saveInstances),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::refreshHomeConnections),

      DECLARE_MODULE_FUNCTION(SqlStudioImpl::showGRTShell), DECLARE_MODULE_FUNCTION(SqlStudioImpl::newGRTFile),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::openGRTFile),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::showPluginManager), DECLARE_MODULE_FUNCTION(SqlStudioImpl::reportBug),

      // Utilities
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::confirm), DECLARE_MODULE_FUNCTION(SqlStudioImpl::requestFileOpen),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::requestFileSave),

      DECLARE_MODULE_FUNCTION(SqlStudioImpl::createConnectionsFromLocalServers),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::createInstancesFromLocalServers),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::create_connection),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::initializeOtherRDBMS),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::deleteConnection),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::deleteConnectionGroup),
      DECLARE_MODULE_FUNCTION(SqlStudioImpl::createSSHSession));

  protected:
    virtual void initialization_done() override {
// Called after init_module (defined by DEFINE_INIT_MODULE above) is done.
// Here we register platform dependent functions.
#ifdef _MSC_VER
      register_functions(DECLARE_MODULE_FUNCTION(SqlStudioImpl::wmiOpenSession),
                         DECLARE_MODULE_FUNCTION(SqlStudioImpl::wmiCloseSession),
                         DECLARE_MODULE_FUNCTION(SqlStudioImpl::wmiQuery),
                         DECLARE_MODULE_FUNCTION(SqlStudioImpl::wmiServiceControl),
                         DECLARE_MODULE_FUNCTION(SqlStudioImpl::wmiSystemStat),
                         DECLARE_MODULE_FUNCTION(SqlStudioImpl::wmiStartMonitoring),
                         DECLARE_MODULE_FUNCTION(SqlStudioImpl::wmiReadValue),
                         DECLARE_MODULE_FUNCTION(SqlStudioImpl::wmiStopMonitoring), NULL);
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

    int copyToClipboard(const std::string &str);

    int hasUnsavedChanges();

    // file
    int newDocument();
    int newDocumentFromDB();
    int openModel(const std::string &path);
    int openRecentModel(const std::string &index);
    int saveModel();
    int saveModelAs(const std::string &path);
    int exportPNG(const std::string &filename);
    int exportPDF(const std::string &filename);
    int exportPS(const std::string &filename);
    int exportSVG(const std::string &filename);
    int activateDiagram(const model_DiagramRef &diagram);
    int exportDiagramToPng(const model_DiagramRef &diagram, const std::string &filename);
    int exit();

    // edit
    int selectAll();
    int selectSimilar();
    int selectConnected();

    int editSelectedFigure(const model_DiagramRef &view);
    int editSelectedFigureInNewWindow(const model_DiagramRef &view);

    int editObject(const GrtObjectRef &object);
    int editObjectInNewWindow(const GrtObjectRef &object);

    // canvas manipulation
    int raiseSelection(const model_DiagramRef &view);
    int lowerSelection(const model_DiagramRef &view);

    // view
    int newDiagram(const model_ModelRef &model);

    int toggleGrid(const model_DiagramRef &view);
    int togglePageGrid(const model_DiagramRef &view);
    int toggleGridAlign(const model_DiagramRef &view);
    int toggleFKHighlight(const model_DiagramRef &view);

    int zoomIn();
    int zoomOut();
    int zoomDefault();

    int goToNextSelected();
    int goToPreviousSelected();

    int setMarker(const std::string &marker);
    int goToMarker(const std::string &marker);

    int setFigureNotation(const std::string &name, studio_physical_ModelRef model);
    int setRelationshipNotation(const std::string &name, studio_physical_ModelRef model);

    int highlightFigure(const model_ObjectRef &figure);
    // undo
    int startTrackingUndo();
    int finishTrackingUndo(const std::string &description);
    int cancelTrackingUndo();

    int addUndoListAdd(const grt::BaseListRef &list);
    int addUndoListRemove(const grt::BaseListRef &list, int index);
    int addUndoObjectChange(const grt::ObjectRef &object, const std::string &member);
    int addUndoDictSet(const grt::DictRef &dict, const std::string &key);
    int beginUndoGroup();
    int endUndoGroup();
    int setUndoDescription(const std::string &text);

    // attached file management
    std::string createAttachedFile(const std::string &group, const std::string &tmpl);
    int setAttachedFileContents(const std::string &filename, const std::string &text);
    std::string getAttachedFileContents(const std::string &filename);
    std::string getAttachedFileTmpPath(const std::string &filename);
    int exportAttachedFileContents(const std::string &filename, const std::string &export_to);
    studio_DocumentRef openModelFile(const std::string &path);
    int closeModelFile();
    std::string getDbFilePath();
    std::string getTempDir();

    int runScriptFile(const std::string &filename);
    int installModuleFile(const std::string &filename);

    // debugging
    int debugValidateGRT();

    int showUserTypeEditor(const studio_physical_ModelRef &model);
    int showDocumentProperties();
    int showModelOptions(const studio_physical_ModelRef &model);
    int showOptions();
    int showConnectionManager();
    int showInstanceManagerFor(const db_mgmt_ConnectionRef &conn);
    int showInstanceManager();
    int showQueryConnectDialog();
    int saveConnections();
    int saveInstances();
    int showGRTShell();
    int showPluginManager();
    int newGRTFile();
    int openGRTFile();
    int reportBug(const std::string error_info = "");

    // UI
    int refreshHomeConnections();
    int confirm(const std::string &title, const std::string &caption);

    std::string requestFileOpen(const std::string &caption, const std::string &extensions);
    std::string requestFileSave(const std::string &caption, const std::string &extensions);
    bool _is_other_dbms_initialized;

#ifdef _MSC_VER
    int wmiOpenSession(const std::string server, const std::string &user, const std::string &password);
    int wmiCloseSession(int session);
    grt::DictListRef wmiQuery(int session, const std::string &query);
    std::string wmiServiceControl(int session, const std::string &service, const std::string &action);
    std::string wmiSystemStat(int session, const std::string &what);

    int wmiStartMonitoring(int session, const std::string &what);
    std::string wmiReadValue(int monitor);
    int wmiStopMonitoring(int monitor);

#endif

    db_mgmt_ConnectionRef create_connection(const std::string &host, const std::string &user,
                                            const std::string socket_or_pipe_name, int can_use_networking,
                                            int can_use_socket_or_pipe, int port, const std::string &name);
    grt::DictListRef getLocalServerList();
    int createConnectionsFromLocalServers();
    int createInstancesFromLocalServers();

    std::string getVideoAdapter();
    std::string getFullVideoAdapterInfo(bool indent);
    int initializeOtherRDBMS();
    db_mgmt_SSHConnectionRef createSSHSession(const grt::ObjectRef &val);
    int deleteConnection(const db_mgmt_ConnectionRef &connection);
    int deleteConnectionGroup(const std::string &group);
  };
};

#endif
