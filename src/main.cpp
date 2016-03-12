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
#include <lightports/base/application.h>
#include "gettext.h"


static int run()
{
  using namespace boost::locale;

  generator gen;
  gen.add_messages_path("locale");
  gen.add_messages_domain(PACKAGE_NAME);
  std::locale::global(gen(""));

  try 
  {
    StayAwakeUi ui(Windows::Application::getInstance());
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
    MessageBoxA(NULL, e.what(), PACKAGE_NAME, MB_OK | MB_ICONERROR); 
    return 1;
  }
  catch(...)
  {
    OutputDebugStringA("Unknown exception happend.");
    MessageBoxA(NULL, "Unknown exception happend.", PACKAGE_NAME, MB_OK | MB_ICONERROR);
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

  Windows::Application app(wstring_literal(PACKAGE_NAME), hInst);
  return app.run(run);
}
