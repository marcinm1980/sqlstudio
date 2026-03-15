/*
 * Copyright (c) 2015, 2019, Oracle and/or its affiliates. All rights reserved.
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

#include "rapidjson/document.h"
#include "mforms/form.h"
#include "mforms/panel.h"
#include "mforms/treeview.h"

#include "Scintilla.h"

#include <set>
#include <functional>


/**
 * @brief A Json view tab control with tree diffrent view text, tree and grid.
 *
 */
namespace mforms {
  /**
   * @brief Json view base class definition.
   */
  class JsonBaseView : public Panel {
  public:
    JsonBaseView(rapidjson::Document &doc);
    virtual ~JsonBaseView();
    auto highlightMatch(const std::string &text) -> void;
    boost::signals2::signal<void(bool)> *dataChanged();

  protected:
    virtual auto clear() -> void = 0;
    boost::signals2::signal<void(bool)> _dataChanged;
    auto isDateTime(const std::string &text) -> bool;
    rapidjson::Document &_document;
  };

  /**
   * @brief Dialog for adding JSON.
   */
  class CodeEditor;
  class TextEntry;
  class JsonInputDlg : public mforms::Form {
  public:
    JsonInputDlg(mforms::Form *owner, bool showTextEntry);
    virtual ~JsonInputDlg();
    auto text() const -> const std::string &;
    auto data() const -> const rapidjson::Value &;
    auto objectName() const -> std::string;
    auto setText(const std::string &text, bool readonly) -> void;
    auto setJson(const rapidjson::Value &json) -> void;
    auto run() -> bool;

  private:
    rapidjson::Value _value;
    rapidjson::Document _document;
    std::string _text;
    CodeEditor *_textEditor;
    Button *_save;
    Button *_cancel;
    TextEntry *_textEntry;
    bool _validated;

    auto setup(bool showTextEntry) -> void;
    auto validate() -> void;
    auto save() -> void;
    auto editorContentChanged(Sci_Position position, Sci_Position length, Sci_Position numberOfLines, bool inserted) -> void;
  };

  /**
   * @brief Json text view control class definition.
   */
  class Label;
  class JsonTextView : public JsonBaseView {
  public:
    JsonTextView(rapidjson::Document &doc);
    virtual ~JsonTextView();
    auto setText(const std::string &jsonText, bool validateJson = true) -> void;
    virtual auto clear() -> void;
    auto findAndHighlightText(const std::string &text, bool backward = false) -> void;
    auto getJson() const -> const rapidjson::Value &;
    auto getText() const -> const std::string &;
    auto validate() -> bool;
    std::function<void()> _stopTextProcessing;
    std::function<void(std::function<bool()>)> _startTextProcessing;

  private:
    struct JsonErrorEntry {
      std::string text;
      std::size_t pos;
      std::size_t length;
    };
    auto init() -> void;
    auto editorContentChanged(Sci_Position position, Sci_Position length, Sci_Position numberOfLines, bool inserted) -> void;
    auto dwellEvent(bool started, size_t position, int x, int y) -> void;

    CodeEditor *_textEditor;
    bool _modified;
    std::string _text;
    Sci_Position _position;
    rapidjson::Value _json;
    std::vector<JsonErrorEntry> _errorEntry;
  };

  class JsonTreeBaseView : public JsonBaseView {
  public:
    typedef std::list<TreeNodeRef> TreeNodeList;
    typedef std::vector<TreeNodeRef> TreeNodeVactor;
    typedef std::map<std::string, TreeNodeVactor> TreeNodeVectorMap;
    struct JsonValueNodeData : public mforms::TreeNodeData {
      JsonValueNodeData(rapidjson::Value &value) : _jsonValue(value), type(value.GetType()) {
      }
      auto getData() -> rapidjson::Value& {
        return _jsonValue;
      }
      ~JsonValueNodeData() {
      }

    private:
      rapidjson::Value &_jsonValue;
      int type;
    };
    JsonTreeBaseView(rapidjson::Document &doc);
    virtual ~JsonTreeBaseView();
    enum JsonNodeIcons { JsonObjectIcon, JsonArrayIcon, JsonStringIcon, JsonNumericIcon, JsonNullIcon };
    auto setCellValue(mforms::TreeNodeRef node, int column, const std::string &value) -> void;
    auto highlightMatchNode(const std::string &text, bool bacward = false) -> void;
    auto filterView(const std::string &text, rapidjson::Value &value) -> bool;
    auto reCreateTree(rapidjson::Value &value) -> void;

  protected:
    auto generateTree(rapidjson::Value &value, int columnId, mforms::TreeNodeRef node, bool addNew = true) -> void;
    virtual auto generateArrayInTree(rapidjson::Value &value, int columnId, TreeNodeRef node) -> void = 0;
    virtual auto generateObjectInTree(rapidjson::Value &value, int columnId, TreeNodeRef node, bool addNew) -> void = 0;
    virtual auto generateNumberInTree(rapidjson::Value &value, int columnId, TreeNodeRef node) -> void = 0;
    virtual auto generateBoolInTree(rapidjson::Value &value, int columnId, TreeNodeRef node) -> void = 0;
    virtual auto generateNullInTree(rapidjson::Value &value, int columnId, TreeNodeRef node) -> void = 0;
    virtual auto setStringData(int columnId, TreeNodeRef node, const std::string &text) -> void = 0;

    auto generateStringInTree(rapidjson::Value &value, int idx, TreeNodeRef node) -> void;
    auto collectParents(TreeNodeRef node, TreeNodeList &parents) -> void;
    static auto getNodeIconPath(JsonNodeIcons icon) -> std::string;

    TreeNodeVectorMap _viewFindResult;
    std::set<rapidjson::Value *> _filterGuard;
    bool _useFilter;
    std::string _textToFind;
    size_t _searchIdx;

    TreeView *_treeView;
    ContextMenu *_contextMenu;

  private:
    auto prepareMenu() -> void;
    virtual auto handleMenuCommand(const std::string &command) -> void;
    auto openInputJsonWindow(TreeNodeRef node, bool updateMode = false) -> void;
  };

  /**
   * @brief Json grid view control class definition.
   */
  class JsonTreeView : public JsonTreeBaseView {
  public:
    JsonTreeView(rapidjson::Document &doc);
    virtual ~JsonTreeView();
    auto setJson(rapidjson::Value &val) -> void;
    auto appendJson(rapidjson::Value &val) -> void;
    virtual auto clear() -> void;

  private:
    auto init() -> void;
    virtual auto generateArrayInTree(rapidjson::Value &value, int columnId, TreeNodeRef node) -> void;
    virtual auto generateObjectInTree(rapidjson::Value &value, int columnId, TreeNodeRef node, bool addNew) -> void;
    virtual auto generateNumberInTree(rapidjson::Value &value, int columnId, TreeNodeRef node) -> void;
    virtual auto generateBoolInTree(rapidjson::Value &value, int columnId, TreeNodeRef node) -> void;
    virtual auto generateNullInTree(rapidjson::Value &value, int columnId, TreeNodeRef node) -> void;
    virtual auto setStringData(int columnId, TreeNodeRef node, const std::string &text) -> void;
  };

  /**
   * @brief Json grid view control class definition.
   */
  class JsonGridView : public JsonTreeBaseView {
  public:
    JsonGridView(rapidjson::Document &doc);
    virtual ~JsonGridView();
    auto setJson(rapidjson::Value &val) -> void;
    auto appendJson(rapidjson::Value &val) -> void;
    virtual auto clear() -> void;
    auto reCreateTree(rapidjson::Value &value) -> void;

  private:
    auto init() -> void;
    auto generateColumnNames(rapidjson::Value &value) -> void;
    auto addColumn(int size, rapidjson::Type type, rapidjson::Value *value, const std::string &name) -> void;
    auto nodeActivated(TreeNodeRef row, int column) -> void;
    auto setCellValue(mforms::TreeNodeRef node, int column, const std::string &value) -> void;
    auto goUp() -> void;

    virtual auto generateArrayInTree(rapidjson::Value &value, int columnId, TreeNodeRef node) -> void;
    virtual auto generateObjectInTree(rapidjson::Value &value, int columnId, TreeNodeRef node, bool addNew) -> void;
    virtual auto generateNumberInTree(rapidjson::Value &value, int columnId, TreeNodeRef node) -> void;
    virtual auto generateBoolInTree(rapidjson::Value &value, int columnId, TreeNodeRef node) -> void;
    virtual auto generateNullInTree(rapidjson::Value &value, int columnId, TreeNodeRef node) -> void;
    virtual auto setStringData(int columnId, TreeNodeRef node, const std::string &text) -> void;

    virtual auto handleMenuCommand(const std::string &command) -> void;
    auto openInputJsonWindow(rapidjson::Value &value) -> void;

    int _level;
    bool _headerAdded;
    int _noNameColId;
    int _columnIndex;
    int _rowNum;
    std::vector<rapidjson::Value *> _actualParent;
    std::map<std::string, int> _colNameToColId;
    Button *_goUpButton;
    Box *_content;
  };

  /**
   * @brief Json tab view control class definition.
   */
  class TabView;
  class MFORMS_EXPORT JsonTabView : public Panel {
  public:
    enum JsonTabViewType { TabText, TabTree, TabGrid };
    auto Setup() -> void;
    JsonTabView(bool tabLess = false, JsonTabViewType defaultView = TabText);
    ~JsonTabView();

    auto setJson(const rapidjson::Value &val) -> void;
    auto setText(const std::string &text, bool validate = true) -> void;
    auto append2(const std::string &text) -> void;
    auto tabChanged() -> void;
    auto dataChanged(bool forceUpdate) -> void;
    auto clear() -> void;
    auto highlightMatch(const std::string &text) -> void;
    auto highlightNextMatch() -> void;
    auto highlightPreviousMatch() -> void;
    auto filterView(const std::string &text) -> bool;
    auto restoreOrginalResult() -> void;
    auto switchTab(JsonTabViewType tab) const -> void;
    auto getActiveTab() const -> JsonTabViewType;
    boost::signals2::signal<void(const std::string &text)> *editorDataChanged();
    auto text() const -> const std::string &;
    auto json() const -> const rapidjson::Value &;

    void setTextProcessingStartHandler(std::function<void(std::function<bool()>)>);
    void setTextProcessingStopHandler(std::function<void()>);

  private:
    JsonTextView *_textView;
    JsonTreeView *_treeView;
    JsonGridView *_gridView;
    TabView *_tabView;
    std::string _jsonText;
    rapidjson::Value _json;
    rapidjson::Document _document;
    int _ident;
    struct {
      int textTabId;
      int treeViewTabId;
      int gridViewTabId;
    } _tabId;
    struct {
      bool textViewUpdate;
      bool treeViewUpdate;
      bool gridViewUpdate;
    } _updateView;
    bool _updating;
    std::string _matchText;
    boost::signals2::signal<void(const std::string &text)> _dataChanged;
    JsonTabViewType _defaultView;
  };
}; // namespace mforms
