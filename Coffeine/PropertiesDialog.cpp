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
#include <Psapi.h>
#include <cassert>

#include "Coffeine.h"

#include "Properties.h"
#include "Utils.h"
#include "resource.h"
#include "PropertiesDialog.h"

PropertiesDialog::PropertiesDialog(HINSTANCE hInst, Properties& props) :
  hinstance_(hInst),
  props_(props)
{

}

PropertiesDialog::~PropertiesDialog() 
{
  if (hwnd_)
  {
    EndDialog(hwnd_, IDCANCEL);
    hwnd_ = nullptr;
  }
}

void PropertiesDialog::show(HWND parent) 
{
  DialogBoxParamW(hinstance_, MAKEINTRESOURCE(IDD_PROPERTIES), parent, &PropertiesDialog::MessageEntry, (LPARAM)this);
}

void PropertiesDialog::onCreate() 
{
  HWND hwnddesktop = GetDesktopWindow(); 

  RECT rcdesktop, rcdlg, rc;

  GetWindowRect(hwnddesktop, &rcdesktop); 
  GetWindowRect(hwnd_, &rcdlg); 
  CopyRect(&rc, &rcdesktop); 

  // Offset the owner and dialog box rectangles so that right and bottom 
  // values represent the width and height, and then offset the owner again 
  // to discard space taken up by the dialog box. 

  OffsetRect(&rcdlg, -rcdlg.left, -rcdlg.top); 
  OffsetRect(&rc, -rc.left, -rc.top); 
  OffsetRect(&rc, -rcdlg.right, -rcdlg.bottom); 

  // The new position is the sum of half the remaining space and the owner's 
  // original position. 

  SetWindowPos(hwnd_, 
    HWND_TOP, 
    rcdesktop.left + (rc.right / 2), 
    rcdesktop.top + (rc.bottom / 2), 
    0, 0,          // Ignores size arguments. 
    SWP_NOSIZE);

  // Set dialog icon
  SendMessage(hwnd_, WM_SETICON, ICON_SMALL, (LPARAM)loadResourceIcon(hinstance_, IDI_SMALL, 0));

  // Set checkbox values 
  Button_SetCheck(GetDlgItem(hwnd_, IDC_CHECK_STARTUP), 
    (props_.GetStartup())?BST_CHECKED:BST_UNCHECKED);
  Button_SetCheck(GetDlgItem(hwnd_, IDC_CHECK_ABODE), 
    (props_.GetAutomatic())?BST_CHECKED:BST_UNCHECKED);
}

INT_PTR PropertiesDialog::onMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
  UNREFERENCED_PARAMETER(lParam);

  switch (message)
  {
  case WM_INITDIALOG:
    onCreate();
    return (INT_PTR)TRUE;

  case WM_COMMAND:
    // Ok button events
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
    {
      EndDialog(hwnd_, LOWORD(wParam));
      hwnd_ = nullptr;
      return (INT_PTR)TRUE;
    }

    // checkbutton events
    if (HIWORD(wParam) == BN_CLICKED)
    {
      switch (LOWORD(wParam)) {
      case IDC_CHECK_STARTUP:
        props_.SetStartup(Button_GetCheck((HWND)lParam) == BST_CHECKED);
        break;
      case IDC_CHECK_ABODE:
        props_.SetAutomatic(Button_GetCheck((HWND)lParam) == BST_CHECKED);
        break;
      }
    }

    break;
  }

  return (INT_PTR)FALSE;
}

INT_PTR CALLBACK
PropertiesDialog::MessageEntry(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam) 
{
  auto window = reinterpret_cast<PropertiesDialog*>(GetWindowLongPtr(handle, GWLP_USERDATA));
  if (!window) {
    if (msg == WM_INITDIALOG) {
      window = reinterpret_cast<PropertiesDialog*>(lparam);
      assert(window);

      SetWindowLongPtr(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
      window->hwnd_ = handle;
    }

    if (!window)
    {
      return DefWindowProc(handle, msg, wparam, lparam);
    }
  }

  return window->onMessage(msg, wparam, lparam);
}
