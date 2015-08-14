/* Properties.cpp 
 *
 *  Copyright (C) 2010, 2015  Richard Liebscher
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

#include "Properties.h"

#include "Coffeine.h"
#include "Utils.h"

const wchar_t * const Properties::Filename = L"config.ini";

Properties::Properties() 
{
  wchar_t exefilepath[MAX_PATH];

  // Build Filename
  // TODO: Error handling
  GetModuleFileNameW(NULL, exefilepath, sizeof(exefilepath));
  _configfilepath = std::wstring(exefilepath);
  size_t found = _configfilepath.rfind('\\');
  _configfilepath.replace(++found, _configfilepath.size() - found, Filename);

  // Load Configuration
  _configfile = IniFile(_configfilepath);
  UpdateStartup(_configfile.getBoolean(L"common", L"startup", StartupDefault));
}

Properties::~Properties()
{
}

void Properties::SetStartup(bool startup) {
  if (startup == _startup)
    return;

  UpdateStartup(startup);

  _configfile.setBoolean(L"common", L"startup", StartupDefault);
}

bool Properties::UpdateStartup(bool startup) {
  HKEY key;
  LONG result = RegCreateKeyExW (
    HKEY_CURRENT_USER,
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
    0, NULL, REG_OPTION_NON_VOLATILE,
    KEY_ALL_ACCESS, NULL, &key, 0);

  if (result != ERROR_SUCCESS) {
    handleWindowsError(L"Kann Autostart Registry Schlüssel nicht öffnen", result);
    return false;
  }

  if (startup) {
    result = RegSetValueExW(key, L"Coffeine", 0,
      REG_SZ, (LPBYTE)(_exefilepath.c_str()), (_exefilepath.size() + 1) * sizeof(wchar_t));
  } else {
    // Prüfe ob Wert existiert:
    if (RegQueryValueExW(key, L"Coffeine",0,0,0,0) != ERROR_FILE_NOT_FOUND)
      result = RegDeleteValueW(key, L"Coffeine");
  }

  RegCloseKey(key);

  if (result != ERROR_SUCCESS) {
    handleWindowsError(L"Kann Autostart Registry Wert nicht verändern", result);
    return false;
  }

  _startup = startup;

  return true;
}

void Properties::SetAutomatic(bool value) {
  if (value == _automatic)
    return;

  _automatic = value;

  _configfile.setBoolean(L"common", L"automatic", value);
}
