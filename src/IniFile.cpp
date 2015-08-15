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

#include "IniFile.h"

#include <errno.h>

#include "Utils.h"

IniFile::IniFile():
_filename()
{}

IniFile::IniFile(const std::wstring& filename):
_filename(filename)
{}

IniFile::~IniFile() {}

std::wstring
IniFile::getString(const wchar_t* section, const wchar_t* key, const wchar_t* defaultvalue) const {
  wchar_t* buffer;
  DWORD buffersize = 512;
  DWORD bytes;

  do {    
    buffer = (wchar_t*) alloca(buffersize);
    bytes = GetPrivateProfileStringW(section, key, defaultvalue, buffer, buffersize, _filename.c_str());

    if (bytes == buffersize-1) {// buffer to small
      buffersize *= 2;
      free(buffer);
      continue;
    }
  } while (false); 

  if (errno == 0x2) { // File not found
    handleWindowsError(L"Cannot read ini file", GetLastError());
    return defaultvalue;
  }

  return std::wstring(buffer);
}

bool
IniFile::getBoolean(const wchar_t* section, const wchar_t* key, bool defaultvalue) const {
  wchar_t buffer[8];
  DWORD bytes;

  bytes = GetPrivateProfileStringW(section, key,
    (defaultvalue)?L"1":L"0", buffer, sizeof(buffer), _filename.c_str());

  if (errno == 0x2) { // File not found
    handleWindowsError(L"Cannot read ini file", GetLastError());
    return defaultvalue;
  }

  if (bytes != 1)
    return defaultvalue;

  if (buffer[0] == '1')
    return true;
  else if (buffer[0] == '0')
    return false;
  else
    return defaultvalue;
}

std::vector<std::wstring>
IniFile::getSections() const {
  wchar_t* buffer;
  DWORD buffersize = 1024;
  DWORD bytes;
  std::vector<std::wstring> l;

  do {    
    buffer = (wchar_t*) alloca(buffersize);
    bytes = GetPrivateProfileStringW(NULL, NULL, NULL, buffer, buffersize, _filename.c_str());

    if (bytes == buffersize-2) {// buffer to small
      buffersize *= 2;
      free(buffer);
      continue;
    }
  } while (false); 

  if (errno == 0x2) { // File not found
    handleWindowsError(L"Cannot read ini file", GetLastError());
    return l;
  }

  wchar_t* ptr = buffer;  
  while (*ptr != '\0') {
    l.push_back(std::wstring(ptr));
    ptr += wcslen(ptr) + 1;
  }

  return l;
}

bool
IniFile::setString(const wchar_t* section, const wchar_t* key, const wchar_t* str) const {
  BOOL e;
  
  e = WritePrivateProfileStringW(section, key, str, _filename.c_str());

  if (e == 0) {
    handleWindowsError(L"Cannot write ini file", GetLastError());
    return false;
  } else {
    return true;
  }
}
