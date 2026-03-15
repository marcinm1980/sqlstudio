/*
 * Copyright (c) 2017, 2018 Oracle and/or its affiliates. All rights reserved.
 * Copyright (c) 2026 dev4fun. All rights reserved.
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

#include <grts/structs.db.mgmt.h>

#include "db_mgmt_SSHConnection.h"
#include <grtpp_util.h>

//------------------------------------------------------------------------------------------------

db_mgmt_SSHConnection::ImplData::ImplData() {

}

//------------------------------------------------------------------------------------------------

db_mgmt_SSHConnection::ImplData::~ImplData() {
}

//------------------------------------------------------------------------------------------------

auto db_mgmt_SSHConnection::init() -> void {
//  if (!_data) _data= new db_mgmt_SSHConnection::ImplData();
}

//------------------------------------------------------------------------------------------------

db_mgmt_SSHConnection::~db_mgmt_SSHConnection() {
  delete _data;
}

//------------------------------------------------------------------------------------------------

auto db_mgmt_SSHConnection::set_data(ImplData *data) -> void {
  _data = data;
}

//------------------------------------------------------------------------------------------------

auto db_mgmt_SSHConnection::disconnect() -> void {
  if (_data)
    _data->disconnect();
}

auto db_mgmt_SSHConnection::isConnected() -> grt::IntegerRef {
  if (_data)
    return _data->isConnected();
  return 0;
}

//------------------------------------------------------------------------------------------------

auto db_mgmt_SSHConnection::connect() -> grt::IntegerRef {
  if (_data)
    return _data->connect();
  return -1;
}

//------------------------------------------------------------------------------------------------

auto db_mgmt_SSHConnection::executeCommand(const std::string &text) -> grt::DictRef {
  if (_data)
    return _data->executeCommand(text);
  grt::DictRef dict(true);
  dict.gset("stdout", "");
  dict.gset("stderr", "");
  dict.gset("stderr", -1);
  return dict;
}

//------------------------------------------------------------------------------------------------

auto db_mgmt_SSHConnection::executeSudoCommand(const std::string &text, const std::string &user) -> grt::DictRef {
  if (_data)
    return _data->executeSudoCommand(text, user);
  grt::DictRef dict(true);
  dict.gset("stdout", "");
  dict.gset("stderr", "");
  dict.gset("stderr", -1);
  return dict;
}

//------------------------------------------------------------------------------------------------

auto db_mgmt_SSHConnection::cd(const std::string &directory) -> grt::IntegerRef {
  if (_data)
    return _data->cd(directory);
  return 0;
}

//------------------------------------------------------------------------------------------------

auto db_mgmt_SSHConnection::get(const std::string &src, const std::string &dest) -> void {
  if (_data)
    _data->get(src, dest);
}

//------------------------------------------------------------------------------------------------

auto db_mgmt_SSHConnection::getContent(const std::string &src) -> grt::StringRef {
  if (_data)
    return _data->getContent(src);
  return "";
}

//------------------------------------------------------------------------------------------------

auto db_mgmt_SSHConnection::ls(const std::string &path) -> grt::DictListRef {
  if (_data)
    return _data->ls(path);
  return grt::DictListRef();
}

//------------------------------------------------------------------------------------------------

auto db_mgmt_SSHConnection::mkdir(const std::string &directory) -> void {
  if (_data)
    _data->mkdir(directory);
}

//------------------------------------------------------------------------------------------------

auto db_mgmt_SSHConnection::open(const std::string &path) -> db_mgmt_SSHFileRef {
  if (_data)
    return _data->open(path);
  return db_mgmt_SSHFileRef();
}

//------------------------------------------------------------------------------------------------

auto db_mgmt_SSHConnection::put(const std::string &src, const std::string &dest) -> void {
  if (_data)
    _data->put(src, dest);
}

//------------------------------------------------------------------------------------------------

auto db_mgmt_SSHConnection::pwd() -> grt::StringRef {
  if (_data)
    return _data->pwd();
  return "";
}

//------------------------------------------------------------------------------------------------

auto db_mgmt_SSHConnection::rmdir(const std::string &directory) -> void {
  if (_data)
    _data->rmdir(directory);
}

//------------------------------------------------------------------------------------------------

auto db_mgmt_SSHConnection::setContent(const std::string &path, const std::string &content) -> void {
  if (_data)
    _data->setContent(path, content);
}

//------------------------------------------------------------------------------------------------

auto db_mgmt_SSHConnection::stat(const std::string &path) -> grt::DictRef {
  if (_data)
    return _data->stat(path);
  return grt::DictRef();
}

//------------------------------------------------------------------------------------------------

auto db_mgmt_SSHConnection::fileExists(const std::string &path) -> grt::IntegerRef {
  if (_data)
    return _data->fileExists(path);

  return grt::IntegerRef();
}

//------------------------------------------------------------------------------------------------

auto db_mgmt_SSHConnection::unlink(const std::string &file) -> void {
  if (_data)
    _data->unlink(file);
}

//------------------------------------------------------------------------------------------------

