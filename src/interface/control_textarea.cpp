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

#include "interface/control_textarea.hpp"

ControlTextarea::ControlTextarea() :
  ControlTextbox()
{
}

void
ControlTextarea::draw(DrawingContext& context)
{
  if (!m_visible)
    return;

  InterfaceControl::draw(context);

  const InterfaceTheme& theme = get_current_theme();

  context.color().draw_filled_rect(m_rect,
                                   theme.bkg_color,
                                   theme.round_corner,
                                   LAYER_GUI);

  if (m_caret_pos != m_secondary_caret_pos) {
    float lgt1 = theme.font->get_text_width(get_first_chars_visible(std::max(
                                std::min(m_caret_pos, m_secondary_caret_pos) - m_current_offset,
                                0
                                )));

    float lgt2 = theme.font->get_text_width(get_first_chars_visible(std::min(
                                 std::max(m_caret_pos, m_secondary_caret_pos) - m_current_offset,
                                 int(get_contents_visible().size())
                                 )));

    context.color().draw_filled_rect(Rectf(m_rect.p1() + Vector(lgt1 + 5.f, 0.f),
                                           m_rect.p1() + Vector(lgt2 + 5.f, m_rect.get_height())
                                           ),
                                     m_has_focus ? Color(1.f, 1.f, .9f, 0.75f)
                                                 : Color(1.f, 1.f, .9f, 0.5f),
                                     LAYER_GUI);
  }

  context.color().draw_text(theme.font,
                            get_contents_visible(),
                            Vector(m_rect.get_left() + 5.f,
                                   (m_rect.get_top() + m_rect.get_bottom()) / 2 -
                                    theme.font->get_height() / 2),
                            FontAlignment::ALIGN_LEFT,
                            LAYER_GUI + 1,
                            theme.txt_color);
  if (m_cursor_timer > 0 && m_has_focus) {
    float lgt = theme.font->get_text_width(get_first_chars_visible(m_caret_pos - m_current_offset));

    context.color().draw_line(m_rect.p1() + Vector(lgt + 5.f, 2.f),
                              m_rect.p1() + Vector(lgt + 5.f,
                                  theme.font->get_height() + 4.f),
                              theme.txt_color,
                              LAYER_GUI + 1);
  }
}

bool
ControlTextarea::on_key_down(const SDL_KeyboardEvent& key)
{
  if (!m_has_focus)
    return false;

  if (key.keysym.sym == SDLK_RETURN)
  {
    put_text("\n");
  }

  return ControlTextbox::on_key_down(key);
}

/* EOF */
