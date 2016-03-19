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

#include "stayawake.h"

#include "stdafx.h"

#include <strsafe.h>
#include <utility>

#include "config.h"
#include <cpp-utils/preprocessor.h>
#include <cpp-utils/assert.h>
#include <cpp-utils/strings/string_literal.h>
#include <lightports/core.h>
#include "gettext.h"

#include "resource.h"
#include "utils.h"

using namespace Windows;

StayAwake::StayAwake(HWND hwnd) :
  automatic_(false),
  manuell_state_(false),
  automatic_state_(false),

  last_state_(false),
  is_timer_active_(false),

  hwnd_(hwnd),
  timeout_([=](){ update(); }, -1)
{ }

StayAwake::~StayAwake()
{
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
    timeout_.setInterval(30 * 1000);
  }
  else
  {
    timeout_.setInterval(-1);
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
    SetWindowTextW(hDlg, P_(L"AboutDialog", L"Info about ...").c_str());
    SetDlgItemTextW(hDlg, IDC_VERSION, CPP_TO_WIDESTRING(PROJECT_NAME) L", Version " CPP_TO_WIDESTRING(PROJECT_VERSION));
    SetDlgItemTextW(hDlg, IDC_COPYRIGHT, CPP_TO_WIDESTRING(PROJECT_COPYRIGHT));
    SetDlgItemTextW(hDlg, IDC_GPL, _(L"This is free software and uses the <a>GNU General Public License Version 3</a>.").c_str());
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
        ShellExecuteW(hDlg, L"open", L"http://www.gnu.org/licenses/gpl-3.0.html", NULL, NULL, SW_SHOW);
      }
    }
    break;
  }
  }
  return (INT_PTR)FALSE;
}

StayAwakeUi::StayAwakeUi(HINSTANCE hinstance) :
  Windows::Window(Window::Type::Normal),

  hinstance_(hinstance),
  trayicon_(),
  coffein_(0)
{
  create(nullptr, wstring_literal("StayAwake"));
}

void StayAwakeUi::onCreate()
{
  auto autostart_flags = properties_.GetStartup()
    ? Windows::MenuEntryFlags::Checked
    : Windows::MenuEntryFlags::Unchecked;
  auto automatic_flags = properties_.GetAutomatic()
    ? Windows::MenuEntryFlags::Checked
    : Windows::MenuEntryFlags::Unchecked;

  IconEntry icon = properties_.GetIcon();
  onIconSet(icon);

  popup_menu_ = Windows::Menu::createPopupMenu();
  popup_menu_.addEntry(InfoEntry, _(L"Info ..."));
  popup_menu_.addSeperator();
  popup_menu_.addEntry(OptionsEntry, _(L"Options"), Windows::MenuEntryFlags::Disabled);
  popup_menu_.addEntry(AutostartEntry, _(L"Start with Windows"), autostart_flags);
  popup_menu_.addEntry(AutomaticEntry, _(L"Activate with fullscreen window"), automatic_flags);
  icon_menu_ = Windows::Menu::createPopupMenu();
  icon_menu_.addEntry(ChocoEntry, _(L"Choco icon"), icon == ChocoEntry ? Windows::MenuEntryFlags::Checked : Windows::MenuEntryFlags::Unchecked );
  icon_menu_.addEntry(TeaEntry, _(L"Tea icon"), icon == TeaEntry ? Windows::MenuEntryFlags::Checked : Windows::MenuEntryFlags::Unchecked);
  popup_menu_.addMenu(_(L"Set icon"), icon_menu_);
  popup_menu_.addSeperator();
  popup_menu_.addEntry(SetManuellEntry, _(L"Activate"));
  popup_menu_.addSeperator();
  popup_menu_.addEntry(ExitEntry, _(L"Quit"));
  trayicon_.add(getNativeHandle(), inactive_icon_, wstring_literal(PROJECT_NAME));

  coffein_.setParent(getNativeHandle());
  coffein_.setAutomatic(properties_.GetAutomatic());
  coffein_.update();
}

StayAwakeUi::~StayAwakeUi()
{
}

void
StayAwakeUi::onDestroy()
{
  trayicon_.remove();
  PostQuitMessage(0);
}

void
StayAwakeUi::onManuellSet(bool value)
{
  if (value == coffein_.getManuellState())
    return;

  coffein_.setManuellState(value);
}

void
StayAwakeUi::onStateChanged()
{
  if (coffein_.getAutomaticState())
  {
    popup_menu_.modifyEntry(SetManuellEntry, _(L"Automatic activated"), MenuEntryFlags::Grayed);
    trayicon_.setIcon(active_icon_);
    return;
  }

  if (coffein_.getManuellState()) {
    popup_menu_.modifyEntry(SetManuellEntry, _(L"Deactivate"), MenuEntryFlags::Enabled);
    trayicon_.setIcon(active_icon_);
  }
  else
  {
    popup_menu_.modifyEntry(SetManuellEntry, _(L"Activate"), MenuEntryFlags::Enabled);
    trayicon_.setIcon(inactive_icon_);
  }
}

void StayAwakeUi::onAutostartSet(bool value)
{
  properties_.SetStartup(value);
  popup_menu_.check(AutostartEntry, value);
}

void StayAwakeUi::onAutomaticSet(bool value)
{
  properties_.SetAutomatic(value);
  coffein_.setAutomatic(value);
  popup_menu_.check(AutomaticEntry, value);
}

void StayAwakeUi::onIconSet(IconEntry icon)
{
  switch(icon)
  {
  case ChocoEntry:
    active_icon_ = loadResourceIcon(hinstance_, IDI_CHOCO_FULL, 0);
    inactive_icon_ = loadResourceIcon(hinstance_, IDI_CHOCO_EMPTY, 0);
    break;

  case TeaEntry:
    active_icon_ = loadResourceIcon(hinstance_, IDI_TEA_FULL, 0);
    inactive_icon_ = loadResourceIcon(hinstance_, IDI_TEA_EMPTY, 0);
    break;
  }

  icon_menu_.check(ChocoEntry, icon == ChocoEntry);
  icon_menu_.check(TeaEntry, icon == TeaEntry);

  properties_.SetIcon(icon);
  onStateChanged();
}

void StayAwakeUi::onAbout()
{
  DialogBox(hinstance_, MAKEINTRESOURCE(IDD_ABOUTBOX), getNativeHandle(), About);
}

void StayAwakeUi::onContextMenu(int x, int y)
{
  SetForegroundWindow(getNativeHandle());
  TrackPopupMenu(popup_menu_.getHMENU(),
		  TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                 x, y, 0, getNativeHandle(), nullptr);
}

LRESULT
StayAwakeUi::onMessage(UINT msg, WPARAM wparam, LPARAM lparam)
{
  int wmId, wmEvent;

  switch (msg)
  {
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

    case ChocoEntry:
    case TeaEntry:
      onIconSet(static_cast<IconEntry>(wmId));
      break;

    case ExitEntry:
      destroy();
      break;
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
  case Windows::TrayIcon::MessageId:
    trayicon_.handleMessage(wparam, lparam, [=](UINT timsg, int x, int y)
    {
      POINT pt;
      GetCursorPos(&pt);

      switch (timsg)
      {
      case WM_LBUTTONDOWN:
        onManuellSet(!coffein_.getManuellState());
        return 1;

      case WM_RBUTTONDOWN:
      case WM_CONTEXTMENU:
        onContextMenu(pt.x, pt.y);
        return 1;
      }
      return 0;
    });
    break;

  case StayAwake::WM_STATE_CHANGED:
    onStateChanged();
    break;
  }

  return Window::onMessage(msg, wparam, lparam);
}
