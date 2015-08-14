/* Coffeine.cpp 
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

#pragma comment(lib, "comctl32.lib")

#include <Strsafe.h>
#include <cassert>

#include "Coffeine.h"

#include "TrayIcon.h"
#include "Utils.h"
#include "PropertiesDialog.h"

void ShowMessage(LPCWSTR msg);
void HandleWindowsError(LPCWSTR msg, DWORD code);

int APIENTRY _tWinMain(HINSTANCE hInst,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  // Testen ob Coffeine schon läuft
  if (FindWindow(COFFEIN_WINDOW_CLASS, nullptr) != nullptr)
    return 0;

  CoffeineUi ui(hInst);
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

inline static
bool isScreensaverActive()
{
  BOOL result;
  SystemParametersInfoW(SPI_GETSCREENSAVERRUNNING, 0, &result, 0);
  return result == TRUE;
}

static
HWND getFullscreenWindow()
{
  RECT desktop_rect;
  HWND desktop_window = GetDesktopWindow();
  RECT app;
  HWND hwnd = GetForegroundWindow();

  if (   desktop_window != nullptr
      && hwnd != nullptr
      && hwnd != desktop_window
      && hwnd != GetShellWindow()
      && GetWindowRect(desktop_window, &desktop_rect)
      && GetWindowRect(hwnd, &app)
      && app.top    <= desktop_rect.top
      && app.bottom >= desktop_rect.bottom
      && app.right  >= desktop_rect.right
      && app.left   <= desktop_rect.left // TODO: Rect.isWarping()
      && !isScreensaverActive())
  {
    return hwnd;
  } 
  else 
  {
    return nullptr;
  }
}

Coffeine::Coffeine(HWND hwnd) : 
  automatic_(false),
  manuell_state_(false),
  automatic_state_(false),

  last_state_(false),
  is_timer_active_(false),

  hwnd_(hwnd)
{
}

Coffeine::~Coffeine()
{
  if (is_timer_active_)
  {
    KillTimer(hwnd_, (UINT_PTR)this); 
  }
}

void Coffeine::setAutomatic(bool value)
{
  if (automatic_ == value) 
    return;

  automatic_ = value;

  updateTimer();
  update();
}

void Coffeine::setManuellState(bool value)
{
  if (manuell_state_ == value) 
    return;

  manuell_state_ = value;

  updateTimer();
  update();
}

void Coffeine::updateTimer()
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

void Coffeine::update()
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

void CALLBACK Coffeine::TimerProc(HWND hwnd, UINT msg, UINT_PTR id, DWORD time)
{
  if (msg != WM_TIMER)
    return;

  Coffeine* self = reinterpret_cast<Coffeine*>(id);
  assert(self != nullptr);

  self->update();
}

//
// UI

INT_PTR CALLBACK 
About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
  UNREFERENCED_PARAMETER(lParam);
  switch (message)
  {
  case WM_INITDIALOG:
    return (INT_PTR)TRUE;

  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
    {
      EndDialog(hDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    }
    break;
  }
  return (INT_PTR)FALSE;
}

CoffeineUi::CoffeineUi(HINSTANCE hinstance) :
  hinstance_(hinstance),
  app_title_(getResourceString(hinstance_, IDS_APP_TITLE)),
  hwnd_(createWindow()),
  trayicon_(),
  coffein_(hwnd_),
  properties_dialog_(hinstance, properties_)
{
  if (hwnd_ == nullptr)
    return;

  coffein_.setAutomatic(properties_.GetAutomatic());
  coffein_.update();

  trayicon_.add(hwnd_, loadResourceIcon(hinstance_, IDI_CUP_EMPTY, 0));  

  HMENU hmenu = LoadMenu(hinstance_, MAKEINTRESOURCE(IDC_COFFEIN));
  popup_menu_ = GetSubMenu(hmenu, 0);
  trayicon_.setPopupMenu(popup_menu_);
}

CoffeineUi::~CoffeineUi()
{
  destroy();
}

void
CoffeineUi::destroy()
{
  if (hwnd_) 
  {
    DestroyWindow(hwnd_);
    hwnd_ = nullptr;
  }
}

ATOM
CoffeineUi::registerWindowClass()
{
  WNDCLASSEXW wcex;

  wcex.cbSize = sizeof(WNDCLASSEXW);

  wcex.style		= CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc	= &CoffeineUi::MessageEntry;
  wcex.cbClsExtra	= 0;
  wcex.cbWndExtra	= 0;
  wcex.hInstance	= hinstance_;
  wcex.hIcon		= LoadIcon(hinstance_, MAKEINTRESOURCE(IDI_COFFEIN));
  wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszMenuName	= MAKEINTRESOURCEW(IDC_COFFEIN);
  wcex.lpszClassName	= COFFEIN_WINDOW_CLASS;
  wcex.hIconSm		= LoadIcon(hinstance_, MAKEINTRESOURCE(IDI_SMALL));

  return RegisterClassExW(&wcex);
}

HWND
CoffeineUi::createWindow()
{
  registerWindowClass();
  HWND hwnd = CreateWindowW(
    COFFEIN_WINDOW_CLASS, 
    app_title_.c_str(), 
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
CoffeineUi::onManuellSet(bool value)
{
  if (value == coffein_.getManuellState())  
    return;
  
  coffein_.setManuellState(value);
}

void
CoffeineUi::onStateChanged(bool newstate)
{
  if (coffein_.getAutomaticState())
  {
    ModifyMenu(popup_menu_,
      IDM_SET,
      MF_BYCOMMAND | MF_STRING | MF_DISABLED | MF_GRAYED,
      IDM_SET,
      L"Aktiviert (wegen Vollbildfenster)");
    trayicon_.setIcon(loadResourceIcon(hinstance_, IDI_CUP_FULL, 0));
    return;
  }

  if (coffein_.getManuellState()) {
    ModifyMenu(popup_menu_,
      IDM_SET,
      MF_BYCOMMAND | MF_STRING,
      IDM_SET,
      L"Deaktivieren");

    trayicon_.setIcon(loadResourceIcon(hinstance_, IDI_CUP_FULL, 0));
  }
  else
  {
    ModifyMenu(popup_menu_,
      IDM_SET,
      MF_BYCOMMAND | MF_STRING,
      IDM_SET,
      L"Aktivieren");

    trayicon_.setIcon(loadResourceIcon(hinstance_, IDI_CUP_EMPTY, 0));
  }
}

LRESULT 
CoffeineUi::onMessage(UINT msg, WPARAM wparam, LPARAM lparam) 
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
    case IDM_ABOUT:
      DialogBox(hinstance_, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd_, About);
      break;

    case IDM_PRO:
      properties_dialog_.show(hwnd_);
      coffein_.setAutomatic(properties_.GetAutomatic());
      coffein_.update();
      break;

    case IDM_EXIT:
      destroy();
      break;

    case IDM_SET:
      onManuellSet(!coffein_.getManuellState());
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
    trayicon_.WinProcHandler(hwnd_, msg, wparam, lparam);
    break;

  case WM_DESTROY:
    DestroyMenu(popup_menu_);
    PostQuitMessage(0);
    break;

  case Coffeine::WM_STATE_CHANGED:
    onStateChanged(wparam != 0);
    break;

  default:
    return DefWindowProc(hwnd_, msg, wparam, lparam);
  }
  return 0;
}

LRESULT CALLBACK
CoffeineUi::MessageEntry(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam) 
{
  auto window = reinterpret_cast<CoffeineUi*>(GetWindowLongPtr(handle, GWLP_USERDATA));
  if (!window) {
    if (msg == WM_NCCREATE) {
      auto create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
      window = static_cast<CoffeineUi*>(create_struct->lpCreateParams);
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
