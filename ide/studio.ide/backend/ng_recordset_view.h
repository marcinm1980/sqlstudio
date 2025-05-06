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

#pragma once
#include "mforms/box.h"
#include "mforms/jsonview.h"
#include "shellcore/types.h"
#include "modules/base_resultset.h"
#include <mutex>

namespace mforms
{
  class TreeView;
  class JsonTabView;
  class ToolBar;
  class ProgressBar;
  class ToolBarItem;
  class Label;
}

namespace ng
{
  /**
  * @brief Class for manage displaying records in NG result set
  *
  **/
  template <typename T>
  class Paginator
  {
  public:
    typedef typename std::vector<T>::iterator Iterator;
    typedef typename std::vector<T>::const_iterator ConstIterator;

    /**
    * @brief Constructor.
    *
    **/
    Paginator() : _actualPage(1), _allPagesCount(1), _entryPerPage(20), _containerSize(0), _useFiltering(false)
    {
      _pagesCountChanged(_allPagesCount);
      _actualPageChanged(_actualPage);
    }

    /**
    * @brief Constructor.
    *
    * @param 
    **/
    void operator += (int val)
    {
      if (_actualPage == _allPagesCount)
        return;
      if (_allPagesCount >= _actualPage + val)
        _actualPage += val;
      else
        _actualPage = _allPagesCount;
      _actualPageChanged(_actualPage);
      _needRepaint();
    }

    /**
    * @brief Constructor.
    *
    * @param
    **/
    void operator -= (int val)
    {
      if (_actualPage - val >= 1)
        _actualPage -= val;
      else
        _actualPage = 1;
      _actualPageChanged(_actualPage);
      _needRepaint();
    }

    /**
    * @brief Set iterator to first page.
    *
    **/
    void setFirst()
    {
      if (_actualPage == 1)
        return;
      _actualPage = 1;
      _actualPageChanged(_actualPage);
      _needRepaint();
    }

    /**
    * @brief Set iterator to last page.
    *
    **/
    void setLast()
    {
      if (_actualPage == _allPagesCount)
        return;
      _actualPage = _allPagesCount;
      _actualPageChanged(_actualPage);
      _needRepaint();
    }

    /**
    * @brief Get iterator pointing to last element.
    *
    **/
    Iterator last()
    {
      auto end = _container.end();
      if (!_container.empty())
        --end;
      return end;
    }

    /**
    *  @brief Add data to the end of the offline container.
    *  @param element Generic value to be inserted.
    *
    **/
    void pushBackSilent(const T &element)
    {
      std::lock_guard<std::mutex> lock(_mutex);
      _containerOffline.push_back(element);
      auto offlineSize = _containerSize + _containerOffline.size();
      auto count = offlineSize / _entryPerPage;
      auto rest = offlineSize % _entryPerPage;
      if (rest > 0)
        count += 1;
      if (count != _allPagesCount)
      {
        _container.insert(end(), std::make_move_iterator(_containerOffline.begin()), std::make_move_iterator(_containerOffline.end()));
        _containerSize = _container.size();
        _containerOffline.clear();
        _allPagesCount = count;
        _pagesCountChanged(_allPagesCount);
      }
      //sleep(1);
    }

    void endLoad()
    {
      std::lock_guard<std::mutex> lock(_mutex);
      // show dialod wait repaint
      _container.insert(end(), std::make_move_iterator(_containerOffline.begin()), std::make_move_iterator(_containerOffline.end()));
      _containerOffline.clear();
      _containerSize = _container.size();
      auto count = _containerSize / _entryPerPage;
      auto rest = _containerSize % _entryPerPage;
      if (rest > 0)
        count += 1;
      if (count != _allPagesCount)
      {
        _allPagesCount = count;
        _pagesCountChanged(_allPagesCount);
      }
    }

    /**
    *  @brief Add data to the end of the container.
    *  @param element Generic value to be inserted.
    *
    * @return iterator pointing to last element.
    **/
    Iterator pushBack(const T &element)
    {
      std::lock_guard<std::mutex> lock(_mutex);
      _container.push_back(element);
      _containerSize = _container.size();
      auto count = _containerSize / _entryPerPage;
      auto rest = _containerSize % _entryPerPage;
      if (rest > 0)
        count += 1;
      if (count != _allPagesCount)
      {
        _allPagesCount = count;
        _pagesCountChanged(_allPagesCount);
      }
      return last();
    }

    /**
    * @brief Returns a read/write iterator.
    *
    * Returns a read/write iterator that points to the first element in the container. 
    *
    * @return iterator for begining of sequence.
    **/
    Iterator begin()
    {
      return _container.begin();
    }

    /**
    * @brief Returns iterator for end of sequence..
    *
    * Returns a read/write iterator that points to end of the container.
    *
    * @return iterator for end of sequence.
    **/
    Iterator end()
    {
      return _container.end();
    }

    /**
    * @brief Erases all elements in a container.
    *
    */
    void clear()
    {
      _container.clear();
      _containerOffline.clear();
      _actualPage = 1;
      _allPagesCount = 1;
      _containerSize = 0;
      _pagesCountChanged(_allPagesCount);
      _actualPageChanged(_actualPage);
    }

    /**
    * @brief Returns a read/write iterator.
    *
    * Returns a read/write iterator that points to the first element to be show in control.
    *
    * @return iterator for the begining of displaying sequence.
    **/
    Iterator from()
    {
      auto skipRecords = (_actualPage - 1) * _entryPerPage;
      if (skipRecords < _containerSize)
        return _container.begin() + skipRecords;
      else 
        return _container.begin();
    }

    /**
    * @brief Returns a read/write iterator.
    *
    * Returns a read/write iterator that points to one past the last element to be shown in control.
    *
    * @return iterator for the last element of displaying sequence.
    **/
    Iterator to()
    {
      auto skipRecords = _actualPage * _entryPerPage;
      if (skipRecords < _containerSize)
        return _container.begin() + skipRecords;
      else
        return _container.end();
    }

    /**
    * @brief Returns an information if element should be visible in control.
    *
    * @param it Iterator to element to be checked
    *
    * @return true or false
    **/
    bool display(ConstIterator it)
    {
      if (it >= from() && it < to())
        return true;
      return false;
    }

    /**
    * @brief Set record count per page.
    *
    * @param value Record count to display on page.
    *
    **/
    void setEntryPerPage(size_t value)
    {
      if (value != _entryPerPage)
      {
        _needRepaint();
        _entryPerPage = value;
      }
      auto count = _containerSize / _entryPerPage;
      auto rest = _containerSize % _entryPerPage;
      if (rest > 0)
        count += 1;
      if (count != _allPagesCount)
      {
        _allPagesCount = count;
        _pagesCountChanged(_allPagesCount);
        _actualPage = 1;
        _actualPageChanged(1);
        _needRepaint();
      }
    }

    /**
    * @brief Filter result set
    *
    * @param check Compare function
    *
    **/
    void filterRecords(std::function<bool(const T &value)> check)
    {
      std::lock_guard<std::mutex> lock(_mutex);
      _containerOffline.insert(_containerOffline.end(), std::make_move_iterator(_container.begin()), std::make_move_iterator(_container.end()));
      _container.clear();
      for (const auto &item : _containerOffline)
      {
        if (check(item))
          _container.push_back(item);
      }
      _containerSize = _container.size();
      auto count = _containerSize / _entryPerPage;
      auto rest = _containerSize % _entryPerPage;
      if (rest > 0)
        count += 1;
      if (count != _allPagesCount)
      {
        _allPagesCount = count;
        _pagesCountChanged(_allPagesCount);
      }
      _useFiltering = true;
      _needRepaint();
    }

    /**
    * @brief Disable record filter
    *
    **/
    void disableFiltering()
    {
      if (!_useFiltering)
        return;
      std::lock_guard<std::mutex> lock(_mutex);
      _container.clear();
      _container.insert(_container.end(), std::make_move_iterator(_containerOffline.begin()), std::make_move_iterator(_containerOffline.end()));
      _containerOffline.clear();
      _containerSize = _container.size();
      auto count = _containerSize / _entryPerPage;
      auto rest = _containerSize % _entryPerPage;
      if (rest > 0)
        count += 1;
      if (count != _allPagesCount)
      {
        _allPagesCount = count;
        _pagesCountChanged(_allPagesCount);
      }
      _needRepaint();
      _useFiltering = false;
    }

    /**
    * @brief Signal emited if control have to be redraw.
    *
    * @returns boost signal function pointer.
    **/
    boost::signals2::signal<void()>* needRepaint()
    {
      return &_needRepaint;
    }

    /**
    * @brief Signal emitted when page count has been changed.
    *
    * @param pageCount Page count.
    * @returns boost signal function pointer.
    **/
    boost::signals2::signal<void(size_t pageCount)>* pagesCountChanged()
    {
      return &_pagesCountChanged;
    }

    /**
    * @brief Signal emitted when actual page has been changed.
    *
    * @param actualPage Actual page.
    * @returns boost signal function pointer.
    **/
    boost::signals2::signal<void(size_t actualPage)>* actualPageChanged()
    {
      return &_actualPageChanged;
    }
  private:
    std::vector<T> _container;
    std::vector<T> _containerOffline;
    size_t _actualPage;
    size_t _allPagesCount;
    size_t _entryPerPage;
    size_t _containerSize;
    boost::signals2::signal<void()> _needRepaint;
    boost::signals2::signal<void(size_t pageCount)> _pagesCountChanged;
    boost::signals2::signal<void(size_t actualPage)> _actualPageChanged;
    std::mutex _mutex;
    bool _useFiltering;
  };

  //----------------- NgRecordSetView--------------------------------------------------------------

  class NgRecordSetView : public mforms::Box
  {
  public:
    enum PaginationEvent { ClickedNext, ClickedLast, ClickedBack, ClickedFirst, PagesEntered };
    virtual void clear() = 0;
    void setSize(size_t size);

  protected:
    NgRecordSetView();
    virtual ~NgRecordSetView();
    void addContent(mforms::View* view);
    void addFooterContent(mforms::View* view, bool expand = false, bool fill = true);
    void setQuery(const std::string &query);
    const std::string& getQuery() const;

    std::shared_ptr<mforms::ToolBar> _toolbar;
    std::shared_ptr<mforms::ProgressBar> _recordLoadProgress;
    size_t _recCount;
    size_t _loadedCount;
    bool _filterActive;

  private:
    void setupUi();

    mforms::Box _contentBox;
    std::string _query;
    mforms::Box _footerBox;

  };

  //----------------- NgRecordSetSqlView-----------------------------------------------------------

  class NgRecordSetSqlView : public NgRecordSetView
  {
  public:
    typedef std::vector<std::pair<shcore::Value_type, std::string>> ColumnDescription;

    NgRecordSetSqlView();
    ~NgRecordSetSqlView();
    virtual void clear();
    void setCoulmnHeaderAndType(const ColumnDescription &description);
    std::string addRow(const mysh::Row &row);
    void addRowSilent(const mysh::Row &row);
    void endLoad();

    void pageCountChanged(size_t pageCount);
    void actualPageChanged(size_t actualPage);
    boost::signals2::signal<void()>* pageCompleted();


  private:
    void addPaggingControls();
    void setupUi();
    void setMainToolbar();
    void paginationModified(const PaginationEvent &ev, int value);
    std::string displayRowInGrid(const mysh::Row &row, bool silent = false);
    void repaint();
    void navigate(mforms::ToolBarItem *item);
    void prepareMenu();
    void filter(mforms::ToolBarItem *item);

    std::shared_ptr<mforms::TreeView> _gridView;
    ColumnDescription _columnDescription;
    Paginator<mysh::Row> _pagination;
    std::string _textOutput;
    std::shared_ptr<mforms::ToolBar> _toolbarPagination;
    mforms::ToolBarItem *_pagingLabel;
    boost::signals2::signal<void(const PaginationEvent &ev, int value)> _signalPaginationModified;
    const int _initialRowsPerPage;
    std::size_t _actualPage;
    int _actualRowsPerPage;
    std::size_t _pageCount;
    boost::signals2::signal<void()> _signalPageCompleated; 
    mforms::ContextMenu *_contextMenu;
  };

  //----------------- NgRecordSetJsonView ---------------------------------------------------------

  class NgRecordSetJsonView : public NgRecordSetView
  {
  public:
    NgRecordSetJsonView();
    ~NgRecordSetJsonView();
    virtual void clear();
    void addRow(const JsonParser::JsonValue &val);
    void addRow(const std::string &text);

  private:
    void setupUi();
    void setMainToolbar();
    void search(mforms::ToolBarItem *item);
    void filter(mforms::ToolBarItem *item);

    std::shared_ptr<mforms::JsonTabView> _jsonView;
    mforms::Label *_recordCountLabel;
    mforms::Label *_executionTimeLabel;
    mforms::Label *_fetchTimeLabel;
  };
}

