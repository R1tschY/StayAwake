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

#pragma once

#include "stdafx.h"

#include <lightports/user/menu.h>
#include <lightports/user/icon.h>
#include <lightports/shell/trayicon.h>
#include <lightports/user/timeout.h>
#include <lightports/controls/toplevelwindow.h>

#include "properties.h"
#include "resource.h"

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
  void setParent(HWND hwnd) { hwnd_ = hwnd; }

private:
  bool automatic_;
  bool manuell_state_;
  bool automatic_state_;

  bool last_state_;
  bool is_timer_active_;

  HWND hwnd_;
  Windows::Timeout timeout_;

  void updateTimer();
};

class StayAwakeUi : public Windows::TopLevelWindow
{
public:
  StayAwakeUi(HINSTANCE hInstance);
  ~StayAwakeUi();

  void onStateChanged();
  void onManuellSet(bool value);
  void onAutostartSet(bool value);
  void onAutomaticSet(bool value);
  void onAbout();
  void onContextMenu(int x, int y);
  void onIconSet(IconEntry icon);

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

  Windows::TrayIcon trayicon_;
  Windows::Menu popup_menu_;
  Windows::Menu icon_menu_;

  Windows::IconView active_icon_;
  Windows::IconView inactive_icon_;

  StayAwake coffein_;
  Properties properties_;

  void onCreate() override;
  void onDestroy() override;
  LRESULT onMessage(UINT msg, WPARAM wparam, LPARAM lparam) override;

  static LRESULT CALLBACK MessageEntry(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
