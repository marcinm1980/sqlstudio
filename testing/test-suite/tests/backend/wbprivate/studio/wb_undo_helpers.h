/*
 * Copyright (c) 2019, Oracle and/or its affiliates. All rights reserved.
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

// Methods to be included as part of the test data structure.

//----------------------------------------------------------------------------------------------------------------------

void resetUndoAccounting() {
  lastUndoStackSize = um->get_undo_stack().size();
  lastRedoStackSize = um->get_redo_stack().size();
}

//----------------------------------------------------------------------------------------------------------------------

void checkOnlyOneUndoAdded() {
  ++lastUndoStackSize;
  EXPECT_EQ(um->get_undo_stack().size(), lastUndoStackSize) << "Added 1 undo action";

  // Adding new stuff to the undo stack will clear the redo stack.
  lastRedoStackSize = um->get_redo_stack().size();
}

//----------------------------------------------------------------------------------------------------------------------

void checkUndo() {
  EXPECT_EQ(um->get_undo_stack().size(), lastUndoStackSize) << "Undo stack size";
  EXPECT_EQ(um->get_redo_stack().size(), lastRedoStackSize) << "Redo stack size";

  // Check that the latest undo action has a description.
  EXPECT_NE(um->get_action_description(), "") << "Undo action description is set";

  um->undo();
  --lastUndoStackSize;

  // Redo stack should grow by 1 and undo shrink by 1.
  EXPECT_EQ(um->get_redo_stack().size(), lastRedoStackSize + 1) << "Redo stack size after undo";
  EXPECT_EQ(um->get_undo_stack().size(), lastUndoStackSize) << "Undo stack size after undo";

  lastRedoStackSize = um->get_redo_stack().size();
}

//----------------------------------------------------------------------------------------------------------------------

void checkRedo() {
  // make sure that the undo/redo stack has the expected size
  EXPECT_EQ(um->get_undo_stack().size(), lastUndoStackSize) << "Undo stack size";
  EXPECT_EQ(um->get_redo_stack().size(), lastRedoStackSize) << "Redo stack size";

  um->redo();
  ++lastUndoStackSize;

  EXPECT_EQ(um->get_redo_stack().size(), lastRedoStackSize - 1) << "Redo stack size after redo";
  EXPECT_EQ(um->get_undo_stack().size(), lastUndoStackSize) << "Undo stack size after redo";

  lastRedoStackSize = um->get_redo_stack().size();
}

//----------------------------------------------------------------------------------------------------------------------
