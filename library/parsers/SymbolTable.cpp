/*
 * Copyright (c) 2016, 2018, Oracle and/or its affiliates. All rights reserved.
 * Copyright (c) 2026 dev4fun. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0,
 * as published by the Free Software Foundation.
 *
 * This program is designed to work with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms, as
 * designated in a particular file or component or in included license
 * documentation. The authors of MySQL hereby grant you an additional
 * permission to link the program and your derivative works with the
 * separately licensed software that they have either included with
 * the program or referenced in the documentation.
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 * the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <mutex>

#include "SymbolTable.h"

using namespace parsers;

//----------------- Symbol ---------------------------------------------------------------------------------------------

Symbol::Symbol(std::string const &aName) : name(aName){};

Symbol::~Symbol() {};

auto Symbol::clear() -> void {
}

auto Symbol::setParent(Symbol *parent) -> void {
  this->parent = parent;
}

auto Symbol::getRoot() const -> Symbol * {
  Symbol *run = parent;
  while (run != nullptr) {
    if (run->parent == nullptr || dynamic_cast<SymbolTable *>(run->parent) != nullptr)
      return run;
    run = run->parent;
  }
  return run;
}

auto Symbol::getSymbolPath() const -> std::vector<Symbol const *> {
  std::vector<Symbol const *> result;
  const Symbol *run = this;
  while (run != nullptr) {
    result.push_back(run);
    if (run->parent == nullptr || dynamic_cast<SymbolTable *>(run->parent) != nullptr)
      break;
    run = run->parent;
  }
  return result;
}

auto Symbol::qualifiedName(std::string const &separator, bool full) const -> std::string {
  std::string result = name;
  Symbol *run = parent;
  while (run != nullptr) {
    result = run->name + separator + result;
    if (!full || run->parent == nullptr || dynamic_cast<SymbolTable *>(run->parent) != nullptr)
      break;
    run = run->parent;
  }
  return result;
}

//----------------- TypedSymbol ----------------------------------------------------------------------------------------

TypedSymbol::TypedSymbol(std::string const &name, Type const *aType) : Symbol(name), type(aType) {
};

//----------------- ScopedSymbol ---------------------------------------------------------------------------------------

ScopedSymbol::ScopedSymbol(std::string const &name) : Symbol(name) {
};

auto ScopedSymbol::clear() -> void {
  children.clear();
}

auto ScopedSymbol::addAndManageSymbol(Symbol *symbol) -> void {
  children.emplace_back(symbol);
  symbol->setParent(this);
}

auto ScopedSymbol::resolve(std::string const &name, bool localOnly) -> Symbol * {
  for (auto &child : children) {
    if (child->name == name)
      return child.get();
  }

  // Nothing found locally. Let the parent continue.
  if (!localOnly) {
    ScopedSymbol *scopedParent = dynamic_cast<ScopedSymbol *>(parent);
    if (scopedParent != nullptr)
      return scopedParent->resolve(name, true);
  }

  return nullptr;
}

auto ScopedSymbol::getTypedSymbols(bool localOnly) const -> std::vector<TypedSymbol *> {
  std::vector<TypedSymbol *> result = getSymbolsOfType<TypedSymbol>();

  if (!localOnly) {
    ScopedSymbol *scopedParent = dynamic_cast<ScopedSymbol *>(parent);
    if (scopedParent != nullptr) {
      auto localList = scopedParent->getTypedSymbols(true);
      result.insert(result.end(), localList.begin(), localList.end());
    }
  }

  return result;
}

auto ScopedSymbol::getTypedSymbolNames(bool localOnly) const -> std::vector<std::string> {
  std::vector<std::string> result;

  for (auto &child : children) {
    TypedSymbol *typedChild = dynamic_cast<TypedSymbol *>(child.get());
    if (typedChild != nullptr)
      result.push_back(typedChild->name);
  }

  if (!localOnly) {
    ScopedSymbol *scopedParent = dynamic_cast<ScopedSymbol *>(parent);
    if (scopedParent != nullptr) {
      auto localList = scopedParent->getTypedSymbolNames(true);
      result.insert(result.end(), localList.begin(), localList.end());
    }
  }
  
  return result;
}

auto ScopedSymbol::getTypes(bool localOnly) const -> std::vector<Type *> {
  std::vector<Type *> result = getSymbolsOfType<Type>();

  if (!localOnly) {
    ScopedSymbol *scopedParent = dynamic_cast<ScopedSymbol *>(parent);
    if (scopedParent != nullptr) {
      auto localList = scopedParent->getTypes(true);
      result.insert(result.end(), localList.begin(), localList.end());
    }
  }
  
  return result;
}

auto ScopedSymbol::getDirectScopes() const -> std::vector<ScopedSymbol *> {
  return getSymbolsOfType<ScopedSymbol>();
}

auto ScopedSymbol::getAllSymbols() const -> std::vector<Symbol *> {
  std::vector<Symbol *> result;

  for (auto &child : children) {
    result.push_back(child.get());
  }

  ScopedSymbol *scopedParent = dynamic_cast<ScopedSymbol *>(parent);
  if (scopedParent != nullptr) {
    auto localList = scopedParent->getAllSymbols();
    result.insert(result.end(), localList.begin(), localList.end());
  }

  return result;
}

auto ScopedSymbol::getAllSymbolNames() const -> std::set<std::string> {
  std::set<std::string> result;

  for (auto &child : children) {
    result.insert(child->name);
  }

  ScopedSymbol *scopedParent = dynamic_cast<ScopedSymbol *>(parent);
  if (scopedParent != nullptr) {
    auto localList = scopedParent->getAllSymbolNames();
    result.insert(localList.begin(), localList.end());
  }

  return result;
}

//----------------- VariableSymbol -------------------------------------------------------------------------------------

VariableSymbol::VariableSymbol(std::string const &name, Type const *type) : TypedSymbol(name, type) {
}

//----------------- ParameterSymbol ------------------------------------------------------------------------------------

//----------------- RoutineSymbol --------------------------------------------------------------------------------------

RoutineSymbol::RoutineSymbol(std::string const &name, Type const *aReturnType)
  : ScopedSymbol(name), returnType(aReturnType) {
};

auto RoutineSymbol::getVariables(bool localOnly) const -> std::vector<VariableSymbol *> {
  return getSymbolsOfType<VariableSymbol>();
}

auto RoutineSymbol::getParameters(bool localOnly) const -> std::vector<ParameterSymbol *> {
  return getSymbolsOfType<ParameterSymbol>();
}

//----------------- MethodSymbol ---------------------------------------------------------------------------------------

MethodSymbol::MethodSymbol(std::string const &name, Type const *returnType) : RoutineSymbol(name, returnType) {
}

//----------------- FieldSymbol ----------------------------------------------------------------------------------------

FieldSymbol::FieldSymbol(std::string const &name, Type const *type) : VariableSymbol(name, type) {
}

//----------------- Type -----------------------------------------------------------------------------------------------

Type::Type(std::string const &name, Type const *base) : name(name), baseType(base) {
}

//----------------- FundamentalType ------------------------------------------------------------------------------------

const Type *FundamentalType::INTEGER_TYPE = new FundamentalType("int", TypeKind::Integer);
const Type *FundamentalType::FLOAT_TYPE = new FundamentalType("float", TypeKind::Float);
const Type *FundamentalType::STRING_TYPE = new FundamentalType("string", TypeKind::String);
const Type *FundamentalType::BOOL_TYPE = new FundamentalType("bool", TypeKind::Bool);
const Type *FundamentalType::DATE_TYPE = new FundamentalType("date", TypeKind::Date);

FundamentalType::FundamentalType(std::string const &name, TypeKind kind) : Type(name), kind(kind) {
}

//----------------- ClassSymbol ----------------------------------------------------------------------------------------

ClassSymbol::ClassSymbol(std::string const &name, ClassSymbol *aSuperClass)
  : ScopedSymbol(name), Type(name), superClasses({aSuperClass}) {
}

auto ClassSymbol::getMethods(bool includeInherited) const -> std::vector<MethodSymbol *> {
  return getSymbolsOfType<MethodSymbol>();
}

auto ClassSymbol::getFields(bool includeInherited) const -> std::vector<FieldSymbol *> {
  return getSymbolsOfType<FieldSymbol>();
}

//----------------- ArrayType ------------------------------------------------------------------------------------------

ArrayType::ArrayType(std::string const &name, Type const *elemType, size_t aSize)
  : Type(name), elementType(elemType), size(aSize) {
}

//----------------- TypeAlias ------------------------------------------------------------------------------------------

TypeAlias::TypeAlias(std::string const &name, Type const *target) : Type(name, target) {
}

//----------------- SymbolTable ----------------------------------------------------------------------------------------

class SymbolTable::Private {
public:
  std::recursive_mutex mutex; // Must be in private class for use in C++/CLI code.
};

//----------------------------------------------------------------------------------------------------------------------

SymbolTable::SymbolTable() : ScopedSymbol() {
  _d = new Private();
}

//----------------------------------------------------------------------------------------------------------------------

SymbolTable::~SymbolTable() {
  delete _d;
}

//----------------------------------------------------------------------------------------------------------------------

auto SymbolTable::lock() -> void {
  _d->mutex.lock();
}

//----------------------------------------------------------------------------------------------------------------------

auto SymbolTable::unlock() -> void {
  _d->mutex.unlock();
}

//----------------------------------------------------------------------------------------------------------------------

auto SymbolTable::addDependencies(std::vector<SymbolTable *> const &newDependencies) -> void {
  // No duplicate check takes place.
  _dependencies.insert(_dependencies.end(), newDependencies.begin(), newDependencies.end());
}

//----------------------------------------------------------------------------------------------------------------------

auto SymbolTable::resolve(std::string const &name, bool localOnly) -> Symbol * {
  lock();
  Symbol *result = ScopedSymbol::resolve(name, localOnly);

  if (result == nullptr && !localOnly) {
    for (auto dependency : _dependencies) {
      result = dependency->resolve(name, false);
      if (result != nullptr)
        break;
    }
  }

  unlock();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
