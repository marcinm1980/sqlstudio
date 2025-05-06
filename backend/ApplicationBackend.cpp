#define BOOST_ERROR_CODE_HEADER_ONLY
#include <boost/system/error_code.hpp>

#include "base/data_types.h"
#include "mforms/view.h"
#include "backend/ng_ide.h"
#include "ng_public_interface.h"

#include "ApplicationBackend.h"
#include "base/log.h"
#include "base/string_utilities.h"
#include "base/file_utilities.h"
#include "mforms/app.h"
#include "mforms/box.h"
#include "base/session_wrapper.h"
#include "editors/code_editor_base.h"
#include "mdc_image_manager.h"


DEFAULT_LOG_DOMAIN("ApplicationBackend")

using namespace studio::X;

#ifdef _WIN32
# define OPPREFIX "-"
#else
# define OPPREFIX "--"
#endif

extern void registerAllMetaclasses();

ApplicationBackend& ApplicationBackend::get()
{
  static ApplicationBackend backend;
  return backend;
}

ApplicationBackend::ApplicationBackend() : _argv0(nullptr), _ide(nullptr), _registeredMetaclasses(false), _homeScreen(nullptr),
    _ngConnectionsSection(nullptr) {

  bec::GRTManager::get(); // force GRTManager to be created before ApplicationBackend
  mforms::App::get(); //force mforms::App to be created before ApplicationBackend
#ifdef _WIN32
  CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
#endif
}

ApplicationBackend::~ApplicationBackend()
{
  if (_ide != nullptr)
    delete _ide;

  if (_homeScreen != nullptr)
    _homeScreen->release();

  for (auto &it: _forms)
    delete it;

  _forms.clear();
}

//-----------------------------------------------------------

void ApplicationBackend::initialize(const dataTypes::AppOptions &opts)
{
  _options = opts;
  // register GRT object class implementations
   if (!_registeredMetaclasses)
   {
     _registeredMetaclasses= true;
     registerAllMetaclasses();
   }

  grt::GRT::get()->set_global_module_data_path("/wb/customData");
  grt::GRT::get()->set_document_module_data_path("/wb/doc/customData");
  bec::GRTManager::get().set_datadir(opts.basedir);
  bec::GRTManager::get().set_basedir(opts.basedir);
  bec::GRTManager::get().set_user_datadir(opts.userDataDir);
  bec::GRTManager::get().cleanup_tmp_dir();
  mforms::App::get()->set_user_data_folder_path(opts.userDataDir);
  mforms::Utilities::set_message_answers_storage_path(base::makePath(opts.userDataDir, "mforms_remembered_dialog_responses"));
  bec::IconManager::get_instance()->set_basedir(opts.basedir);
  // Setup image search paths
  std::string path;
  static const char* dirs[]= {
    "images",
    "images/icons",
    "images/grt",
    "images/grt/structs",
    "images/png",
#ifdef _WIN32
    "images/home",
#endif
    "images/ui",
    "images/sql",
    "images/sql/mac",
    "",
    NULL
  };

  for (unsigned int i= 0; dirs[i]!=NULL; i++)
  {
    mdc::ImageManager::get_instance()->add_search_path(base::makePath(opts.basedir, dirs[i]));
    bec::IconManager::get_instance()->add_search_path(dirs[i]);
  }


  bec::GRTManager::get().set_search_paths(
      opts.moduleSearchPath,
      base::pathlistPrepend(opts.structSearchPath, base::makePath(opts.basedir, "structs")),
      opts.librarySearchPath);

  bec::GRTManager::get().initialize(false, opts.pluginSearchPath);




  grt::DictRef root(true);
    studio_studioRef app(grt::Initialized);

    _wb_root= app;

    root.set("wb", app);

    // setup application subtree
    {
      app_InfoRef info(grt::Initialized);
      GrtVersionRef version(grt::Initialized);
      info->owner(app);

      version->majorNumber(1);
      version->minorNumber(1);
      version->releaseNumber(1);
      version->buildNumber(1);
      version->status(1);

      info->name("MySQL studio");
      info->version(version);
      info->copyright("Oracle and/or its affiliates");
      info->license("GPL");
      info->edition("Development");
      app->info(info);
    }


    {
      app_RegistryRef registry(grt::Initialized);
      registry->owner(app);
      registry->appDataDirectory(bec::GRTManager::get().get_basedir());
      registry->appExecutablePath(_argv0 ? _argv0 : "");

      app->registry(registry);
    }

    // ------------------

    db_mgmt_ManagementRef mgmt_info(grt::Initialized);

    // load datatype groups from XML

    grt::ListRef<db_DatatypeGroup> grouplist;

    std::shared_ptr<grt::internal::Unserializer> unserializer = grt::GRT::get()->get_unserializer();
    grouplist= grt::ListRef<db_DatatypeGroup>::cast_from(grt::GRT::get()->unserialize(base::makePath(opts.basedir, "data/db_datatype_groups.xml"), unserializer));
    for (size_t c= grouplist.count(), i= 0; i < c; i++)
    {
      grouplist[i]->owner(mgmt_info);
      mgmt_info->datatypeGroups().insert(grouplist[i]);
    }
    app->rdbmsMgmt(mgmt_info);

    grt::GRT::get()->set_root(root);

    db_mgmt_RdbmsRef rdbms= db_mgmt_RdbmsRef::cast_from(grt::GRT::get()->unserialize(base::makePath(bec::GRTManager::get().get_basedir(), "modules/data/mysql_rdbms_info.xml")));

    studio_studioRef::cast_from(grt::GRT::get()->get("/wb"))->rdbmsMgmt()->rdbms().insert(rdbms);
}

//-----------------------------------------------------------

bool ApplicationBackend::checkArgWithValue(char **argv, int &argi, const char *arg, char *&value)
{
  char *a;
  if (strncmp(argv[argi], OPPREFIX, sizeof(OPPREFIX)-1) == 0)
    a = argv[argi] + sizeof(OPPREFIX)-1;
  else
    return false;

  if (strcmp(a, arg) == 0)
  {                                                
    // value must be in next arg
    if (argv[argi+1] != NULL)
    {
      ++argi;
      value = argv[argi];
    }
    else
      value = NULL;
    return true;
  }
  else if (strncmp(a, arg, strlen(arg)) == 0 && a[strlen(arg)] == '=')
  {
    // value must be after =
    value = a + strlen(arg)+1;
    return true;
  }
  return false;
}

bool ApplicationBackend::parseParams(int argc, char **argv, int *retval)
{
  _argv0 = argv[0];

  logInfo("Parsing application arguments.\n");
  for (int j = 0; j < argc; j++)
    logInfo("    %s\n", argv[j]);

  char *argval = NULL;
  bool log_level_set = false;
  int i = 1;
  while (i < argc)
  {
    int start_index = i; // Keep the current index in case we check further entries and need it for
                             // error messages.

    if (strcmp(argv[i], OPPREFIX"help") == 0 || strcmp(argv[i], "-h") == 0)
    {
      showHelp(_argv0);
      if (retval)
        *retval = 0;
      return false;
    }
    else if (strncmp(argv[i], OPPREFIX"log-level", 10) == 0)
    {
      if (!parseLoglevel(argv[i]) && (i+1) < argc) // If parse failed try to add next arg from CLI
      {
        std::string line(argv[i]);
        line += argv[i + 1];

        if (!parseLoglevel(line))
        {
          if ( (i + 2) < argc ) // Yet, we may have three CLI args if it was written like --log-level = <level>. Handle extra spaces
          {
            line += argv[i+2];
            if (parseLoglevel(line))
            {
              i += 2; // correct arg count, so we do not parse log-level parts as smth different
              log_level_set = true;
            }
          }
        }
        else
        {
          ++i; // correct arg count, so we do not parse log-level parts as smth different
          log_level_set = true;
        }
      }
      else // Parse succeeded
        log_level_set = true;
    }
    else if (!strncmp(argv[i], OPPREFIX"log-to-stderr", sizeof(OPPREFIX"log-to-stderr")))
    {
        base::Logger::log_to_stderr(true);
    }
    else if (checkArgWithValue(argv, i, "open", argval))
    {
      if (argval)
      {
        _initialProject = ng::getSessionByUUID(argval);
        if (!_initialProject.isValid())
        {
          printf("%s: There's no such session for uuid: %s\n", argv[0], argval);
          *retval = 2;
          return false;
        }
      }
      else
      {
        printf("%s: Missing argument for option %s\n", argv[0], argv[start_index]);
        if (retval)
          *retval = 1;
        return false;
      }
    }
    i++;
  }

  // Set the log level from environment var WB_LOG_LEVEL if specified or set a default log level.
  if (!log_level_set)
  {
    const char* log_setting = NULL;// getenv("WB_LOG_LEVEL");
    if (log_setting == NULL)
    {
      #if defined(_DEBUG) || defined(ENABLE_DEBUG)
      log_setting = "debug2";
      #else
      log_setting = "info";
      #endif
    }
    else
      log_level_set = true;

    std::string level = base::tolower(log_setting);
    base::Logger::active_level(level);
  }

  if (log_level_set)
  {
    logInfo("Logger set to level '%s'\n", base::Logger::active_level().c_str());
    base::Logger::setLogLevelSpecifiedByUser();
  }

  return true;
}

void ApplicationBackend::showHelp(const char* arg0)
{
  const char *p = strrchr(arg0, '/');
  if (p)
    arg0 = p+1;
  p = strrchr(arg0, '\\');
  if (p)
    arg0 = p+1;

  printf("%s [<options>] [<attribute>]\n", arg0);
  printf("Options:\n");

  printf("  %sopen <uuid>           Open connection specified by uuid\n", OPPREFIX);
  printf("  %slog-to-stderr         Also log to stderr\n", OPPREFIX);
  printf("  %shelp, -h              Show command line options and exit\n", OPPREFIX);
  printf("  %slog-level=<level>     Valid levels are: error, warning, info, debug1, debug2, debug3\n", OPPREFIX);

}

bool ApplicationBackend::parseLoglevel(const std::string& line)
{
  bool ret = false;

  const size_t eq_char_pos = line.find("=");
  if (eq_char_pos != std::string::npos)
  {
    std::string level = line.substr(eq_char_pos + 1);
    level = base::tolower(level);
    ret = base::Logger::active_level(level);
    if (ret)
      printf("Logger set to level '%s'. '%s'\n", level.c_str(), base::Logger::get_state().c_str());
  }

  return ret;
}

void ApplicationBackend::openNgProject(const dataTypes::XProject &project)
{
  // First we get a view, so the pw dialog is shown when homescreen is still there.
  mforms::View *view = nullptr;
  try {
    if (_ide == nullptr)
      _ide = new ng::NgIDE();

    for (auto &form : _forms) {
      ng::NgSheet *sheet = dynamic_cast<ng::NgSheet*>(form->get_content());
      if (sheet == nullptr)
        continue;

      if (sheet->getConnection().uuid == project.connection.uuid)
      {
        form->show(true);
        sheet->setupFinished();
        return;
      }
    }
    view = _ide->openInstance(project.connection, ng::EditorLanguage::ECMA);
    view->set_size(1000, 800);

    mforms::Form *form = new mforms::Form(nullptr, (mforms::FormFlag) (mforms::FormNormal));
    form->set_title(project.name);
    form->set_name("NgSheet");
    form->set_content(view);
    form->show(true);
    form->center();
    ng::NgSheet *sheet = dynamic_cast<ng::NgSheet*>(view);
    if (sheet != nullptr)
      sheet->setupFinished();
    form->signal_closed()->connect(boost::bind(&ApplicationBackend::formCloseHandler, this, form));
    base::MutexLock lock(_formListProtector);
    _forms.push_back(form);

  } catch (grt::user_cancelled &/*uc*/)
  {
    if (view != nullptr)
      view->release();
    throw;
  }
}

void ApplicationBackend::handleHomeContextMenu(const base::any &object, const std::string &action)
{
  // noop
}

void ApplicationBackend::homeActionCallback(mforms::HomeScreenAction action, const base::any &object)
{
  switch (action) {
    case mforms::HomeScreenAction::ActionOpenXConnection:
    {
      dataTypes::XProject project = object;
      try
      {
        openNgProject(project);
      } catch (grt::user_cancelled &)
      {
        logInfo("User cancel pw dialog\n");
      }
      break;
    }

    case mforms::HomeScreenAction::ActionNewXConnection:
      break;

    case mforms::HomeScreenAction::ActionManageXConnections:
      break;

    case mforms::HomeScreenAction::ActionOpenXTutorial:
      mforms::Utilities::open_url("http://dev.mysql.com/doc/refman/5.7/en/mysql-shell-tutorial-javascript.html");
      break;

    case mforms::HomeScreenAction::ActionOpenXLearnMore:
      mforms::Utilities::open_url("http://dev.mysql.com/doc/refman/5.7/en/document-store.html");
      break;

    case mforms::HomeScreenAction::ActionOpenXTraditional:
#ifdef _WIN32
      base::launchApplication("MySqlStudio.exe", { "" });
#elif __APPLE__
      base::launchApplication("MySqlStudio.app", { "" });
#else
      base::launchApplication("MySqlStudio", { "" });
#endif
      break;

    default:
      break;
  }
}

mforms::View *ApplicationBackend::getHomeScreen()
{
  if (_homeScreen != nullptr)
    return _homeScreen;

  _homeScreen = mforms::manage(new mforms::HomeScreen(true));
  _homeScreen->onHomeScreenAction = std::bind(&ApplicationBackend::homeActionCallback, this, std::placeholders::_1, std::placeholders::_2);
  _homeScreen->handleContextMenu = std::bind(&ApplicationBackend::handleHomeContextMenu, this, std::placeholders::_1, std::placeholders::_2);

  // now we have to add sections
  _ngConnectionsSection = new mforms::XConnectionsSection(_homeScreen);
  _ngConnectionsSection->set_name("Home X Connections Section");
  _homeScreen->addSection(_ngConnectionsSection);
  _ngConnectionsSection->loadProjects(ng::loadNgSessions());

  return _homeScreen;
}

bool ApplicationBackend::showHomeScreen()
{
  return !_initialProject.isValid();
}

void ApplicationBackend::start()
{
  if (showHomeScreen())
  {
    mforms::Form *form = new mforms::Form(nullptr, (mforms::FormFlag) (mforms::FormNormal));
    form->set_title("MySQL studio");
    form->set_name("HomeScreen");
    form->set_content(getHomeScreen());
    form->set_size(960, 800);
    form->show(true);
    form->center();
    form->signal_closed()->connect(boost::bind(&ApplicationBackend::formCloseHandler, this, form));
    base::MutexLock lock(_formListProtector);
    _forms.push_back(form);
  }
  else
  {
    try
    {
      openNgProject(_initialProject);
    } catch (grt::user_cancelled &)
    {
      logInfo("User cancel pw dialog\n");
      // If we're here, it means someone tried to start X with --open param and canelled pw dialog,
      // we have to call quitCallback so Linux and Win will properly close application
      if (quitCallback)
        quitCallback();
    }
  }
}

void ApplicationBackend::formCloseHandler(mforms::Form *frm)
{
  base::MutexLock lock(_formListProtector);
  _forms.erase(std::remove_if(_forms.begin(), _forms.end(), [&frm](mforms::Form *f){
      if (frm == f)
      {
        delete f;
        return true;
      }
      return false;
    }));

  if (_forms.empty() && quitCallback)
    quitCallback();
}
