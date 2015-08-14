/* Properties.h 
 *
 *  Copyright (C) 2010, 2015 Richard Liebscher
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

class PropertiesDialog
{
  PropertiesDialog(const PropertiesDialog&);
  PropertiesDialog& operator=(const PropertiesDialog&);
public:
  PropertiesDialog(HINSTANCE hinstance, Properties& props);
  ~PropertiesDialog();

  void show(HWND parent);

private:
  HWND hwnd_;
  HINSTANCE hinstance_;

  Properties& props_;

  static INT_PTR CALLBACK MessageEntry(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  INT_PTR onMessage(UINT msg, WPARAM wparam, LPARAM lparam);

  void onCreate();
};