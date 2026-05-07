#pragma once

#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include <variant>

namespace casmine {

std::string getEnvVar(const std::string &name, const std::string &defaultValue = "");

using SettingValue = std::variant<bool, std::int64_t, std::string>;

struct EnvironmentBase {
  virtual ~EnvironmentBase() = default;
};

class TestContext final {
public:
  static TestContext *get();

  std::string tmpDataDir();
  std::string getConfigurationStringValue(const std::string &key, const std::string &defaultValue = "");
  std::int64_t getConfigurationIntValue(const std::string &key, std::int64_t defaultValue = 0);
  bool getConfigurationBoolValue(const std::string &key, bool defaultValue = false);

  void setConfigurationValue(const std::string &key, const std::string &value);
  void setConfigurationValue(const std::string &key, const char *value);
  void setConfigurationValue(const std::string &key, std::int64_t value);
  void setConfigurationValue(const std::string &key, bool value);

  std::map<std::string, SettingValue> settings;

private:
  TestContext();
  static std::int64_t getPortFromEnvironment();

  mutable std::mutex _mutex;
  std::map<std::string, SettingValue> _configuration;
  std::string _tmpDataDir;
};

using CasmineContext = TestContext;

}