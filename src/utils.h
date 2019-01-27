/* 
 * This file is part of StayAwake.
 *
 *  Copyright (C) 2015  Richard Liebscher <r1tschy@yahoo.de>
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

std::wstring getResourceString(HINSTANCE hinstance, unsigned id);
HICON loadResourceIcon(HINSTANCE hinstance, WORD resourceid, int size = 0);

bool isScreensaverActive();

HWND getFullscreenWindow();
