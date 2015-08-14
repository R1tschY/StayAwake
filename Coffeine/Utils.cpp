/*  Utils.cpp 
 *
 *  Copyright (C) 2010,2015  Richard Liebscher
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
*/


#include "stdafx.h"

#include <sstream>
#include "Utils.h"
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

HICON
loadResourceIcon(HINSTANCE hinstance, WORD resourceid, int size) 
{
  return (HICON)::LoadImage(
    hinstance,
    MAKEINTRESOURCE(resourceid),
    IMAGE_ICON,
    size,
    size,
    LR_SHARED
  );
}

void
handleWindowsError(LPCWSTR msg, DWORD code) 
{
  wchar_t* buffer;
  FormatMessageW(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      nullptr,
      code,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (wchar_t*)&buffer,
      0,
      nullptr);

  if (buffer != nullptr) {
    int len = std::char_traits<wchar_t>::length(buffer);
    if (len > 1 && buffer[len - 1] == '\n') {
       buffer[len-1] = 0;
       if (buffer[len - 2] == '\r')
         buffer[len-2] = 0;
    }
  }

  if (!buffer) 
  {
    std::wstringstream strstream;
    strstream << "Error code: " << code;
    MessageBoxW(NULL, strstream.str().c_str(), getResourceString(GetModuleHandle(nullptr), IDS_APP_TITLE).c_str(), MB_OK | MB_ICONERROR); 
  }
  else
  {
    MessageBoxW(NULL, buffer, getResourceString(GetModuleHandle(nullptr), IDS_APP_TITLE).c_str(), MB_OK | MB_ICONERROR); 
    LocalFree(buffer);
  }  
}