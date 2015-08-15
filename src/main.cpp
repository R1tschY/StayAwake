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

#include "StayAwake.h"
#include "version.h"


int APIENTRY _tWinMain(HINSTANCE hInst,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  // Testen ob StayAwake schon läuft
  if (FindWindow(COFFEIN_WINDOW_CLASS, nullptr) != nullptr)
    return 0;

  try 
  {
    StayAwakeUi ui(hInst);
    if (!ui.okay())
    {
      // TODO: log failure
      return 100;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    return (int) msg.wParam;
  }
  catch (const std::exception& e)
  {
    MessageBoxA(NULL, e.what(), PACKAGE_NAME, MB_OK | MB_ICONERROR); 
    return 1;
  }
  catch(...)
  {
    MessageBoxA(NULL, "Unknown exception happend.", PACKAGE_NAME, MB_OK | MB_ICONERROR); 
    return 1;
  }  
}