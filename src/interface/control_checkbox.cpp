//  SuperTux
//  Copyright (C) 2020 A. Semphris <semphris@protonmail.com>
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

#include "interface/control_checkbox.hpp"

#include "math/vector.hpp"
#include "supertux/resources.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

ControlCheckbox::ControlCheckbox() :
  InterfaceControl(InterfaceThemeSet(
    InterfaceTheme(Resources::control_font, Color::BLACK, Color(.5f, .5f, .5f), 0.f), // base
    InterfaceTheme(Resources::control_font, Color::BLACK, Color(.6f, .6f, .6f), 0.f), // hover
    InterfaceTheme(Resources::control_font, Color::BLACK, Color(.75f, .75f, .75f), 0.f), // active
    InterfaceTheme(Resources::control_font, Color::BLACK, Color(.75f, .75f, .75f), 0.f), // focused
    InterfaceTheme(Resources::control_font, Color(.2f, .2f, .2f), Color(.6f, .6f, .6f), 0.f) // disabled
  )),
  m_value()
{
}

void
ControlCheckbox::draw(DrawingContext& context)
{
  if (!m_visible)
    return;

  InterfaceControl::draw(context);

  const InterfaceTheme& theme = get_current_theme();

  context.color().draw_filled_rect(m_rect,
                                   theme.bkg_color,
                                   theme.round_corner,
                                   LAYER_GUI);
  if (*m_value) {
    context.color().draw_text(theme.font,
                              "X", 
                              Vector((m_rect.get_left() + m_rect.get_right()) / 2 + 1.f,
                                     (m_rect.get_top() + m_rect.get_bottom()) / 2 - theme.font->get_height() / 2),
                              FontAlignment::ALIGN_CENTER,
                              LAYER_GUI,
                              theme.txt_color);
  }
}

bool
ControlCheckbox::on_mouse_button_up(const SDL_MouseButtonEvent& button)
{
  InterfaceControl::on_mouse_button_up(button);

  if (button.button != SDL_BUTTON_LEFT)
    return false;

  Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(button.x, button.y);

  if (!m_rect.contains(mouse_pos))
    return false;

  *m_value = !*m_value;

  if (m_on_change)
    m_on_change();

  set_focus(true);

  return true;
}

bool
ControlCheckbox::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{
  InterfaceControl::on_mouse_button_down(button);

  Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(button.x, button.y);
  if (!m_rect.contains(mouse_pos)) {
    set_focus(false);
  }
  return false;
}

bool
ControlCheckbox::on_key_up(const SDL_KeyboardEvent& key)
{
  if (key.keysym.sym != SDLK_SPACE || !m_has_focus)
    return false;

  *m_value = !*m_value;

  if (m_on_change)
    m_on_change();

  return true;
}

