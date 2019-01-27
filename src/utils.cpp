/* 
 * This file is part of StayAwake.
 *
 *  Copyright (C) 2015  Richard Liebscher <r1tschy@yahoo.de>
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

#include "utils.h"

#include "stdafx.h"

#include <sstream>
#include <memory>
#include <iostream>

#include "config.h"
#include "resource.h"

std::wstring
getResourceString(HINSTANCE hinstance, unsigned id)
{
  const wchar_t* p = nullptr;
  int len = ::LoadStringW(hinstance, id, reinterpret_cast<LPWSTR>(&p), 0);
  if (len <= 0)
  {
    // TODO: log fail
    return std::wstring();
  }

  return std::wstring(p, static_cast<std::size_t>(len));
}

HICON loadResourceIcon(HINSTANCE hinstance, WORD resourceid, int size)
{
  return (HICON)::LoadImage(
    hinstance,
    MAKEINTRESOURCE(resourceid),
    IMAGE_ICON,
    size,
    size,
    0
  );
}

bool isScreensaverActive()
{
  BOOL result;
  SystemParametersInfoW(SPI_GETSCREENSAVERRUNNING, 0, &result, 0);
  return result == TRUE;
}

HWND getFullscreenWindow()
{
  RECT desktop_rect;
  HWND desktop_window = GetDesktopWindow();
  RECT app;
  HWND hwnd = GetForegroundWindow();

  if (   desktop_window != nullptr
      && hwnd != nullptr
      && hwnd != desktop_window
      && hwnd != GetShellWindow()
      && GetWindowRect(desktop_window, &desktop_rect)
      && GetWindowRect(hwnd, &app)
      && app.top    <= desktop_rect.top
      && app.bottom >= desktop_rect.bottom
      && app.right  >= desktop_rect.right
      && app.left   <= desktop_rect.left // TODO: Rect.isWarping()
      && !isScreensaverActive())
  {
    return hwnd;
  } 
  else 
  {
    return nullptr;
  }
}
