/*
 * Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#import "ApplicationController.h"

#include "base/log.h"
#include "base/data_types.h"
#include "base/string_utilities.h"

#include "mforms/mforms.h"
#include "mforms/view.h"
#include "mforms/box.h"

#include "ApplicationBackend.h"

// for _NSGetArg*
#include <crt_externs.h>

DEFAULT_LOG_DOMAIN("studio.X")

using namespace studio::X;

static NSString *applicationSupportFolder()
{
  NSArray *res = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
  if (res.count > 0)
    return res[0];
  return @"/tmp/";
}

extern "C" {
  extern void mforms_cocoa_init();
  extern void mforms_cocoa_check();
};


//--------------------------------------------------------------------------------------------------

@interface ApplicationController () {
@public
  int _eventLoopRetCode;
}

@end

class MainWindowDockingPoint : public mforms::DockingPointDelegate
{
  ApplicationController *controller;

public:
  MainWindowDockingPoint(ApplicationController *aController) : controller(aController) {}

  virtual std::string get_type()
  {
    return "ApplicationController";
  }

  virtual void dock_view(mforms::AppView *view, const std::string &arg1, int arg2)
  {
  }

  virtual bool select_view(mforms::AppView *view)
  {
    return false;
  }

  virtual void undock_view(mforms::AppView *view)
  {
  }

  virtual void set_view_title(mforms::AppView *view, const std::string &title)
  {
  }

  virtual std::pair<int, int> get_size()
  {
    NSRect rect = controller.mainWindow.frame;
    return { NSWidth(rect), NSHeight(rect) };
  }

  virtual int view_count()
  {
    return 0;
  }

  virtual mforms::AppView *selected_view()
  {
    return nullptr;
  }

  virtual mforms::AppView *view_at_index(int index)
  {
    // No tab interface here, so no view at a given index.
    return nullptr;
  }
};

//--------------------------------------------------------------------------------------------------

static void set_status_text(mforms::App *app, const std::string &text)
{
}

//--------------------------------------------------------------------------------------------------

static std::string get_resource_path(mforms::App *app, const std::string &file)
{
  if (file.empty())
    return NSBundle.mainBundle.resourcePath.fileSystemRepresentation;

  if (file[0] == '/')
    return file;

  std::string path;
  if (base::hasSuffix(file, ".png"))
  {
    path = bec::IconManager::get_instance()->get_icon_path(file);
    if (!path.empty())
      return path;
  }

  {
    std::string fn = base::basename(file);
    NSString *filename = [NSString stringWithUTF8String: fn.c_str()];

    NSString *str = [[NSBundle mainBundle] pathForResource: filename.stringByDeletingPathExtension
                                                    ofType: filename.pathExtension];
    if (str != nil)
      return str.fileSystemRepresentation;

    // Look for the same image but with tiff extension, in case the actual image is
    // a combined art file.
    str = [[NSBundle mainBundle] pathForResource: filename.stringByDeletingPathExtension
                                          ofType: @"tiff"];
    if (str != nil)
      return str.fileSystemRepresentation;
  }
  return "";
}

//--------------------------------------------------------------------------------------------------

static std::string get_executable_path(mforms::App *app, const std::string &file)
{
  if (file.empty())
    return NSBundle.mainBundle.executablePath.stringByDeletingLastPathComponent.fileSystemRepresentation;

  if (!file.empty() && file[0] == '/')
    return file;

  {
    std::string fn = base::basename(file);
    NSString *filename = [NSString stringWithUTF8String: fn.c_str()];

    NSString *path = [NSBundle.mainBundle.executablePath stringByDeletingLastPathComponent];
    return [path stringByAppendingPathComponent: filename].fileSystemRepresentation;
  }
  return "";
}


//--------------------------------------------------------------------------------------------------

static base::Rect get_main_window_bounds(mforms::App *app)
{
  ApplicationController *controller = app->get_data();

  NSRect r = controller.mainWindow.frame;
  return base::Rect(NSMinX(r), NSMaxY(r), NSWidth(r), NSHeight(r));
}

//--------------------------------------------------------------------------------------------------

static int enter_event_loop(mforms::App *app, float timeout)
{
  ApplicationController *controller = app->get_data();
  if (controller != nil)
  { // XXX: reconsider if this hack is still needed.
    NSDate *later = timeout > 0.0 ? [NSDate dateWithTimeIntervalSinceNow: timeout] : [NSDate distantFuture];
    controller->_eventLoopRetCode = -0xdead1009;

    while (controller->_eventLoopRetCode == -0xdead1009)
    {
      NSEvent *e = [NSApp nextEventMatchingMask: NSAnyEventMask
                                      untilDate: later
                                         inMode: NSDefaultRunLoopMode
                                        dequeue: YES];

      if (e != nil)
        [NSApp sendEvent: e];
      else
        if ([[NSDate date] earlierDate: later] == later)
        {
          controller->_eventLoopRetCode = -1;
          break;
        }
    }

    return controller->_eventLoopRetCode;
  }
  return -1;
}

//--------------------------------------------------------------------------------------------------

static void exit_event_loop(mforms::App *app, int retcode)
{
  ApplicationController *controller = app->get_data();
  if (controller != nullptr)
    controller->_eventLoopRetCode = retcode;
}

//--------------------------------------------------------------------------------------------------

static float backing_scale_factor(mforms::App *app)
{
  ApplicationController *controller = app->get_data();
  if ([controller.mainWindow respondsToSelector: @selector(backingScaleFactor)])
    return [controller.mainWindow backingScaleFactor];
  return 1.0;
}

//--------------------------------------------------------------------------------------------------

// This is our main bridge between the Swift platform layer and the C++ backend.
@implementation ApplicationController

@synthesize mainWindow;

- (nullable instancetype)init {
  self = [super init];
  if (self != nil) {
    [self setup];
  }
  return self;
}

- (void)initializeForms
{
  mforms_cocoa_init();
  mforms::ControlFactory *cf = mforms::ControlFactory::get_instance();

  mforms::App::instantiate(new MainWindowDockingPoint(self), true);
  mforms::App::get()->set_data(self);

  cf->_app_impl.get_resource_path = get_resource_path;
  cf->_app_impl.get_executable_path = get_executable_path;
  cf->_app_impl.set_status_text = set_status_text;
  cf->_app_impl.get_application_bounds = get_main_window_bounds;
  cf->_app_impl.enter_event_loop = enter_event_loop;
  cf->_app_impl.exit_event_loop = exit_event_loop;
  cf->_app_impl.backing_scale_factor = backing_scale_factor;

  mforms_cocoa_check();
}

- (BOOL)setup
{
  base::Logger([applicationSupportFolder() stringByAppendingString: @"/MySQL/studio"].fileSystemRepresentation);
  logDebug("Initializing mforms\n");

  [self initializeForms];

  dataTypes::AppOptions options;
  //options.userDataDir = std::string(g_get_home_dir()).append("/.mysql/studio");
  options.basedir = NSBundle.mainBundle.resourcePath.fileSystemRepresentation;
  options.pluginSearchPath = NSBundle.mainBundle.builtInPlugInsPath.fileSystemRepresentation;
  options.structSearchPath = options.basedir + "/grt";
  options.moduleSearchPath = NSBundle.mainBundle.builtInPlugInsPath.fileSystemRepresentation + std::string(":") +
    NSBundle.mainBundle.resourcePath.fileSystemRepresentation + "/plugins";
  options.librarySearchPath = NSBundle.mainBundle.resourcePath.fileSystemRepresentation + std::string("/libraries");

  base::threading_init();
  ApplicationBackend::get().quitCallback = []() { // Called when all forms have been closed.
    [NSApplication.sharedApplication terminate: nil];
  };
  ApplicationBackend::get().initialize(options);

  int argc = *_NSGetArgc();
  char **argv = *_NSGetArgv();

  int retval;
  if (!ApplicationBackend::get().parseParams(argc, argv, &retval))
    return false;

  ApplicationBackend::get().start();

  return true;
}

- (NSView *)createHomeScreen
{
  if (!ApplicationBackend::get().showHomeScreen())
    return nullptr;

  mforms::View *view = nullptr;
  try {
    view = ApplicationBackend::get().getHomeScreen();
    return view->get_data();
  }
  catch (grt::user_cancelled &g)
  {
    mforms::Utilities::show_message("Aborted", "User canceled operation", "Ok", "", "");
  }

  return nullptr;
}

@end