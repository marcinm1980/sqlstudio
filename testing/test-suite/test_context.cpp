#include "test_context.h"

#include <cstdlib>
#include <filesystem>
#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

#include "helpers.h"

namespace casmine {

std::string getEnvVar(const std::string &name, const std::string &defaultValue) {
  return testing::getEnvVar(name, defaultValue);
}

TestContext *TestContext::get() {
  static TestContext instance;
  return &instance;
}

std::string TestContext::tmpDataDir() {
  std::lock_guard<std::mutex> guard(_mutex);
  return _tmpDataDir;
}

std::string TestContext::getConfigurationStringValue(const std::string &key, const std::string &defaultValue) {
  std::lock_guard<std::mutex> guard(_mutex);
  auto iterator = _configuration.find(key);
  if (iterator == _configuration.end())
    return defaultValue;

  if (auto value = std::get_if<std::string>(&iterator->second))
    return *value;
  if (auto value = std::get_if<std::int64_t>(&iterator->second))
    return std::to_string(*value);
  if (auto value = std::get_if<bool>(&iterator->second))
    return *value ? "true" : "false";

  return defaultValue;
}

std::int64_t TestContext::getConfigurationIntValue(const std::string &key, std::int64_t defaultValue) {
  std::lock_guard<std::mutex> guard(_mutex);
  auto iterator = _configuration.find(key);
  if (iterator == _configuration.end())
    return defaultValue;

  if (auto value = std::get_if<std::int64_t>(&iterator->second))
    return *value;
  if (auto value = std::get_if<std::string>(&iterator->second)) {
    try {
      return std::stoll(*value);
    } catch (...) {
      return defaultValue;
    }
  }
  if (auto value = std::get_if<bool>(&iterator->second))
    return *value ? 1 : 0;

  return defaultValue;
}

bool TestContext::getConfigurationBoolValue(const std::string &key, bool defaultValue) {
  std::lock_guard<std::mutex> guard(_mutex);
  auto iterator = _configuration.find(key);
  if (iterator == _configuration.end())
    return defaultValue;

  if (auto value = std::get_if<bool>(&iterator->second))
    return *value;
  if (auto value = std::get_if<std::int64_t>(&iterator->second))
    return *value != 0;
  if (auto value = std::get_if<std::string>(&iterator->second))
    return *value == "1" || *value == "true" || *value == "TRUE";

  return defaultValue;
}

void TestContext::setConfigurationValue(const std::string &key, const std::string &value) {
  std::lock_guard<std::mutex> guard(_mutex);
  _configuration[key] = value;
}

void TestContext::setConfigurationValue(const std::string &key, const char *value) {
  setConfigurationValue(key, value == nullptr ? std::string() : std::string(value));
}

void TestContext::setConfigurationValue(const std::string &key, std::int64_t value) {
  std::lock_guard<std::mutex> guard(_mutex);
  _configuration[key] = value;
}

void TestContext::setConfigurationValue(const std::string &key, bool value) {
  std::lock_guard<std::mutex> guard(_mutex);
  _configuration[key] = value;
}

TestContext::TestContext() {
  settings["verbose"] = getEnvVar("TEST_VERBOSE", "0") == "1";

  _configuration.emplace("db/host", getEnvVar("TEST_DB_HOST", "127.0.0.1"));
  _configuration.emplace("db/port", getPortFromEnvironment());
  _configuration.emplace("db/user", getEnvVar("TEST_DB_USER", "root"));
  _configuration.emplace("db/password", getEnvVar("TEST_DB_PASSWORD", ""));

  auto configuredTempPath = getEnvVar("TEST_DATA_DIR", "");
  if (!configuredTempPath.empty()) {
    _tmpDataDir = std::string(configuredTempPath);
  } else {
    std::filesystem::path basePath;
    try {
      basePath = std::filesystem::temp_directory_path();
    } catch (...) {
      basePath = std::filesystem::current_path();
    }

    auto processId = std::to_string(
#ifdef _WIN32
      static_cast<unsigned long>(::_getpid())
#else
      static_cast<long>(::getpid())
#endif
    );
    _tmpDataDir = (basePath / ("mysqlstudio-test-suite-" + processId)).string();
  }

  std::error_code error;
  std::filesystem::create_directories(_tmpDataDir, error);
}

std::int64_t TestContext::getPortFromEnvironment() {
  auto value = getEnvVar("TEST_DB_PORT", "3306");
  try {
    return std::stoll(value);
  } catch (...) {
    return 3306;
  }
}

}