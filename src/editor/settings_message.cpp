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

#include "editor/settings_message.hpp"

#include "supertux/resources.hpp"
#include "video/surface.hpp"

#include <iostream>

SettingsMessage::SettingsMessage(const Rectf& rect, const std::string& label, const GameObject::ValidationLevel& type) :
  InterfaceLabel(rect, label),
  m_type(type),
  m_icon(Surface::from_file(get_icon()))
{
  m_multiline = true;
  m_theme = InterfaceThemeSet(
    InterfaceTheme(Resources::control_font, Color::WHITE, Color::INVISIBLE, 0.f),
    InterfaceTheme(Resources::control_font, Color::WHITE, Color::INVISIBLE, 0.f),
    InterfaceTheme(Resources::control_font, Color::WHITE, Color::INVISIBLE, 0.f),
    InterfaceTheme(Resources::control_font, Color::WHITE, Color::INVISIBLE, 0.f),
    InterfaceTheme(Resources::control_font, Color::WHITE, Color::INVISIBLE, 0.f)
  );
  m_v_align = VAlign::TOP;
  m_h_align = FontAlignment::ALIGN_LEFT;
}

void
SettingsMessage::draw(DrawingContext& context)
{
  context.color().draw_filled_rect(m_rect.grown(2.f),
                                   get_color(),
                                   5.f,
                                   LAYER_GUI);
  context.color().draw_filled_rect(m_rect,
                                   Color(0.f, 0.f, 0.f, .5f),
                                   3.f,
                                   LAYER_GUI);

  context.push_transform();
  context.transform().clip = Rect(m_rect).intersect(context.transform().clip);
  context.color().draw_surface(m_icon, m_rect.p1() - Vector(32, 32), LAYER_GUI);
  context.pop_transform();

  InterfaceLabel::draw(context);
}

Color
SettingsMessage::get_color() const
{
  switch (m_type)
  {
    case GameObject::ValidationLevel::NOTICE:
      return Color(.7f, .7f, .7f);
    case GameObject::ValidationLevel::WARNING:
      return Color(.7f, .7f, .2f);
    case GameObject::ValidationLevel::ERROR:
      return Color(.7f, .2f, .2f);
  }

  // problem
  assert(false);
}

std::string
SettingsMessage::get_icon() const
{
  switch (m_type)
  {
    case GameObject::ValidationLevel::NOTICE:
      return "images/engine/editor/info.png";
    case GameObject::ValidationLevel::WARNING:
      return "images/engine/editor/warn.png";
    case GameObject::ValidationLevel::ERROR:
      return "images/engine/editor/error.png";
  }

  // problem
  assert(false);
}


/* EOF */
