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

#include "config.h"
#include "stdafx.h"

#include "utils.h"
#include <cpp-utils/preprocessor.h>
#include <lightports/user/application.h>
#include <lightports/extra/autostart.h>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

Properties::Properties() 
{
  // config dir
  fs::path configdir = Windows::Application::getConfigPath();
  configdir /= CPP_TO_WIDESTRING(PROJECT_NAME);
  fs::create_directories(configdir);

  // config file
  fs::path configfile = configdir / CPP_TO_WIDESTRING(PROJECT_NAME) L".ini";

  // load config
  _configfile.loadFromFile(configfile.wstring());

  // load properties
  _automatic = _configfile.getInteger(L"common", L"automatic", AutomaticDefault);
  icon_ = static_cast<IconEntry>(_configfile.getInteger(L"common", L"icon", IconDefault));
  _last_state = _configfile.getInteger(L"common", L"state", LastStateDefault);
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

void Properties::SetIcon(IconEntry value)
{
  if (value == icon_)
    return;

  icon_ = value;
  _configfile.setInteger(L"common", L"icon", icon_);
}

void Properties::SetLastState(bool value)
{
  if (value == _last_state)
    return;

  _last_state = value;
  _configfile.setInteger(L"common", L"state", _last_state);
}

