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

#ifndef HEADER_SUPERTUX_EDITOR_TOPBAR_ENTRY_HPP
#define HEADER_SUPERTUX_EDITOR_TOPBAR_ENTRY_HPP

#include "interface/control_button.hpp"

#include "sprite/sprite_ptr.hpp"
#include "sprite/sprite.hpp"

class TopbarEntry : public ControlButton
{
public:
  TopbarEntry(std::string label = "", std::string icon = "");

  virtual void draw(DrawingContext& context) override;

public:
  SpritePtr m_btn_icon;
  bool separator;

private:
  TopbarEntry(const TopbarEntry&) = delete;
  TopbarEntry& operator=(const TopbarEntry&) = delete;
};

#endif

/* EOF */
