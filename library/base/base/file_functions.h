/*
 * Copyright (c) 2007, 2019, Oracle and/or its affiliates. All rights reserved.
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

#include "common.h"

#include <stdlib.h>
#include <stdio.h>

#include <glib.h>
#include <iosfwd>
#include <fstream>

#ifndef _MSC_VER
#include <sys/stat.h>
#endif

// TODO: These function should probably be merged with file_utilities.
BASELIBRARY_PUBLIC_FUNC auto base_fopen(const char *filename, const char *mode) -> FILE *;
BASELIBRARY_PUBLIC_FUNC auto base_open(const std::string &filename, int open_flag, int permissions) -> int;
BASELIBRARY_PUBLIC_FUNC auto base_remove(const std::string &filename) -> int;
BASELIBRARY_PUBLIC_FUNC auto base_rename(const char *oldname, const char *newname) -> int;
#ifdef _MSC_VER
BASELIBRARY_PUBLIC_FUNC auto base_stat(const char *filename, struct _stat *stbuf) -> int;
#else
BASELIBRARY_PUBLIC_FUNC auto base_stat(const char *filename, struct stat *stbuf) -> int;
#endif

BASELIBRARY_PUBLIC_FUNC auto base_rmdir_recursively(const char *dirname) -> int;
BASELIBRARY_PUBLIC_FUNC auto base_get_file_size(const char *filename) -> long;
