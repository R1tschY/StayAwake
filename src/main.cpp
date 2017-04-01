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

#include "config.h"
#include "stayawake.h"
#include "stdafx.h"
#include <iostream>

#include <cpp-utils/strings/string_literal.h>
#include <cpp-utils/preprocessor.h>
#include <lightports/user/application.h>
#include <boost/filesystem.hpp>
#include "gettext.h"

using namespace Windows;

static int run()
{
  using namespace boost::locale;

  boost::filesystem::path local_path = Application::getExecutablePath();
  local_path.remove_filename();
  local_path /= L"locale";

  generator gen;
  gen.add_messages_path(local_path.string());
  gen.add_messages_domain(PROJECT_NAME);
  std::locale::global(gen(""));

  try 
  {
    StayAwakeUi ui(Windows::Application::getHINSTANCE());
    if (!ui.okay())
    {
      // TODO: log failure
      return 100;
    }

    return Windows::Application::processMessages();
  }
  catch (const std::exception& e)
  {
    OutputDebugStringA(e.what());
    MessageBoxA(NULL, e.what(), PROJECT_NAME, MB_OK | MB_ICONERROR);
    return 1;
  }
  catch(...)
  {
    OutputDebugStringA("Unknown exception happend.");
    MessageBoxA(NULL, "Unknown exception happend.", PROJECT_NAME, MB_OK | MB_ICONERROR);
    return 1;
  }
}

int APIENTRY wWinMain(HINSTANCE hInst,
                     HINSTANCE hPrevInstance,
                     LPWSTR    lpCmdLine,
                     int       nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);
  UNREFERENCED_PARAMETER(nCmdShow);

  Windows::Application app(CPP_TO_WIDESTRING(PROJECT_NAME), hInst);
  return app.run(run);
}
