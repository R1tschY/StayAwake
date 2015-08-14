/* Coffeine.h 
 *
 *  Copyright (C) 2010,2015  Richard Liebscher
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

#pragma once

#include "stdafx.h"

#include "Properties.h"
#include "PropertiesDialog.h"
#include "TrayIcon.h"
#include "resource.h"

#define COFFEIN_WINDOW_CLASS L"COFFEIN"

class Coffeine
{
  Coffeine(const Coffeine&);
  Coffeine& operator=(const Coffeine&);
public:
  static const UINT WM_STATE_CHANGED = WM_USER + 0xC;

  Coffeine(HWND hwnd);
  ~Coffeine();

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

class CoffeineUi
{
  CoffeineUi(const CoffeineUi&);
  CoffeineUi& operator=(const CoffeineUi&);
public:
  CoffeineUi(HINSTANCE hInstance);
  ~CoffeineUi();

  HWND getWindow() const { return hwnd_; }

  bool okay() const { return hwnd_ != nullptr; }

  void destroy();

  void onStateChanged(bool newstate);
  void onManuellSet(bool value);

private:
  HINSTANCE hinstance_;
  std::wstring app_title_;

  HWND hwnd_;

  TrayIcon trayicon_;
  HMENU popup_menu_;

  Coffeine coffein_;
  Properties properties_;
  PropertiesDialog properties_dialog_;

  LRESULT onMessage(UINT msg, WPARAM wparam, LPARAM lparam);

  ATOM registerWindowClass();
  HWND createWindow();

  static LRESULT CALLBACK MessageEntry(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
