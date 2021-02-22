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

#ifndef HEADER_SUPERTUX_EDITOR_SETTINGS_MESSAGE_HPP
#define HEADER_SUPERTUX_EDITOR_SETTINGS_MESSAGE_HPP

#include "interface/label.hpp"
#include "supertux/game_object.hpp"

class SettingsMessage : public InterfaceLabel
{
public:
  SettingsMessage(const Rectf& rect, const std::string& label, const GameObject::ValidationLevel& type) :
    InterfaceLabel(rect, label),
    m_type(type)
  {}
  virtual ~SettingsMessage() {}

  virtual void draw(DrawingContext& context) override;

private:
  Color get_color() const;

private:
  GameObject::ValidationLevel m_type;

private:
  SettingsMessage(const SettingsMessage&) = delete;
  SettingsMessage& operator=(const SettingsMessage&) = delete;
};

#endif

/* EOF */
