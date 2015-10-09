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

#include "Shellapi.h"

#define WM_TRAY_ICON_MESSAGE (WM_USER + 1)

class TrayIcon {
public:
  TrayIcon();
  ~TrayIcon();

  void add(HWND hwnd, HICON icon);
  void remove();

  bool setIcon(HICON icon);
  bool setToolTip(const std::wstring& src);

  /** Set the popup menu of the tray icon. Set popup to NULL to remove the popup.
  *  You have to call WinProcHandler in your WinProc to make the Popup working.
  */
  void setPopupMenu(HMENU popup) { _popup = popup; }

  bool WinProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
  static const unsigned int id = 0;

  NOTIFYICONDATA _trayicon;
  bool _added;
  HMENU _popup;

  bool onContextMenu(HWND hwnd);
};

