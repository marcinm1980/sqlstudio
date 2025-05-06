/**
 * Copyright (c) 2008, 2015, Oracle and/or its affiliates. All rights reserved.
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

using System;
using System.Collections;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;
using System.Windows.Forms;
using MySQL.Forms;
using MySQL.Utilities.SysUtils;
using MySQL.studio;
using MySqlStudio.X.Properties;
using MySqlStudio.X;

namespace MySqlStudio.X
{
  internal static class Program
  {
    #region Static Variables and Enums

    /// <summary>
    /// The types of application metadata information
    /// </summary>
    public enum ApplicationMetaInfo { Company, Copyright, Version, Revision, Configuration, ReleaseType };

    /// <summary>
    /// Flag to collect garbage
    /// </summary>
    private static bool _gcRequested;


    /// <summary>
    /// 
    /// </summary>
    public static void CollectGarbageOnIdle()
    {
      _gcRequested = true;
    }

    /// <summary>
    /// Application controller.
    /// </summary>
    private static ApplicationController _controller;

    #endregion

    /// <summary>
    /// Try to find the given file in known locations. If name is empty return the absolute application
    /// path (as common root for all resource subfolders).
    /// </summary>
    /// <param name="name"></param>
    static public string GetIconPath(string name)
    {
      string[] ResourcePaths =
      {
        "",
        "images/ui",
        "images/icons",
        "images/grt",
        "images/grt/structs",
        "images/cursors",
        "images/home",
        "images/sql",
        "images/sql/mac",
      };

      if (name == "")
        return Environment.CurrentDirectory;

      var result = "";
      if (File.Exists(name) || Directory.Exists(name))
        return name;
      foreach (String path in ResourcePaths)
        if (File.Exists(path + "/" + name))
        {
          result = path + "/" + name;
          break;
        }
      return result;
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="command"></param>
    /// <param name="str"></param>
    /// <returns></returns>
    public static String ApplicationCommand(AppCommand command, String str)
    {
      String result = "";
      switch (command)
      {
        case AppCommand.AppGetResourcePath:
          result = GetIconPath(str);
          break;
      }
      return result;
    }

    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    private static void Main(string[] args)
    {
      ManagedDelegate managedDelegate = new ManagedDelegate();
      // Connect the application to console to have proper output there if requested.
      var consoleRedirectionWorked = Win32Api.RedirectConsole();

      // Start with command line parsing.
      var userDir = Path.Combine(Path.Combine( Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData),
        "MySQL"), "studio");
      Logger.InitLogger(userDir);

      if (!consoleRedirectionWorked)
        Logger.LogError("studio", "Console redirection failed.\n");

      PrintInitialLogInfo();

      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault(false);

      // Hook into the exception handling to establish our own handling.
      AppDomain currentDomain = AppDomain.CurrentDomain; // CLR
      currentDomain.UnhandledException += OnUnhandledException;
      Application.ThreadException += OnGuiUnhandledException;

      // Give the main thread a proper name, so we can later check for it when needed.
      Thread.CurrentThread.Name = "mainthread";

      // Change the working dir to to application path.
      // This is necessary because all our internal data files etc. are located under the app dir
      // and WB could have been called from a different dir.
      var asm = Assembly.GetEntryAssembly();
      var baseDir = Path.GetDirectoryName(asm.Location);
      if (baseDir != null) 
        Directory.SetCurrentDirectory(baseDir);

      // Some people don't have c:\windows\system32 in PATH, so we need to set it here
      // for WBA to find the needed commands
      var systemFolder = Environment.GetFolderPath(Environment.SpecialFolder.System);
      var cleanedPath = Environment.GetEnvironmentVariable("PATH");
      if (cleanedPath != null)
      {
        var paths = cleanedPath.Split(';');
        cleanedPath = paths.Aggregate("", (current, path) => current + ";" + path);
      }

      Environment.SetEnvironmentVariable("PATH", systemFolder + cleanedPath);
      Logger.LogInfo("MySqlStudio.X", string.Format("Setting PATH to: {0}{1}{2}", systemFolder, cleanedPath, '\n'));

      // Initialize forms stuff.
      var formsManager = Manager.get_instance(); // Creates the singleton.

      _controller = new ApplicationController();
      if(_controller != null)
      {
        #region Initialize Callbacks and Mainform

        // Initialize the studio context
        var app = new ManagedApplication(ApplicationCommand, managedDelegate);
        _controller.init(baseDir, userDir);
        _controller.parse(args, Path.GetDirectoryName(asm.Location));

        // Set the Application.Idle event handler
        Application.Idle += OnApplicationIdle;

        // Setup Menus
        Logger.LogInfo("MySqlStudio.X", "UI is up\n");
        try
        {
          Logger.LogInfo("studio", "Running the application\n");
          Application.Run(new ApplicationContext());
        }
        catch (Exception e)
        {
          HandleException(e);
        }

        #endregion

        Logger.LogInfo("MySqlStudio.X", "Shutting down MySqlStudio.X\n");
        formsManager.Dispose();
        GC.Collect();
      }

      Win32Api.ReleaseConsole();
      Logger.LogInfo("MySQL.studio.X", "Done\n");
    }

    /// <summary>
    /// Prints some general info to the log file.
    /// </summary>
    private static void PrintInitialLogInfo()
    {
      Logger.LogInfo("MySqlStudio.X", "Starting up studio X\n");
      Logger.LogInfo("MySqlStudio.X", string.Format("Current environment:\n\tCommand line: {0}\n\tCurrentDirectory: {1}\n" +
        "\tHasShutdownStarted: {2}\n\tOSVersion: {3}\n\tSystemDirectory: {4}\n" +
        "\tTickCount: {5}\n\tUserInteractive: {6}\n\tVersion: {7}\n\tWorkingSet: {8}\n",
        Environment.CommandLine, Environment.CurrentDirectory, Environment.HasShutdownStarted,
        Environment.OSVersion, Environment.SystemDirectory,
        Environment.TickCount, Environment.UserInteractive,
        Environment.Version, Environment.WorkingSet));

      var environmentVariables = Environment.GetEnvironmentVariables();
      var variables = environmentVariables.Cast<DictionaryEntry>().Aggregate("", (current, entry) => current + string.Format("\t{0} = {1}\n", entry.Key, entry.Value));
      Logger.LogInfo("MySqlStudio.X", "Environment variables:\n" + variables);
    }

    #region Application handlers

    private static void OnApplicationIdle(object sender, EventArgs e)
    {
      if (!_gcRequested) 
        return;
      _gcRequested = false;
      GC.Collect();
    }

    /// <summary>
    /// CLR unhandled exception.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private static void OnUnhandledException(object sender, UnhandledExceptionEventArgs e)
    {
      HandleException(e.ExceptionObject);
    }

    /// <summary>
    /// Windows Forms unhandled exception.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private static void OnGuiUnhandledException(object sender, ThreadExceptionEventArgs e)
    {
      HandleException(e.Exception);
    }

    #endregion


    #region Various Static Functions

    /// <summary>
    /// Retrieves application metadata and returns it as a string
    /// </summary>
    /// <param name="kind">The type of metadata information to return</param>
    /// <returns></returns>
    public static string GetApplicationMetaInfo(ApplicationMetaInfo kind)
    {
      object[] attributes;
      Version version;
      var assembly = Assembly.GetExecutingAssembly();
      var value = "";

      switch (kind)
      {
        case ApplicationMetaInfo.Company:
          attributes = assembly.GetCustomAttributes(typeof(AssemblyCompanyAttribute), false);
          if (attributes.Length > 0)
          {
            var assemblyCompanyAttribute = attributes[0] as AssemblyCompanyAttribute;
            if (assemblyCompanyAttribute != null)
              value = assemblyCompanyAttribute.Company;
          }
          break;

        case ApplicationMetaInfo.Copyright:
          attributes = assembly.GetCustomAttributes(typeof(AssemblyCopyrightAttribute), false);
          if (attributes.Length > 0)
          {
            var assemblyCopyrightAttribute = attributes[0] as AssemblyCopyrightAttribute;
            if (assemblyCopyrightAttribute != null)
              value = assemblyCopyrightAttribute.Copyright;
          }
          break;

        case ApplicationMetaInfo.Configuration:
          attributes = assembly.GetCustomAttributes(typeof(AssemblyConfigurationAttribute), false);
          if (attributes.Length > 0)
          {
            var assemblyConfigurationAttribute = attributes[0] as AssemblyConfigurationAttribute;
            if (assemblyConfigurationAttribute != null)
              value = assemblyConfigurationAttribute.Configuration;
          }
          break;

        case ApplicationMetaInfo.Version:
          version = new Version(Application.ProductVersion);
          value = string.Format("{0}.{1}.{2}",
            version.Major, version.Minor, version.Build);
          break;

        case ApplicationMetaInfo.Revision:
          version = new Version(Application.ProductVersion);
          value = string.Format("{0}", version.MinorRevision);
          break;
        case ApplicationMetaInfo.ReleaseType:
          attributes = assembly.GetCustomAttributes(typeof(AssemblyReleaseTypeAttribute), false);
          if (attributes.Length > 0)
          {
            var assemblyReleaseTypeAttribute = attributes[0] as AssemblyReleaseTypeAttribute;
            if (assemblyReleaseTypeAttribute != null)
              value = assemblyReleaseTypeAttribute.ReleaseType;
          }
          break;
        default:
          throw new ArgumentOutOfRangeException("kind", kind, null);
      }

      return value;
    }

    /// <summary>
    /// Handle exception.
    /// </summary>
    /// <param name="o">Exception object.</param>
    private static void HandleException(object o)
    {
      var e = o as Exception;
      string message;
      string info;

      var isFontProblem = false;
      if (e != null)
      {
        // Report System.Exception info
        message = e.Message;
        info = "Exception = " + e.GetType() + "\n";
        info += "Message = " + e.Message + "\n";
        info += "FullText = " + e;
        isFontProblem = (e is ArgumentException) && message.Contains("'Regular'") && info.Contains(".CreateNativeFont");
      }
      else
      { // Report exception Object info
        message = "Exception = " + o.GetType();
        info = "Exception = " + o.GetType() + "\n";
        info += "FullText = " + o;
      }

      Logger.LogError("MySqlStudio.X", message + "\n" + info + '\n');

      // Check for blocked files (Windows "security" feature).
      if (info.Contains("0x80131515"))
      {
        MessageBox.Show(Resources.ProgramException_BlockedFiles,
          Resources.ProgramException_BlockedDllDetected, MessageBoxButtons.OK, MessageBoxIcon.Exclamation);

        Win32Api.UnblockstudioFiles(Directory.GetCurrentDirectory());
      }
      else
      {
        // There's a relatively common error with missing 'Regular' style even on system
        // standard fonts, which is extremely weird, but not caused by WB. However, we show
        // a hint and close WB, so the user knows he can fix it and doesn't enter new bug reports for this.
        if (isFontProblem)
        {
          MessageBox.Show(string.Format(Resources.ProgramException_FontProblem, message),
          Resources.ProgramException_FontProblemDescription, MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
          Application.Exit();
        }
        else
          ExceptionDialog.Show(message, info, _controller);
      }
    }

    #endregion
  }
}


