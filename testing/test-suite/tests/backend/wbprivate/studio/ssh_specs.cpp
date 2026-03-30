/*
 * Copyright (c) 2019, Oracle and/or its affiliates. All rights reserved.
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

#include "base/util_functions.h"
#include "base/file_utilities.h"
#include "base/string_utilities.h"

#include "SSHCommon.h"
#include "SSHTunnelManager.h"
#include "studio/SSHSessionWrapper.h"
#include "SSHSftp.h"
#include "cdbc/src/driver_manager.h"
#include "grtpp_util.h"
#include "helpers.h"
#include "wb_test_helpers.h"

#include <cppconn/statement.h>
#include <cppconn/resultset.h>

#include "gtest/gtest.h"
#include "context.h"

namespace {

struct TestData {
  ssh::SSHConnectionConfig connectionConfig;
  ssh::SSHConnectionCredentials connectionCredentials;

  testing::Context *context = &testing::Context::get();
};

class SSHTestingTest : public ::testing::Test {
protected:
  TestData *data = new TestData();

  void SetUp() override {
    data->connectionConfig.localhost = "127.0.0.1";
    data->connectionConfig.remoteSSHhost = data->context->getConfigurationStringValue("ssh/host", "127.0.0.1");
    data->connectionConfig.remoteSSHport = data->context->getConfigurationIntValue("ssh/port", 22);
    data->connectionConfig.connectTimeout = 10;
    data->connectionConfig.optionsDir = data->context->getConfigurationStringValue("ssh/optionsdir");
    data->connectionCredentials.username = data->context->getConfigurationStringValue("ssh/user");
    data->connectionCredentials.password = data->context->getConfigurationStringValue("ssh/password");
  }

  void TearDown() override {
    delete data;
  }
};

  TEST_F(SSHTestingTest, DISABLED_PerformsConnection) {
    auto file = base::makeTmpFile("/tmp/known_hosts");
    std::string knownHosts = file.getPath();
    file.dispose();
    auto config = data->connectionConfig;
    config.knownHostsFile = knownHosts;

    auto credentials = data->connectionCredentials;
    credentials.auth = ssh::SSHAuthtype::PASSWORD;
    EXPECT_FALSE(credentials.username.empty()) << "No SSH user name set";

    auto session = ssh::SSHSession::createSession();
    std::tuple<ssh::SSHReturnType, base::any> retVal;
    try {
      retVal = session->connect(config, credentials);
      EXPECT_TRUE(std::get<0>(retVal) == ssh::SSHReturnType::FINGERPRINT_UNKNOWN) << "fingerprint unknown";
      session->disconnect();
    } catch (std::runtime_error &exc) {
      std::ignore = exc;
      session->disconnect();
      throw;
    }

    // have to use tmp val to make clang happy
    std::string tmp = std::get<1>(retVal);

    config.fingerprint = tmp;
    retVal = session->connect(config, credentials);
    EXPECT_TRUE(std::get<0>(retVal) == ssh::SSHReturnType::CONNECTED) << "Connection failed";
    EXPECT_TRUE(session->isConnected()) << "Connection status is wrong";
    session->disconnect();
    EXPECT_FALSE(session->isConnected()) << "Connection is still valid";

    if (base::file_exists(knownHosts))
      base::remove(knownHosts);
  }

  TEST_F(SSHTestingTest, DISABLED_TestsSftpFunctionality) {
    auto config = data->connectionConfig;
    config.strictHostKeyCheck = false;
    auto credentials = data->connectionCredentials;
    credentials.auth = ssh::SSHAuthtype::PASSWORD;

    auto session = ssh::SSHSession::createSession();

    auto retVal = session->connect(config, credentials);
    EXPECT_EQ(std::get<0>(retVal) == ssh::SSHReturnType::CONNECTED, true) << "Connection failed";
    EXPECT_EQ(session->isConnected(), true) << "Connection status is wrong";

    ssh::SSHSftp sftp(session, 65535);
    std::string randomDir = "test_" + testing::randomString();
    try {
      sftp.mkdir(randomDir);
    } catch (ssh::SSHSftpException &) {
      FAIL() << "Unable to create remote directory";
    }

    try {
      auto info = sftp.stat(randomDir);
    } catch (ssh::SSHSftpException &) {
      FAIL() << "Unable to stat remote directory";
    }

    try {
      sftp.mkdir(randomDir);
      FAIL() << "Directory was created but it shouldn't";
    } catch (ssh::SSHSftpException &) {
      // pass
    }

    try {
        sftp.rmdir(randomDir);
    } catch (ssh::SSHSftpException &) {
      FAIL() << "Unable to remove directory";
    }

    std::string sampleText = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Praesent scelerisque quam ac.";
    std::string testFile = "ssh_test_" + testing::randomString();
    try {
      sftp.setContent(testFile, sampleText);
    } catch (ssh::SSHSftpException &) {
      FAIL() << "Unable to create file";
    }

    try {
      std::string content = sftp.getContent(testFile);
      EXPECT_EQ(sampleText, content) << "File content missmatch";
    } catch (ssh::SSHSftpException &) {
      FAIL() << "Unable to get contents of file: " + testFile;
    }

    auto file = base::makeTmpFile("tremporary_download");
    auto tmpFilePath = file.getPath();
    file.dispose();
    try {
      sftp.get(testFile, tmpFilePath);
      std::string fContents = base::getTextFileContent(tmpFilePath);
      EXPECT_EQ(base::same_string(fContents, sampleText, true), true) << "Get file content failed";
    } catch (ssh::SSHSftpException &exc) {
      std::string msg = "Unable to get contents of file: " + testFile + " error:";
      msg.append(exc.what());
      FAIL() << msg;
    }

    sftp.unlink(testFile);
    base::remove(tmpFilePath);
    auto currentDir = sftp.pwd();

    EXPECT_EQ(sftp.cd("/home"), 1) << "Can't use /home directory, check ftp configuration";
    EXPECT_EQ(sftp.pwd(), "/home") << "Invalid current directory information";
    EXPECT_EQ(sftp.cd(".."), 1) << "Can't change to parent dir, check ftp configuration";
    EXPECT_EQ(sftp.cd("home"), 1) << "Can't change to /home, check ftp configuration";
    EXPECT_EQ(sftp.cd(currentDir), 1) << "Unable to switch to initial directory";
    EXPECT_EQ(sftp.cd("/this_is_invalid"), -1) << "Existing directory /this_is_invalid";

    // TODO: This is not portable. Need a better way to check for restricted folders.
    // ensure_true("Dir /etc/ssl/private should be restricted", sftp.cd("/etc/ssl/private") == -2);
    EXPECT_EQ(sftp.pwd(), currentDir) << "Invalid current directory information";
  }

  TEST_F(SSHTestingTest, DISABLED_TestsTunnelConnection) {
    auto config = data->connectionConfig;
    config.remotehost = "127.0.0.1";
    config.remoteport = data->context->getConfigurationIntValue("ssh/dbport", 3306);
    config.strictHostKeyCheck = false;
    auto credentials = data->connectionCredentials;
    credentials.auth = ssh::SSHAuthtype::PASSWORD;

    auto manager = std::unique_ptr<ssh::SSHTunnelManager>(new ssh::SSHTunnelManager());

    // This should start new worker thread.
    manager->start();

    auto session = ssh::SSHSession::createSession();

    auto retVal = session->connect(config, credentials);
    EXPECT_EQ(std::get<0>(retVal) == ssh::SSHReturnType::CONNECTED, true) << "connection established";
    EXPECT_EQ(session->isConnected(), true) << "connection status, is connected";

    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_EQ(manager->isRunning(), true) << "Tunnel Manager isn't running";

    try {
      retVal = manager->createTunnel(session);
    } catch (ssh::SSHTunnelException &exc) {
      FAIL() << std::string("Unable to create tunnel: ") + exc.what();
    }

    uint16_t port = std::get<1>(retVal);

    sql::DriverManager *dm = sql::DriverManager::getDriverManager();
    dm->set_testing();
    EXPECT_NE(nullptr, dm) << "dm is NULL";

    db_mgmt_ConnectionRef connectionProperties = db_mgmt_ConnectionRef(grt::Initialized);
    grt::DictRef conn_params(true);
    conn_params.set("hostName", grt::StringRef(config.localhost));
    conn_params.set("port", grt::IntegerRef(port));
    conn_params.set("userName", grt::StringRef(data->context->getConfigurationStringValue("ssh/dbuser")));
    conn_params.set("password", grt::StringRef(data->context->getConfigurationStringValue("ssh/dbpass")));
    grt::replace_contents(connectionProperties->parameterValues(), conn_params);
    db_mgmt_DriverRef driverProperties(grt::Initialized);
    driverProperties->driverLibraryName(grt::StringRef("mysqlcppconn"));
    connectionProperties->driver(driverProperties);

    try {
      std::vector<sql::ConnectionWrapper> wrapperList;

      for (int i = 0; i < 4; i++) {
        sql::ConnectionWrapper wrapper = dm->getConnection(connectionProperties);
        EXPECT_NE(wrapper.get(), nullptr) << "conn is NULL";
        wrapperList.push_back(wrapper);
      }

      for (auto &iter: wrapperList) {
        sql::Connection *conn = iter.get();
        std::unique_ptr<sql::Statement> stmt(conn->createStatement());
        EXPECT_NE(stmt.get(), nullptr) << "Statement is invalid";

        std::unique_ptr<sql::ResultSet> rset(stmt->executeQuery("SELECT CONNECTION_ID()"));
        EXPECT_NE(rset.get(), nullptr) << "Invalid connection ID result set";

        EXPECT_EQ(rset->next(), true) << "Result set is empty";
      }
    } catch (std::exception &exc) {
      manager->setStop();
      manager->pokeWakeupSocket();
      FAIL() << std::string("Unable to make tunnel connection. ") + exc.what();
    }

    manager->setStop();
    manager->pokeWakeupSocket();
  }

}
