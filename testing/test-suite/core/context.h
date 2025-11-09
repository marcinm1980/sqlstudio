/*
 * Copyright (c) 2025, dev4fun. All rights reserved.
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
#include <string>
#include <map>
#include <mutex>
namespace rapidjson {
  // class Document;
  // class Value;
} // namespace rapidjson

namespace testing {

  // Test execution context singleton.
  // Provides access to configuration and directories.
  class Context {
  public:
    virtual ~Context();

    // Returns the single global instance (thread-safe since C++11).
    static Context& get();

    // Non-copyable / non-movable to enforce singleton semantics.
    Context(Context const&) = delete;
    Context& operator=(Context const&) = delete;
    Context(Context&&) = delete;
    Context& operator=(Context&&) = delete;
    //void addInitializer(DescribeInit* initializer);

    std::string baseDir() {
      return _baseDir;
    };

    std::string outputDir() {
      return _baseDir + "/output";
    }
    std::string tmpDataDir() {
      return _baseDir + "/tmpdata";
    }

    std::string getConfigurationStringValue(std::string const& path, std::string const& defaultValue = "") const;
    int getConfigurationIntValue(std::string const& path, int defaultValue = 0) const;
    double getConfigurationDoubleValue(std::string const& path, double defaultValue = 0.0) const;
    bool getConfigurationBoolValue(std::string const& path, bool defaultValue = false) const;

  protected:
    Context();

  private:
    std::map<std::string, std::string> settings;
    // Opaque pointer to implementation-specific configuration document to
    // avoid exposing rapidjson types in the public header.
    void* _configuration_impl = nullptr; // allocated/deleted in .cpp
    // These objects create the actual test object on demand.
    // std::vector<DescribeInit*> _initializers;

    // std::vector<std::unique_ptr<Reporter>> _reporters;
    // Describe* _currentDescribe = nullptr;
    std::string _baseDir;

    std::mutex _resultMutex; // Synchronizes result recording across threads.

    // Ping support, to ensure regular output also for long lasting tests.
    // std::atomic<bool> _stopPing; // Set to true when the ping thread has to stop.
    // size_t _pingCount = 0;       // Counter for intermittant line breaks and the initial message.
    // std::timed_mutex _pingMutex; // The "semaphore" to signal new output.
    // std::thread _pingThread;     // The background thread to print the ping dots.

    // void startPing();
    // void resetPing();

    // void getConfigValueFromPath(std::string const& path) const;

    // The specs that were given on the command line.
    //std::vector<std::string> _specForceList;
  };
} // namespace testing