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

#include "Properties.h"

#include "Utils.h"
#include "version.h"

const wchar_t * const Properties::Filename = TO_WIDESTRING(PACKAGE_NAME) L".ini";

Properties::Properties() 
{
  std::wstring path = getApplicationExecutablePath();
  if (path.empty())
  {
    return;
  }

  // Build Filename
  // TODO: Error handling
  size_t found = path.rfind('\\');
  path.replace(++found, path.size() - found, Filename);

  // Load Configuration
  _configfile = IniFile(path);
  _automatic = _configfile.getBoolean(L"common", L"automatic", AutomaticDefault);
  _startup = isProgramInAutostart();
}

Properties::~Properties()
{
}

void Properties::SetStartup(bool startup) {
  if (startup == _startup)
    return;

  setProgramToAutostart(startup);
}

void Properties::SetAutomatic(bool value) {
  if (value == _automatic)
    return;

  _automatic = value;
  _configfile.setBoolean(L"common", L"automatic", value);
}
