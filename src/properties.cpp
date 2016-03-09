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

#include "properties.h"

#include "config.h.in"
#include "stdafx.h"

#include "utils.h"
#include <cpp-utils/preprocessor.h>
#include <lightports/base/application.h>
#include <lightports/extra/autostart.h>

const wchar_t * const Properties::Filename = CPP_WSTRINGIFY(PACKAGE_NAME) L".ini";

Properties::Properties() 
{
  std::wstring path = Windows::Application::getExecutablePath().getFolder();
  path.append(Filename);

  // Load Configuration
  _configfile.loadFromFile(path);
  _automatic = _configfile.getInteger(L"common", L"automatic", AutomaticDefault);
  _startup = Windows::isProgramInAutostart();
}

Properties::~Properties()
{
}

void Properties::SetStartup(bool startup) {
  if (startup == _startup)
    return;

  Windows::setProgramToAutostart(startup);
}

void Properties::SetAutomatic(bool value) {
  if (value == _automatic)
    return;

  _automatic = value;
  _configfile.setInteger(L"common", L"automatic", value);
}
