/*
This file is part of MPDControl.

MPDControl is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Foobar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once
constexpr uint8_t modesWidth = 8;
constexpr uint8_t modesHeight = 8;

const uint8_t PROGMEM modes[] =
{
  modesWidth, modesHeight,
  //Frame 0
  0x00, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x00, 

  //Frame 1
  0x00, 0x7E, 0x7E, 0x3C, 0x3C, 0x18, 0x18, 0x00, 

  //Frame 2
  0x00, 0x7E, 0x7E, 0x00, 0x00, 0x7E, 0x7E, 0x00
};
