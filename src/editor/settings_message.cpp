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

#include <iostream>

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
  context.color().draw_text(Resources::control_font,
                            get_truncated_text(),
                            Vector(m_rect.get_left() + 5.f,
                                   (m_rect.get_top() + m_rect.get_bottom()) / 2 -
                                    Resources::control_font->get_height() / 2 + 1.f),
                            FontAlignment::ALIGN_LEFT,
                            LAYER_GUI,
                            Color::WHITE);
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


/* EOF */
