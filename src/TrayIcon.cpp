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

#include "stdafx.h"

#include "TrayIcon.h"

#include "Utils.h"


TrayIcon::TrayIcon():
  _added(false),
  _popup(NULL)
{ }

void
TrayIcon::add(HWND hwnd, HICON icon) 
{
  _trayicon.cbSize = sizeof(NOTIFYICONDATA);

  _trayicon.hWnd             = hwnd;
  _trayicon.uID              = 1;
  _trayicon.uFlags           = NIF_MESSAGE | NIF_ICON;
  _trayicon.uCallbackMessage = WM_TRAY_ICON_MESSAGE;
  _trayicon.hIcon            = icon;

  Shell_NotifyIcon(NIM_ADD, &_trayicon);

  _added = true;
}

void
TrayIcon::remove() 
{
  Shell_NotifyIcon(NIM_DELETE, &_trayicon);
  _added = false;
}

TrayIcon::~TrayIcon() 
{
  if (_added)
    remove();
}

bool
TrayIcon::setIcon(HICON icon) 
{
  _trayicon.uFlags = NIF_ICON;
  _trayicon.hIcon = icon;
  Shell_NotifyIcon(NIM_MODIFY, &_trayicon);
  return true;
}

bool
TrayIcon::setToolTip(const std::wstring& src) 
{
  _trayicon.uFlags = NIF_TIP;
  wcscpy_s(_trayicon.szTip, src.c_str());  
  Shell_NotifyIcon(NIM_MODIFY, &_trayicon);
  return true;
}

bool
TrayIcon::onContextMenu(HWND hwnd) 
{
  POINT pt;
		
  if (_popup == NULL) return false;

  SetForegroundWindow(hwnd);
  GetCursorPos(&pt);
  TrackPopupMenu(_popup, 
		  TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
		  pt.x, pt.y, 0, hwnd, NULL); 

  return true; 
}

bool 
TrayIcon::WinProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
  if (message == WM_TRAY_ICON_MESSAGE && wParam == 1) {
    switch (lParam)
    {
    case WM_RBUTTONDOWN: 
    case WM_CONTEXTMENU: 
      if (onContextMenu(hWnd)) 
        return true;
    }
  }
  return false;
}
