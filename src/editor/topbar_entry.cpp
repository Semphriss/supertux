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

#include "editor/topbar_entry.hpp"

#include "sprite/sprite_manager.hpp"

TopbarEntry::TopbarEntry(std::string label, std::string icon) :
  ControlButton(label),
  m_btn_icon(icon != "" ? SpriteManager::current()->create(icon) : nullptr),
  separator()
{
}

void
TopbarEntry::draw(DrawingContext& context)
{
  if (!m_visible)
    return;

  InterfaceControl::draw(context);

  const InterfaceTheme& theme = get_current_theme();

  context.color().draw_filled_rect(m_rect,
                                   theme.bkg_color,
                                   theme.round_corner,
                                   LAYER_GUI);

  context.color().draw_text(theme.font,
                            m_btn_label,
                            Vector(m_rect.get_left() + m_rect.get_height() + 5.f,
                                   (m_rect.get_top() + m_rect.get_bottom()) / 2 - theme.font->get_height() / 2),
                            FontAlignment::ALIGN_LEFT,
                            LAYER_GUI,
                            theme.txt_color);

  context.color().draw_line(Vector(m_rect.get_left() + m_rect.get_height(),
                                   m_rect.get_top()),
                            Vector(m_rect.get_left() + m_rect.get_height(),
                                   m_rect.get_bottom()),
                            Color(1.f,1.f,1.f,.3f),
                            LAYER_GUI);

  if (separator)
  {
    context.color().draw_line(m_rect.p1(),
                              Vector(m_rect.p2().x, m_rect.p1().y),
                              Color(1.f, 1.f, 1.f, .1f),
                              LAYER_GUI);
    context.color().draw_line(m_rect.p1() - Vector(0.f, 1.f),
                              Vector(m_rect.p2().x, m_rect.p1().y - 1.f),
                              Color(1.f, 1.f, 1.f, .1f),
                              LAYER_GUI);
  }

  if (m_btn_icon)
    context.color().draw_surface_scaled(m_btn_icon->get_current_surface(),
                                        Rectf(m_rect.get_left() + 4.f,
                                              m_rect.get_top() + 4.f,
                                              m_rect.get_left() +
                                                  m_rect.get_height() - 4.f,
                                              m_rect.get_bottom() - 4.f),
                                        LAYER_GUI);
}

/* EOF */
