/* 
 * This file is part of StayAwake.
 *
 *  Copyright (C) 2019  Richard Liebscher <r1tschy@yahoo.de>
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

#include <libintl.h>

#include <cstdio>  // libintl.h defines macros for *printf. cstdio undefines these.
#include <iostream>

#include <boost/filesystem.hpp>
#include <lightports/extra/charcodecs.h>
#include <lightports/user/application.h>

#include "gettext.h"
#include "config.h"

void init_gettext()
{
  boost::filesystem::path local_path = Windows::Application::getExecutablePath();
  local_path.remove_filename();
  local_path /= L"locale";

  ::setlocale(LC_ALL, "");
  ::bindtextdomain(PROJECT_NAME, local_path.string().c_str());
  ::bind_textdomain_codeset(PROJECT_NAME, "UTF-8");
  ::textdomain(PROJECT_NAME);
}

std::wstring wgettext(const char *msgid)
{
  return Windows::to_wstring(::gettext(msgid));
}
