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

#pragma comment(lib, "comctl32.lib")

#include <Strsafe.h>
#include <cassert>

#include "StayAwake.h"

#include "TrayIcon.h"
#include "Utils.h"
#include "resource.h"
#include "version.h"

StayAwake::StayAwake(HWND hwnd) : 
  automatic_(false),
  manuell_state_(false),
  automatic_state_(false),

  last_state_(false),
  is_timer_active_(false),

  hwnd_(hwnd)
{

}

StayAwake::~StayAwake()
{
  if (is_timer_active_)
  {
    KillTimer(hwnd_, (UINT_PTR)this); 
  }
}

void StayAwake::setAutomatic(bool value)
{
  if (automatic_ == value) 
    return;

  automatic_ = value;

  updateTimer();
  update();
}

void StayAwake::setManuellState(bool value)
{
  if (manuell_state_ == value) 
    return;

  manuell_state_ = value;

  updateTimer();
  update();
}

void StayAwake::updateTimer()
{
  bool state = (automatic_ || manuell_state_);

  if (state == is_timer_active_)
    return;

  is_timer_active_ = state;

  if (state) 
  {
    SetTimer(hwnd_, (UINT_PTR)this, 30 * 1000, TimerProc);
  } 
  else 
  {
    KillTimer(hwnd_, (UINT_PTR)this); 
  }
}

void StayAwake::update()
{
  if (automatic_)
  {
    automatic_state_ = (getFullscreenWindow() != nullptr);    
  }

  bool state = getState();

  // if fullscreen window exists, work in polling mode.
  if (state) 
  {
    SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED);
  }
  
  if (state != last_state_) 
  {
    last_state_ = state;
    SendMessageW(hwnd_, WM_STATE_CHANGED, state, 0);

    if (!state) 
    {
      // if no fullscreen window exists anymore, then activate normal mode.
      SetThreadExecutionState(ES_CONTINUOUS);
    }
  }
}

void CALLBACK StayAwake::TimerProc(HWND hwnd, UINT msg, UINT_PTR id, DWORD time)
{
  if (msg != WM_TIMER)
    return;

  StayAwake* self = reinterpret_cast<StayAwake*>(id);
  assert(self != nullptr);

  self->update();
}

//
// UI

static
INT_PTR CALLBACK
About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
  UNREFERENCED_PARAMETER(lParam);
  switch (message)
  {
  case WM_INITDIALOG: {
    SetWindowTextW(hDlg, getResourceString(nullptr, IDS_ABOUT_TITLE).c_str());
    SetDlgItemTextW(hDlg, IDC_VERSION, TO_WIDESTRING(PACKAGE_NAME) L", Version " TO_WIDESTRING(PACKAGE_VERSION));
    SetDlgItemTextW(hDlg, IDC_COPYRIGHT, TO_WIDESTRING(PACKAGE_COPYRIGHT));
    SetDlgItemTextW(hDlg, IDC_GPL, getResourceString(nullptr, IDS_GPL).c_str());
    return (INT_PTR)TRUE;
  }

  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
    {
      EndDialog(hDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    }
    break;

  case WM_NOTIFY: {
    LPNMHDR evt = reinterpret_cast<LPNMHDR>(lParam); 
    if (evt->code == NM_CLICK || evt->code == NM_RETURN)
    {    
      if (evt->idFrom == IDC_GPL) {
        ShellExecute(hDlg, L"open", TO_WIDESTRING(GPL_WEBSITE), NULL, NULL, SW_SHOW);
      }
    }    
    break;
  }
  }
  return (INT_PTR)FALSE;
}

StayAwakeUi::StayAwakeUi(HINSTANCE hinstance) :
  hinstance_(hinstance),
  hwnd_(createWindow()),
  trayicon_(),
  coffein_(hwnd_),

  activate_string_(getResourceString(hinstance_, IDS_POPUP_SET)),
  deactivate_string_(getResourceString(hinstance_, IDS_POPUP_UNSET)),
  auto_activated_string_(getResourceString(hinstance_, IDS_POPUP_AUTO_SET))
{
  if (hwnd_ == nullptr)
    return;

  auto autostart_flags = properties_.GetStartup()
    ? Windows::MenuEntryFlags::Checked
    : Windows::MenuEntryFlags::Unchecked;
  auto automatic_flags = properties_.GetAutomatic()
    ? Windows::MenuEntryFlags::Checked
    : Windows::MenuEntryFlags::Unchecked;

  popup_menu_ = std::move(Windows::Menu::createPopupMenu());
  popup_menu_.addEntry(InfoEntry, getResourceString(hinstance_, IDS_POPUP_INFO));
  popup_menu_.addSeperator();
  popup_menu_.addEntry(OptionsEntry, getResourceString(hinstance_, IDS_POPUP_OPTIONS), Windows::MenuEntryFlags::Disabled);
  popup_menu_.addEntry(AutostartEntry, getResourceString(hinstance_, IDS_POPUP_AUTOSTART), autostart_flags);
  popup_menu_.addEntry(AutomaticEntry, getResourceString(hinstance_, IDS_POPUP_AUTOMATIC), automatic_flags);
  popup_menu_.addSeperator();
  popup_menu_.addEntry(SetManuellEntry, activate_string_.c_str());
  popup_menu_.addSeperator();
  popup_menu_.addEntry(ExitEntry, getResourceString(hinstance_, IDS_POPUP_EXIT));
  trayicon_.add(hwnd_, loadResourceIcon(hinstance_, IDI_CUP_EMPTY, 0));  

  coffein_.setAutomatic(properties_.GetAutomatic());
  coffein_.update();
}

StayAwakeUi::~StayAwakeUi()
{
  destroy();
}

void
StayAwakeUi::destroy()
{
  if (hwnd_) 
  {
    DestroyWindow(hwnd_);
    hwnd_ = nullptr;
  }
}

ATOM
StayAwakeUi::registerWindowClass()
{
  WNDCLASSEXW wcex;

  wcex.cbSize = sizeof(WNDCLASSEXW);

  wcex.style		= CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc	= &StayAwakeUi::MessageEntry;
  wcex.cbClsExtra	= 0;
  wcex.cbWndExtra	= 0;
  wcex.hInstance	= hinstance_;
  wcex.hIcon		= LoadIcon(hinstance_, MAKEINTRESOURCE(IDI_CUP_FULL));
  wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszMenuName	= nullptr;
  wcex.lpszClassName	= COFFEIN_WINDOW_CLASS;
  wcex.hIconSm		= LoadIcon(hinstance_, MAKEINTRESOURCE(IDI_CUP_FULL));

  return RegisterClassExW(&wcex);
}

HWND
StayAwakeUi::createWindow()
{
  registerWindowClass();
  HWND hwnd = CreateWindowW(
    COFFEIN_WINDOW_CLASS, 
    TO_WIDESTRING(PACKAGE_NAME), 
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, 
    0, 
    CW_USEDEFAULT, 
    0, 
    NULL, 
    NULL, 
    hinstance_,
    this);

  if (!hwnd) 
  {
    handleWindowsError(L"", GetLastError());
  }
  return hwnd;
}

void 
StayAwakeUi::onManuellSet(bool value)
{
  if (value == coffein_.getManuellState())  
    return;
  
  coffein_.setManuellState(value);
}

void
StayAwakeUi::onStateChanged(bool newstate)
{
  if (coffein_.getAutomaticState())
  {
    popup_menu_.modifyEntry(
      SetManuellEntry, 
      auto_activated_string_.c_str(),
      Windows::MenuEntryFlags::_(Windows::MenuEntryFlags::Disabled | Windows::MenuEntryFlags::Grayed));
    trayicon_.setIcon(loadResourceIcon(hinstance_, IDI_CUP_FULL, 0));
    return;
  }

  if (coffein_.getManuellState()) {
    popup_menu_.modifyEntry(
      SetManuellEntry, 
      deactivate_string_.c_str());

    trayicon_.setIcon(loadResourceIcon(hinstance_, IDI_CUP_FULL, 0));
  }
  else
  {
    popup_menu_.modifyEntry(
      SetManuellEntry, 
      activate_string_.c_str());

    trayicon_.setIcon(loadResourceIcon(hinstance_, IDI_CUP_EMPTY, 0));
  }
}

void StayAwakeUi::onAutostartSet(bool value)
{
  properties_.SetStartup(value);

  auto flags = value
    ? Windows::MenuEntryFlags::Checked
    : Windows::MenuEntryFlags::Unchecked;
  popup_menu_.modifyEntry(
    AutostartEntry, 
    getResourceString(hinstance_, IDS_POPUP_AUTOSTART),
    flags);
}

void StayAwakeUi::onAutomaticSet(bool value)
{
  properties_.SetAutomatic(value);
  coffein_.setAutomatic(value);
  
  auto flags = value
    ? Windows::MenuEntryFlags::Checked
    : Windows::MenuEntryFlags::Unchecked;
  popup_menu_.modifyEntry(
    AutomaticEntry, 
    getResourceString(hinstance_, IDS_POPUP_AUTOMATIC),
    flags);
}

void StayAwakeUi::onAbout()
{
  DialogBox(hinstance_, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd_, About);
}

void StayAwakeUi::onContextMenu() 
{
  POINT pt;

  SetForegroundWindow(hwnd_);
  GetCursorPos(&pt);
  TrackPopupMenu(popup_menu_.getHMENU(), 
		  TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
		  pt.x, pt.y, 0, hwnd_, nullptr); 

  return true; 
}

LRESULT 
StayAwakeUi::onMessage(UINT msg, WPARAM wparam, LPARAM lparam) 
{
  int wmId, wmEvent;

  switch (msg)
  {
  case WM_NCCREATE:
    return TRUE;

  case WM_CREATE:
    return TRUE;

  // Messages des PopupMenus
  case WM_COMMAND:
    wmId    = LOWORD(wparam);
    wmEvent = HIWORD(wparam);
    // Menüauswahl bearbeiten:
    switch (wmId)
    {
    case InfoEntry:
      onAbout();
      break;

    case AutostartEntry:
      onAutostartSet(!popup_menu_.isEntryChecked(AutostartEntry));
      break;

    case AutomaticEntry:
      onAutomaticSet(!popup_menu_.isEntryChecked(AutomaticEntry));
      break;

    case SetManuellEntry:
      onManuellSet(!coffein_.getManuellState());
      break;

    case ExitEntry:
      destroy();
      break;

    default:
      return DefWindowProc(hwnd_, msg, wparam, lparam);
    }
    break;

  case WM_POWERBROADCAST:
    if (wparam == PBT_APMQUERYSUSPEND && coffein_.getState())
    {
      if (lparam & 1)
      {
        // deny standby
        return BROADCAST_QUERY_DENY;
      }
      else
      {
        // cannot deny standby
        return TRUE;
      }      
    }
    return TRUE;

    // Messages des TrayIcon
  case WM_TRAY_ICON_MESSAGE:
    switch (lParam)
    {
    case WM_LBUTTONDOWN:
      onManuellSet(!coffein_.getManuellState());
      return 1;
    
    case WM_RBUTTONDOWN: 
    case WM_CONTEXTMENU: 
      onContextMenu();
      return 1;
    }
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  case StayAwake::WM_STATE_CHANGED:
    onStateChanged(wparam != 0);
    break;

  default:
    return DefWindowProc(hwnd_, msg, wparam, lparam);
  }
  return 0;
}

LRESULT CALLBACK
StayAwakeUi::MessageEntry(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam) 
{
  auto window = reinterpret_cast<StayAwakeUi*>(GetWindowLongPtr(handle, GWLP_USERDATA));
  if (!window) {
    if (msg == WM_NCCREATE) {
      auto create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
      window = static_cast<StayAwakeUi*>(create_struct->lpCreateParams);
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
