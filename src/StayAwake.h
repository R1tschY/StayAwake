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

#pragma once

#include "stdafx.h"

#include "Properties.h"
#include "TrayIcon.h"
#include "Menu.h"
#include "resource.h"

#define COFFEIN_WINDOW_CLASS L"StayAwakeWindowClass"

class StayAwake
{
  StayAwake(const StayAwake&);
  StayAwake& operator=(const StayAwake&);
public:
  static const UINT WM_STATE_CHANGED = WM_USER + 0xC;

  StayAwake(HWND hwnd);
  ~StayAwake();

  void setAutomatic(bool value);
  bool isAutomaticSet() const { return automatic_; }

  void setManuellState(bool value);
  bool getManuellState() const { return manuell_state_; }

  bool getAutomaticState() const { return automatic_ && automatic_state_; }

  bool getState() const { return manuell_state_ || (automatic_ && automatic_state_); }
  void update();

private:
  bool automatic_;
  bool manuell_state_;
  bool automatic_state_;

  bool last_state_;
  bool is_timer_active_;

  HWND hwnd_;

  void updateTimer();

  static void CALLBACK TimerProc(HWND hwnd, UINT msg, UINT_PTR id, DWORD time);
};

class StayAwakeUi
{
  StayAwakeUi(const StayAwakeUi&);
  StayAwakeUi& operator=(const StayAwakeUi&);
public:
  StayAwakeUi(HINSTANCE hInstance);
  ~StayAwakeUi();

  HWND getWindow() const { return hwnd_; }

  bool okay() const { return hwnd_ != nullptr; }

  void destroy();

  void onStateChanged(bool newstate);
  void onManuellSet(bool value);
  void onAutostartSet(bool value);
  void onAutomaticSet(bool value);
  void onAbout();
  void onContextMenu();

private:
  enum PopupEntry {
    InfoEntry = 1000,
    // ---
    OptionsEntry,
    AutostartEntry,
    AutomaticEntry,
    // ---
    SetManuellEntry,
    // ---
    ExitEntry
  };

  HINSTANCE hinstance_;
  HWND hwnd_;

  TrayIcon trayicon_;
  Windows::Menu popup_menu_;

  StayAwake coffein_;
  Properties properties_;

  LRESULT onMessage(UINT msg, WPARAM wparam, LPARAM lparam);

  ATOM registerWindowClass();
  HWND createWindow();

  std::wstring activate_string_;
  std::wstring deactivate_string_;
  std::wstring auto_activated_string_;

  static LRESULT CALLBACK MessageEntry(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
