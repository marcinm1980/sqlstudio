/*
* Copyright (c) 2015 Oracle and/or its affiliates. All rights reserved.
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

#include "ng_recordset_view.h"
#include "mforms/box.h"
#include "mforms/jsonview.h"
#include "mforms/treeview.h"
#include "mforms/toolbar.h"
#include "mforms/progressbar.h"
#include "mforms/textentry.h"
#include "mforms/label.h"
#include "mforms/menubar.h"
#include "grt/icon_manager.h"
#include "base/string_utilities.h"

using namespace ng;
using namespace mforms;
using namespace bec;

/**
* @brief Add action item to toolbar.
* 
* @param toolbar Actual toolbar pointer.
* @param im Icon manager pointer.
* @param itemIcon Path to icon image.
* @param itemName Item name.
* @param itemToolTip Toolbar item tool tip.
*
* @return Toolbar item pointer.
*/
static mforms::ToolBarItem *addActionItem(std::shared_ptr<mforms::ToolBar> toolbar, bec::IconManager *im, const std::string &itemIcon, const std::string &itemName, const std::string &itemTooltip)
{
  mforms::ToolBarItem *item = mforms::manage(new mforms::ToolBarItem(mforms::ActionItem));
  item->set_name(itemName);
  if (im)
    item->set_icon(im->get_icon_path(itemIcon));
  item->set_tooltip(itemTooltip);
  toolbar->add_item(item);
  return item;
}

/**
* @brief Default constructor.
*
* @param toolbar Actual toolbar pointer
* @param label String containing description
*
* @return Toolbar item pointer.
*/
static mforms::ToolBarItem * addLabelItem(std::shared_ptr<mforms::ToolBar> toolbar, const std::string &label)
{
  mforms::ToolBarItem *item = mforms::manage(new mforms::ToolBarItem(mforms::LabelItem));;
  item->set_text(label);
  toolbar->add_item(item);
  return item;
}

/**
*  @brief Default constructor.
*/
NgRecordSetView::NgRecordSetView()
  : mforms::Box(false),
  _toolbar(std::make_shared<mforms::ToolBar>(ToolBarType::MainToolBar)),
  _recordLoadProgress(std::make_shared<mforms::ProgressBar>()),
  _recCount(0),
  _loadedCount(0),
  _filterActive(false),
  _contentBox(false),
  _footerBox(true)
{
  setupUi();
}

/**
* @brief Destructor.
*/
NgRecordSetView::~NgRecordSetView()
{
}

/**
* @brief Set query string for current result.
*
* @param Query string to set
*/
void NgRecordSetView::setQuery(const std::string &query)
{
  _query = query;
}

/**
* @brief Get query string for current result.
*
* @return Query string for current record set.
*/
const std::string &NgRecordSetView::getQuery() const
{
  return _query;
}

/**
* @brief Init record set view
*
**/
void NgRecordSetView::setupUi()
{
  _contentBox.set_padding(2);
  _contentBox.set_spacing(2);
  _footerBox.set_size(-1, 20);

  add(_toolbar.get(), false, true);
  add(&_contentBox, true, true);
  add_end(&_footerBox, false, true);

  _recordLoadProgress->set_indeterminate(false);
  auto box = mforms::manage(new mforms::Box(true));
  box->set_padding(12);
  box->set_size(100, -1);
  box->add(_recordLoadProgress.get(), true, true);
  _footerBox.add_end(box, false, true);
}

/**
* @brief Add content to footer box.
*
* param view Content to dock.
* @param expand - whether the subview should expand to use leftover space in the box.
* @param fill - whether the subview should be resized to fill all allocated space.
*
**/
void NgRecordSetView::addFooterContent(mforms::View* view, bool expand, bool fill)
{
  _footerBox.add(view, expand, fill);
}

/**
* @brief Clear control content
*
**/
void NgRecordSetView::clear()
{
}

/**
* @brief Clear control content
*
**/
void NgRecordSetView::setSize(size_t size)
{
  _recCount = size;
}

/**
* @brief Add grid or json control to result set view.
*
* @view View to add.
**/
void NgRecordSetView::addContent(mforms::View* view)
{
  _contentBox.add(view, true, true);
}

/**
* @brief Default constructor.
*
*/
NgRecordSetSqlView::NgRecordSetSqlView()
  : _gridView(std::make_shared<mforms::TreeView>(mforms::TreeAltRowColors | mforms::TreeShowRowLines | mforms::TreeShowColumnLines | mforms::TreeNoBorder)),
  _toolbarPagination(std::make_shared<mforms::ToolBar>(ToolBarType::SecondaryToolBar)),
  _pagingLabel(nullptr), _initialRowsPerPage(20), _actualPage(1), _actualRowsPerPage(_initialRowsPerPage)

{
  setupUi();
  setMainToolbar();
  NgRecordSetView::addContent(_gridView.get());
  scoped_connect(&_signalPaginationModified, boost::bind(&NgRecordSetSqlView::paginationModified, this, _1, _2));
}

/**
* @brief Signal emitted when visible page is compleated
*
*/
boost::signals2::signal<void()>* NgRecordSetSqlView::pageCompleted()
{
  return &_signalPageCompleated;
}

/**
* @brief Function called when page count has been changed.
*
* @pageCount Actual page count.
**/
void NgRecordSetSqlView::pageCountChanged(size_t pageCount)
{
  if (pageCount == 2)
    _signalPageCompleated();
  _pageCount = pageCount;
  auto text = base::to_string(_actualPage);
  text += "/";
  text += base::to_string(pageCount);
  _pagingLabel->set_text(text);
}

/**
* @brief Function called when actual page has been changed.
*
* @pageCount Actual page.
**/
void NgRecordSetSqlView::actualPageChanged(size_t actualPage)
{
  _actualPage = actualPage;
  auto text = base::to_string(_actualPage);
  text += "/";
  text += base::to_string(_pageCount);
  _pagingLabel->set_text(text);
}

/**
* @brief Destructor.
*
*/
NgRecordSetSqlView::~NgRecordSetSqlView()
{
}

/**
* @brief Init record set view
*
**/
void NgRecordSetSqlView::setupUi()
{
  _contextMenu = mforms::manage(new mforms::ContextMenu());
  _gridView->set_context_menu(_contextMenu);
  _contextMenu->signal_will_show()->connect(boost::bind(&NgRecordSetSqlView::prepareMenu, this));
  addPaggingControls();
  addFooterContent(_toolbarPagination.get(), true);
  scoped_connect(_pagination.needRepaint(), boost::bind(&NgRecordSetSqlView::repaint, this));
  scoped_connect(_pagination.pagesCountChanged(), boost::bind(&NgRecordSetSqlView::pageCountChanged, this, _1));
  scoped_connect(_pagination.actualPageChanged(), boost::bind(&NgRecordSetSqlView::actualPageChanged, this, _1));
}

/**
* @brief Add pagination to view
*
**/
void NgRecordSetSqlView::addPaggingControls()
{
  auto im = IconManager::get_instance();
  addLabelItem(_toolbarPagination, "Show");

  auto item = mforms::manage(new mforms::ToolBarItem(mforms::TextEntryItem));
  item->set_name("rpp");
  item->set_text(base::to_string(_actualRowsPerPage));
  item->signal_activated()->connect(boost::bind(&NgRecordSetSqlView::navigate, this, item));
  _toolbarPagination->add_item(item);

  addLabelItem(_toolbarPagination, "rows.");
  _toolbarPagination->add_separator_item();

  addLabelItem(_toolbarPagination, "Navigate:");

  item = addActionItem(_toolbarPagination, im, "record_first.png", "first", "Go to the first row in the recordset.");
  item->signal_activated()->connect(boost::bind(&NgRecordSetSqlView::navigate, this, _1));

  item = addActionItem(_toolbarPagination, im, "record_back.png", "back", "Go back one row in the recordset.");
  item->signal_activated()->connect(boost::bind(&NgRecordSetSqlView::navigate, this, _1));

  _pagingLabel = addLabelItem(_toolbarPagination, "");

  item = addActionItem(_toolbarPagination, im, "record_next.png", "next", "Go next one row in the recordset.");
  item->signal_activated()->connect(boost::bind(&NgRecordSetSqlView::navigate, this, _1));

  item = addActionItem(_toolbarPagination, im, "record_last.png", "last", "Go to the last row in the recordset.");
  item->signal_activated()->connect(boost::bind(&NgRecordSetSqlView::navigate, this, _1));
}

/**
* @brief Pagiantion user action.
*
* @item Toolbar item.
**/
void NgRecordSetSqlView::navigate(mforms::ToolBarItem *item)
{
  auto name = item->get_name();
  if (name == "first")
  {
    _signalPaginationModified(NgRecordSetView::ClickedFirst, 0);
  }
  else if (name == "back")
  {
    _signalPaginationModified(NgRecordSetView::ClickedBack, 0);
  }
  else if (name == "next")
  {
    _signalPaginationModified(NgRecordSetView::ClickedNext, 0);
  }
  else if (name == "last")
  {
    _signalPaginationModified(NgRecordSetView::ClickedLast, 0);
  }
  else if (name == "rpp")
  {
    auto value = item->get_text();
    if (base::is_number(value))
    {
      std::stringstream ss(value);
      int rowPerPage = -1;
      ss >> rowPerPage;
      if (rowPerPage > 0)
        _actualRowsPerPage = rowPerPage;
      _signalPaginationModified(NgRecordSetView::PagesEntered, rowPerPage);
    }
  }
}

/**
* @brief Set column header in grid view.
*
* @value Column description reference.
**/
void NgRecordSetSqlView::setCoulmnHeaderAndType(const ColumnDescription &description)
{
  _columnDescription = description;
  for (const auto &value : description)
  {
    auto columnType = mforms::TreeColumnType::StringColumnType;
    switch (value.first)
    {
    case shcore::Value_type::Bool:
      columnType = mforms::TreeColumnType::CheckColumnType;
      break;
    case shcore::Value_type::Float:
      columnType = mforms::TreeColumnType::FloatColumnType;
      break;
    case shcore::Value_type::UInteger:
    case shcore::Value_type::Integer:
      columnType = mforms::TreeColumnType::IntegerColumnType;
      break;
    case shcore::Value_type::Object:
    case shcore::Value_type::Array:
    case shcore::Value_type::Map:
    case shcore::Value_type::MapRef:
    case shcore::Value_type::Function:
    case shcore::Value_type::String:
    default:
      columnType = mforms::TreeColumnType::StringColumnType;
    }
    _gridView->add_column(columnType, value.second, 100, false, true);
  }
  _gridView->end_columns();
}

/**
* @brief Pagiantion user action.
*
* @ev Pagination event type.
* @value Record count per page to set.
**/
void NgRecordSetSqlView::paginationModified(const PaginationEvent &ev, int value)
{
  switch (ev)
  {
  case ClickedNext:
    _pagination += 1;
    break;
  case ClickedLast:
    _pagination.setLast();
    break;
  case ClickedBack:
    _pagination -= 1;
    break;
  case ClickedFirst:
    _pagination.setFirst();
    break;
  case PagesEntered:
    _pagination.setEntryPerPage(value);
    break;
  default:
    break;
  }
}

/**
* @brief Redraw control.
*
**/
void NgRecordSetSqlView::repaint()
{
  _gridView->clear();
  auto end = _pagination.to();
  for (auto it = _pagination.from(); it != end; ++it)
    displayRowInGrid(*it);
}

/**
* @brief Add record to result set control.
*
* @param row Row reference to add.
* @return Record as string.
**/
std::string NgRecordSetSqlView::addRow(const mysh::Row &row)
{
  _recordLoadProgress->set_value((float)_loadedCount / (float)_recCount);
  _pagination.pushBack(row);
  _loadedCount++;
  const auto &current = _pagination.last();
  if (!_pagination.display(current))
    return displayRowInGrid(row, true);
  return displayRowInGrid(row);
}

/**
* @brief Add record to result set control without calling redraw.
*
* @param row Row reference to add.
**/
void NgRecordSetSqlView::addRowSilent(const mysh::Row &row)
{
  _recordLoadProgress->set_value((float)_loadedCount / (float)_recCount);
  _loadedCount++;
  _pagination.pushBackSilent(row);
}

/**
* @brief All records are loaded.
*
**/
void NgRecordSetSqlView::endLoad()
{
  _pagination.endLoad();
}

/**
* @brief Setup context manu.
*
**/
void NgRecordSetSqlView::prepareMenu()
{
  if (_contextMenu)
  {
    _contextMenu->remove_all();
    bool ro = true;
    mforms::MenuItem *item = mforms::manage(new mforms::MenuItem(ro ? "Open Value in Viewer" : "Open Value in Editor"));
    item->set_name("edit_cell");
    _contextMenu->add_item(item);
    _contextMenu->add_separator();
  }
}

/**
* @brief Display records in control.
*
* @row Ng shell row reference.
* @silent Generate only text without adding node to grid view.
* 
* @returns Return row as string.
**/
std::string NgRecordSetSqlView::displayRowInGrid(const mysh::Row &row, bool silent /*= false*/)
{
  TreeNodeRef node;
  std::stringstream buf;
  if (!silent)
    node = _gridView->root_node()->add_child();
  for (int i = 0; i < (int)_columnDescription.size(); ++i)
  {
    auto value = row.get_member(_columnDescription[i].second);
    switch (value.type)
    {
    case shcore::Value_type::Bool:
      buf << value.as_bool();
      if (!silent)
        node->set_bool(i, value.as_bool());
      break;
    case shcore::Value_type::Float:
      buf << value.as_double();
      if (!silent)
        node->set_float(i, value.as_double());
      break;
    case shcore::Value_type::UInteger:
      buf << value.as_uint();
      if (!silent)
        node->set_long(i, value.as_uint());
      break;
    case shcore::Value_type::Integer:
      buf << value.as_int();
      if (!silent)
        node->set_long(i, value.as_int());
      break;
    case shcore::Value_type::Object:
    case shcore::Value_type::Array:
    case shcore::Value_type::Map:
    case shcore::Value_type::MapRef:
    case shcore::Value_type::Function:
    case shcore::Value_type::Undefined:
      buf << value.repr();
      if (!silent)
        node->set_string(i, value.repr());
      break;
    case shcore::Value_type::String:
    default:
      buf << value.as_string();
      if (!silent)
        node->set_string(i, value.as_string());
    }
    buf << "\t";
  }
  return buf.str();
}

/**
* @brief Clear control content.
*
**/
void NgRecordSetSqlView::clear()
{
  _loadedCount = 0;
  _gridView->clear();
  _pagination.clear();
}

/**
* @brief Filter items in JSON tree.
*
* @param item Toolbar item.
**/
void NgRecordSetSqlView::filter(mforms::ToolBarItem *item)
{
  if (item != nullptr)
  {
    auto name = item->get_name();
    if (name == "filter")
    {
      auto textToFind = item->get_text();
      if (textToFind.empty())
        _pagination.disableFiltering();
      else
      {
        _pagination.filterRecords([&, this](const mysh::Row &row) -> bool {
          for (int i = 0; i < (int)_columnDescription.size(); ++i)
          {
            auto value = row.get_member(_columnDescription[i].second);
            if (base::contains_string(value.repr(), textToFind, false))
              return true;
          }
          return false;
        });
      }
    }
    else if (name == "filter_off" && _filterActive)
    {
      _pagination.disableFiltering();
    }
  }
}

/**
* @brief Init toolbar items.
*
**/
void NgRecordSetSqlView::setMainToolbar()
{
  mforms::ToolBarItem *item = nullptr;
  bec::IconManager *im = bec::IconManager::get_instance();
  assert(_toolbar != nullptr);
  _toolbar->remove_all();

  item = mforms::manage(new mforms::ToolBarItem(mforms::TitleItem));;
  item->set_text("Result Grid");
  _toolbar->add_item(item);
  _toolbar->add_separator_item();

  addActionItem(_toolbar, im, "record_sort_reset.png", "record_sort_reset", "Resets all sorted columns");
  //item->signal_activated()->connect(boost::bind(&NgRecordSetSqlView::filter, this, _1));
  item = addActionItem(_toolbar, im, "record_refresh.png", "filter_off", "Refresh data re-executing the original query");
  item->signal_activated()->connect(boost::bind(&NgRecordSetSqlView::filter, this, _1));

  addLabelItem(_toolbar, "Filter Rows:");
  item = mforms::manage(new mforms::ToolBarItem(mforms::SearchFieldItem));
  item->signal_activated()->connect(boost::bind(&NgRecordSetSqlView::filter, this, _1));
  item->set_name("filter");
  _toolbar->add_item(item);
  _toolbar->add_separator_item();

  _toolbar->add_separator_item();
  addLabelItem(_toolbar, "Edit:");
  addActionItem(_toolbar, im, "record_edit.png", "record_edit", "Edit current row");
  addActionItem(_toolbar, im, "record_add.png", "record_add", "Insert new row");
  addActionItem(_toolbar, im, "record_del.png", "record_del", "Delete selected rows");

  addLabelItem(_toolbar, "Export/Import:");
  addActionItem(_toolbar, im, "record_export.png", "record_export", "Export recordset to an external file");
  addActionItem(_toolbar, im, "record_import.png", "record_import", "Import records from an external file");

  _toolbar->add_separator_item();
  addLabelItem(_toolbar, "Apply changes:");
  item = addActionItem(_toolbar, im, "record_save.png", "record_save", "Apply changes to data");
  //item->signal_activated()->connect(boost::bind(&Recordset::apply_changes, this));
  item = addActionItem(_toolbar, im, "record_discard.png", "record_discard", "Discard changes to data");
  //item->signal_activated()->connect(boost::bind(&Recordset::rollback, this));
}

/**
* @brief Default constructor.
*/
NgRecordSetJsonView::NgRecordSetJsonView()
  : _jsonView(std::make_shared<mforms::JsonTabView>()), 
  _recordCountLabel(nullptr),
  _executionTimeLabel(nullptr),
  _fetchTimeLabel(nullptr)
{
  setupUi();
  setMainToolbar();
  NgRecordSetView::addContent(_jsonView.get());
}

/**
* @brief Destructor.
*/
NgRecordSetJsonView::~NgRecordSetJsonView()
{
}

/**
* @brief Init record set view.
*
**/
void NgRecordSetJsonView::setupUi()
{
  auto box = mforms::manage(new mforms::Box(true));
  box->set_spacing(8);

  auto boxLeft = mforms::manage(new mforms::Box(true));
  auto recordCountDescription = mforms::manage(new mforms::Label("Number of records: "));
  _recordCountLabel = mforms::manage(new mforms::Label("200"));

  boxLeft->add(recordCountDescription, true, true);
  boxLeft->add(_recordCountLabel, true, true);
  box->add(boxLeft, false, true);

  auto boxRight = mforms::manage(new mforms::Box(true));
  auto executionTimeDescription = mforms::manage(new mforms::Label("Server Execution Time: "));
  _executionTimeLabel = mforms::manage(new mforms::Label("0.43 sec"));
  auto totalFetchTime = mforms::manage(new mforms::Label("Total Fetch Time: "));
  _fetchTimeLabel = mforms::manage(new mforms::Label("0.1243 sec"));

  boxRight->add(executionTimeDescription, true, true);
  boxRight->add(_executionTimeLabel, true, true);
  boxRight->add(totalFetchTime, true, true);
  boxRight->add(_fetchTimeLabel, true, true);
  box->add_end(boxRight, false, true);

  addFooterContent(box, true, true);
}

/**
* @brief Clear control content
*
**/
void NgRecordSetJsonView::clear()
{
  _loadedCount = 0;
  _jsonView->clear();
}

/**
* @brief Add JSON value to control
*
* @param val JSON data to append
**/
void NgRecordSetJsonView::addRow(const JsonParser::JsonValue &val)
{
  _jsonView->setJson(val);
}

/**
* @brief Add Text value to control
*
* @param text Data to append
**/
void NgRecordSetJsonView::addRow(const std::string &text)
{
  _jsonView->append(text);
}

/**
* @brief Filter items in JSON tree
*
* @param item Pressed toolbar item.
**/
void NgRecordSetJsonView::filter(mforms::ToolBarItem *item)
{
  if (item != nullptr)
  {
    auto name = item->get_name();
    if (name == "filter")
    {
      auto value = item->get_text();
      _filterActive = _jsonView->filterView(value);
    }
    else if (name == "filter_off" && _filterActive)
    {
      _filterActive = false;
      _jsonView->restoreOrginalResult();
    }
  }
}

/**
* @brief Serach item in JSON tree
*
* @param item Toolbar item.
**/
void NgRecordSetJsonView::search(mforms::ToolBarItem *item)
{
  if (item != nullptr)
  {
    auto name = item->get_name();
    if (name == "search")
    {
      auto value = item->get_text();
      _jsonView->highlightMatch(value);
    }
    else if(name == "search_next")
    {
      _jsonView->highlightNextMatch();
    }
    else if (name == "search_back")
    {
      _jsonView->highlightPreviousMatch();
    }
  }
}

/**
* @brief Init toolbar items.
* 
**/
void NgRecordSetJsonView::setMainToolbar()
{
  mforms::ToolBarItem *item = nullptr;
  bec::IconManager *im = bec::IconManager::get_instance();
  assert(_toolbar != nullptr);
  _toolbar->remove_all();

  item = mforms::manage(new mforms::ToolBarItem(mforms::TitleItem));;
  item->set_text("Result JSON");
  _toolbar->add_item(item);
  _toolbar->add_separator_item();

  addActionItem(_toolbar, im, "record_sort_reset.png", "record_sort_reset", "Resets all sorted columns");

  item = addActionItem(_toolbar, im, "record_refresh.png", "filter_off", "Refresh data re-executing the original script");
  scoped_connect(item->signal_activated(), boost::bind(&NgRecordSetJsonView::filter, this, _1));
  //item->signal_activated()->connect(boost::bind(&NgRecordSetJsonView::filter, this, _1));

  addLabelItem(_toolbar, "Filter Rows:");
  item = mforms::manage(new mforms::ToolBarItem(mforms::SearchFieldItem));
  item->signal_activated()->connect(boost::bind(&NgRecordSetJsonView::filter, this, _1));
  item->set_name("filter");
  _toolbar->add_item(item);
  _toolbar->add_separator_item();

  addLabelItem(_toolbar, "Search:");
  item = mforms::manage(new mforms::ToolBarItem(mforms::SearchFieldItem));
  item->set_name("search");
  item->signal_activated()->connect(boost::bind(&NgRecordSetJsonView::search, this, _1));
  _toolbar->add_item(item);
  item = addActionItem(_toolbar, im, "record_back.png", "search_back", "Search backward.");
  item->signal_activated()->connect(boost::bind(&NgRecordSetJsonView::search, this, _1));
  item = addActionItem(_toolbar, im, "record_next.png", "search_next", "Search forward.");
  item->signal_activated()->connect(boost::bind(&NgRecordSetJsonView::search, this, _1));

  _toolbar->add_item(item);
  _toolbar->add_separator_item();

  addLabelItem(_toolbar, "Edit:");
  addActionItem(_toolbar, im, "record_add.png", "doc_add", "Insert new JSON object"); // connect in frontend
  addActionItem(_toolbar, im, "record_del.png", "doc_del", "Delete selected JSON object"); // connect in frontend
  _toolbar->add_separator_item();

  addLabelItem(_toolbar, "Export/Import:");
  addActionItem(_toolbar, im, "record_export.png", "record_export", "Export recordset to an external file");
  addActionItem(_toolbar, im, "record_import.png", "record_import", "Import records from an external file");

  _toolbar->add_separator_item();
  addLabelItem(_toolbar, "Apply changes:");
  item = addActionItem(_toolbar, im, "record_save.png", "record_save", "Apply changes to data");
  //item->signal_activated()->connect(boost::bind(&Recordset::apply_changes, this));
  item = addActionItem(_toolbar, im, "record_discard.png", "record_discard", "Discard changes to data");
  //item->signal_activated()->connect(boost::bind(&Recordset::rollback, this));
}
