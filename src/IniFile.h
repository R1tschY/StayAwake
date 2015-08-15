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

#ifndef __INIFILES_H__
#define __INIFILES_H__

#include "stdafx.h"

#include <string>
#include <vector>

class IniFile {
public:
  IniFile();
  explicit IniFile(const std::wstring& filename);
  virtual ~IniFile();

  std::wstring getString (const wchar_t* section, const wchar_t* key, const wchar_t* defaultvalue) const;
  bool         getBoolean(const wchar_t* section, const wchar_t* key, bool defaultvalue) const;

  std::vector<std::wstring> getSections() const;

  bool setString (const wchar_t* section, const wchar_t* key, const wchar_t* str) const;
  bool setBoolean(const wchar_t* section, const wchar_t* key, bool value) const {
    return setString(section, key, value?L"1":L"0");
  }
//  bool setInteger(const wchar_t* section, const wchar_t* key, int value) const {
//    return setString(section, key, IntegerToStringT(value));
//  }

private:
  std::wstring _filename;
};

#endif // __INIFILES_H__

