//  SuperTux
//  Copyright (C) 2021 A. Semphris <semphris@protonmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_SUPERTUX_INTERFACE_CONTROL_TEXTAREA_HPP
#define HEADER_SUPERTUX_INTERFACE_CONTROL_TEXTAREA_HPP

#include "interface/control_textbox.hpp"

class ControlTextarea : public ControlTextbox
{
public:
  ControlTextarea();

  virtual void draw(DrawingContext& context) override;
  virtual bool on_key_down(const SDL_KeyboardEvent& key) override;

private:
  ControlTextarea(const ControlTextarea&) = delete;
  ControlTextarea& operator=(const ControlTextarea&) = delete;
};

#endif

/* EOF */
