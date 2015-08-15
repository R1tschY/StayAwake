/* 
 * This file is part of StayAwake.
 *
 *  Copyright (C) 2010, 2015  Richard Liebscher <r1tschy@yahoo.de>
 *
 *  StayAwake is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  StayAwake is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "stdafx.h"
#include <memory>

namespace Windows {

namespace MenuEntryFlags {
enum _ {
  Checked = MF_CHECKED,
  Disabled = MF_DISABLED,
  Enabled = MF_ENABLED,
  Grayed = MF_GRAYED,
  Unchecked = MF_UNCHECKED
};
}

class Menu {
  Menu(const Menu&);
  Menu& operator=(const Menu& other);
public:
  Menu() :
    hmenu_()
  { }

  Menu(HMENU handle) :
    hmenu_(handle)
  { }

  Menu(Menu&& other) :
    hmenu_(std::move(other.hmenu_))
  { }
  Menu& operator=(Menu&& other)
  {
    hmenu_ = std::move(other.hmenu_);
    return *this;
  }

  ~Menu()
  { }

  static bool isMenu(HMENU handle) { return IsMenu(handle); }
  static Menu getMainMenu(HWND hwnd) { return Menu(GetMenu(hwnd)); }
  static Menu createMenu() { return Menu(CreateMenu()); }
  static Menu createPopupMenu() { return Menu(CreatePopupMenu()); }
  
  void addEntry(unsigned command, const std::wstring& caption, MenuEntryFlags::_ flags = MenuEntryFlags::Enabled)
  {
    ::AppendMenuW(hmenu_.get(), MF_STRING | flags, command, caption.c_str());
  }

  void insertEntry(unsigned position, unsigned command, const std::wstring& caption, MenuEntryFlags::_ flags = MenuEntryFlags::Enabled)
  {
    ::InsertMenuW(hmenu_.get(), position, MF_BYPOSITION | MF_STRING | flags, command, caption.c_str());
  }

  void insertEntryBefore(unsigned entry_command, unsigned command, const std::wstring& caption, MenuEntryFlags::_ flags = MenuEntryFlags::Enabled)
  {
    ::InsertMenuW(hmenu_.get(), entry_command, MF_BYCOMMAND | MF_STRING | flags, command, caption.c_str());
  }

  void addSeperator()
  {
    ::AppendMenuW(hmenu_.get(), MF_SEPARATOR, 0, nullptr);
  }

  void insertSeperator(unsigned position)
  {
    ::InsertMenuW(hmenu_.get(), position, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr);
  }

  void insertSeperatorBefore(unsigned entry_command)
  {
    ::InsertMenuW(hmenu_.get(), entry_command, MF_BYCOMMAND | MF_SEPARATOR, 0, nullptr);
  }

  bool isEntryChecked(unsigned command) const
  {
    return GetMenuState(hmenu_.get(), command, MF_BYCOMMAND) & MF_CHECKED;      
  }

  void modifyEntry(unsigned command, const std::wstring& caption, MenuEntryFlags::_ flags = MenuEntryFlags::Enabled)
  {
    ::ModifyMenuW(hmenu_.get(), command, MF_BYCOMMAND | MF_STRING | flags, command, caption.c_str());
  }

  void modifyEntryAt(unsigned position, unsigned command, const std::wstring& caption, MenuEntryFlags::_ flags = MenuEntryFlags::Enabled)
  {
    ::ModifyMenuW(hmenu_.get(), position, MF_BYPOSITION | MF_STRING | flags, command, caption.c_str());
  }

  HMENU getHMENU() const { return hmenu_.get(); }

private:
  struct HMenuDeleter {
    typedef HMENU pointer;
    void operator()(HMENU ptr) { DestroyMenu(ptr); }
  };
  typedef std::unique_ptr<HMENU, HMenuDeleter> HMenu;

  HMenu hmenu_;
};

} // namespace Windows

