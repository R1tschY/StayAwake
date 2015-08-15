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

#define _STRINGIFY(x) #x
#define STRINGIFY(x) _STRINGIFY(x)

#define _TO_WIDESTRING(x) L ## x
#define TO_WIDESTRING(x) _TO_WIDESTRING(x)
#define WSTRINGIFY(x) TO_WIDESTRING(_STRINGIFY(x))


template <typename T, std::size_t N>
inline
std::size_t length(const T(&) [N]) {
  return N;
}

template <typename Container, std::size_t N>
inline
std::size_t length(const Container& t) {
  return t.size();
}


std::wstring getResourceString(HINSTANCE hinstance, unsigned id);
HICON loadResourceIcon(HINSTANCE hinstance, WORD resourceid, int size = 0);

void handleWindowsError(LPCWSTR msg, DWORD code);

bool isScreensaverActive();

HWND getFullscreenWindow();

std::wstring getApplicationExecutablePath();

bool setProgramToAutostart(bool value = true);
bool isProgramInAutostart();