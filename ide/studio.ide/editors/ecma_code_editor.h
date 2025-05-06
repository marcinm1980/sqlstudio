/*
 * Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
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

#pragma once

#include "ng_public_interface.h"
#include "code_editor_base.h"

#include "ecma-parser.h"

/**
 * The central ecma editor class.
 */

namespace ng {

  class NG_PUBLIC_TYPE NgECMAEditor : public NgBaseEditor
  {
    friend class NgBaseEditor;

  public:
    typedef std::shared_ptr<NgECMAEditor> Ref;

    virtual ~NgECMAEditor();

    virtual void showCodeCompletion(bool auto_choose_single);
    //    void objectNamesCache(MySQLObjectNamesCache *cache);

  protected:
    NgECMAEditor(bool noToolbar);

    virtual void setupCodeCompletion();
    virtual std::string getWrittenPart(std::size_t position);
    virtual void splitStatementsIfRequired();
    virtual void doSyntaxCheck();

  private:
    void createEditorConfig();
    bool startCodeProcessing();

    ECMARecognizer _parser;
    //MySQLObjectNamesCache *_objectNamesCache;
  };
  
}
