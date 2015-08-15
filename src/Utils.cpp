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

#include <sstream>
#include <memory>
#include <iostream>
#include "Utils.h"
#include "resource.h"
#include "version.h"

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
    strstream << msg << " (Error code: " << code << ")";
    MessageBoxW(NULL, strstream.str().c_str(), TO_WIDESTRING(PACKAGE_NAME), MB_OK | MB_ICONERROR); 
  }
  else
  {
    std::wstringstream strstream;
    strstream << msg << " (Error: " << buffer << ")";
    MessageBoxW(NULL, strstream.str().c_str(), TO_WIDESTRING(PACKAGE_NAME), MB_OK | MB_ICONERROR); 
    LocalFree(buffer);
  }  
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

std::wstring getApplicationExecutablePath() {
  wchar_t result[MAX_PATH+1];
  DWORD size = GetModuleFileNameW(nullptr, result, length(result));
  if (size == length(result))
    return std::wstring(); // Ups: buffer to small
  if (size == 0)
    return std::wstring();

  return std::wstring(result, result + size);
}

bool setProgramToAutostart(bool value) 
{
  std::wstring exe = getApplicationExecutablePath();
  if (exe.empty())
  {
    return false;
  }

  HKEY key;
  LONG error = RegCreateKeyExW (
    HKEY_CURRENT_USER,
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
    0, NULL, REG_OPTION_NON_VOLATILE,
    KEY_ALL_ACCESS, NULL, &key, 0);
  if (error != ERROR_SUCCESS) {
    handleWindowsError(L"cannot open autostart registry key: %s", error);
    return false;
  }

  if (value) {
    error = RegSetValueExW(key, TO_WIDESTRING(PACKAGE_NAME), 0, REG_SZ,
      (LPBYTE)(exe.c_str()), (DWORD)((exe.size() + 1) * sizeof(wchar_t)));
  } else {
    if (RegQueryValueExW(key, TO_WIDESTRING(PACKAGE_NAME),0,0,0,0) != ERROR_FILE_NOT_FOUND)
      error = RegDeleteValueW(key, TO_WIDESTRING(PACKAGE_NAME));
  }

  RegCloseKey(key);
  if (error != ERROR_SUCCESS) {
    handleWindowsError(L"cannot set autostart registry value: %s", error);
    return false;
  }
  return true;
}

bool isProgramInAutostart() 
{
  std::wstring exe = getApplicationExecutablePath();
  if (exe.empty())
  {
    return false;
  }

  HKEY key;
  LONG error;

  error = RegCreateKeyExW(
    HKEY_CURRENT_USER,
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
    0, NULL, REG_OPTION_NON_VOLATILE,
    KEY_ALL_ACCESS, NULL, &key, 0);
  if (error != ERROR_SUCCESS) {
    handleWindowsError(L"cannot open autostart registry key: %s", error);
    return false;
  }

  DWORD buffersize = 0;
#if (WINVER < _WIN32_WINNT_VISTA)
  DWORD type;
  error = RegQueryValueEx(key, TO_WIDESTRING(PACKAGE_NAME), NULL,  &type, NULL, &buffersize);
#else
  error = RegGetValue(key, NULL, TO_WIDESTRING(PACKAGE_NAME), RRF_RT_REG_SZ, NULL, NULL, &buffersize);
#endif

  if (error != ERROR_SUCCESS) {
    if (error != ERROR_FILE_NOT_FOUND) {
      handleWindowsError(L"cannot read autostart registry value: %s", error);
    }
    RegCloseKey(key);
    return false;
  }
#if (WINVER < _WIN32_WINNT_VISTA)
  if (type != REG_SZ) {
    handleWindowsError(L"autostart registry value has wrong type.", error);
    RegCloseKey(key);
    return false;
  }
#endif

  std::unique_ptr<wchar_t[]> buffer = std::unique_ptr<wchar_t[]>(new wchar_t[buffersize+1]);
#if (WINVER < _WIN32_WINNT_VISTA)
  error = RegQueryValueEx(key, TO_WIDESTRING(PACKAGE_NAME), NULL,  &type, (BYTE*)buffer.get(), &buffersize);
#else
  error = RegGetValue(key, NULL, TO_WIDESTRING(PACKAGE_NAME), RRF_RT_REG_SZ, NULL, buffer.get(), &buffersize);
#endif

  if (error != ERROR_SUCCESS) {
    if (error != ERROR_FILE_NOT_FOUND) {
      handleWindowsError(L"cannot read autostart registry value: %s", error);
    }
    RegCloseKey(key);
    return false;
  } else {
    RegCloseKey(key);
    return (exe.compare(buffer.get()) == 0);
  }
}