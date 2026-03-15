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

#pragma once

#include "wb_backend_public_interface.h"

#include <map>
#include <string>
#include "grt.h"
#include "base/file_utilities.h"
#include "grts/structs.studio.h"
#include "base/trackable.h"

#ifndef _MSC_VER
#include <cairo/cairo.h>
#endif

#define MAIN_DOCUMENT_NAME "document.mwb.xml"
#define MAIN_DOCUMENT_AUTOSAVE_NAME "document-autosave.mwb.xml"

namespace bec {
  class GRTManager;
}

namespace wb {
  class MYSQLWBBACKEND_PUBLIC_FUNC ModelFile : public base::trackable {
  public:
    ModelFile(const std::string &tmpdir);
    ~ModelFile();

    auto create() -> void;
    auto open(const std::string &path) -> void;

    static auto read_comment(const std::string &path) -> std::string;

    auto cleanup() -> void;

    bool save_to(const std::string &path, const std::string &comment = "");

    auto has_unsaved_changes() -> bool {
      return _dirty;
    }

    auto retrieve_document() -> studio_DocumentRef;

    auto get_load_warnings() const -> std::list<std::string> {
      return _load_warnings;
    }

    auto store_document(const studio_DocumentRef &doc) -> void;
    auto store_document_autosave(const studio_DocumentRef &doc) -> void;

    std::list<std::string> get_file_list(const std::string &prefixdir = "");
    auto has_file(const std::string &name) -> bool;

    auto get_rel_db_file_path() -> std::string;
    auto get_db_file_dir_path() -> std::string;
    auto get_db_file_path() -> std::string;
    auto add_db_file(const std::string &content_dir) -> void;

    auto add_image_file(const std::string &path) -> std::string;
    auto add_script_file(const std::string &path) -> std::string;
    auto add_note_file(const std::string &path) -> std::string;
    auto delete_file(const std::string &path) -> void;
    auto undelete_file(const std::string &path) -> bool;

    auto set_file_contents(const std::string &path, const std::string &data) -> void;
    auto set_file_contents(const std::string &path, const char *data, size_t size) -> void;
    auto get_file_contents(const std::string &path) -> std::string;

    auto get_path_for(const std::string &file) -> std::string;
    auto get_tempdir_path() -> std::string {
      return _content_dir;
    }

    static auto copy_file(const std::string &path, const std::string &dest) -> void;

    auto copy_file_to(const std::string &file, const std::string &dest) -> void;

    auto in_disk_document_version() const -> std::string {
      return _loaded_version;
    }

    // image management
    auto get_image(const std::string &path) -> cairo_surface_t *;

    boost::signals2::signal<void()> *signal_changed() {
      return &_changed_signal;
    }

    static const std::string lock_filename;

  private:
    base::LockFile *_temp_dir_lock;
    base::RecMutex _mutex;
    std::string _temp_dir;                //< temporary files directory
    std::string _content_dir;             //< path for directory where document contents are stored in disk
    std::list<std::string> _delete_queue; //< files marked for deletion
    std::string _loaded_version;          //< version of the model file as stored in disk

    std::list<std::string> _load_warnings; //< warnings from loaded model

    bool _dirty;

    using TableInsertsSqlScripts = std::map<std::string, std::string>; // table guid -> sql script (inserts)
    TableInsertsSqlScripts
      table_inserts_sql_scripts; // for model upgrade only: move insert sql scripts from xml to sqlite db

    boost::signals2::signal<void()> _changed_signal;

    auto unserialize_document(xmlDocPtr xmldoc, const std::string &path) -> studio_DocumentRef;

  private:
    auto attempt_xml_document_upgrade(xmlDocPtr xmldoc, const std::string &version) -> bool;
    auto attempt_document_upgrade(const studio_DocumentRef &doc, xmlDocPtr xmldoc,
                                                const std::string &version) -> studio_DocumentRef;
    auto cleanup_upgrade_data() -> void;

    auto check_and_fix_data_file_bug() -> void;
    auto check_and_fix_duplicate_uuid_bug(xmlDocPtr xmldoc) -> bool;

    auto check_and_fix_inconsistencies(xmlDocPtr xmldoc, const std::string &version) -> void;

    auto check_and_fix_inconsistencies(const studio_DocumentRef &doc, const std::string &version) -> void;

  public:
    static auto unpack_zip(const std::string &zipfile, const std::string &destdir) -> std::list<std::string>;
    void pack_zip(const std::string &zipfile, const std::string &destdir, const std::string &comment = "");

  private:
    static auto add_attachment_file(const std::string &destdir, const std::string &path) -> std::string;

  private:
    auto create_document_dir(const std::string &dir, const std::string &prefix) -> std::string;
    auto semantic_check(studio_DocumentRef doc) -> bool;
  };
}; // namespace wb
