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

#ifndef __PROPERTIES_H__
#define __PROPERTIES_H__

#include "stdafx.h"
#include <lightports/base/configfile.h>

class Properties {
public:
  Properties();
  ~Properties();

  void SetStartup(bool startup);
  bool GetStartup() const { return _startup; };

  static const bool AutomaticDefault = true;
  void SetAutomatic(bool value);
  bool GetAutomatic() const { return _automatic; };

private:
  Windows::ConfigFile _configfile;

  static const wchar_t * const Filename;
  
  bool _startup;
  bool _automatic;
};

#endif // __PROPERTIES_H__

