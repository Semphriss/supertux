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

#include "interface/control_button.hpp"

#include "math/vector.hpp"
#include "supertux/resources.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

ControlButton::ControlButton(std::string label) :
  InterfaceControl(InterfaceThemeSet(
    InterfaceTheme(Resources::control_font, Color::BLACK, Color(.5f, .5f, .5f), 0.f), // base
    InterfaceTheme(Resources::control_font, Color::BLACK, Color(.6f, .6f, .6f), 0.f), // hover
    InterfaceTheme(Resources::control_font, Color::BLACK, Color(.3f, .3f, .3f), 0.f), // active
    InterfaceTheme(Resources::control_font, Color::BLACK, Color(.75f, .75f, .75f), 0.f), // focused
    InterfaceTheme(Resources::control_font, Color(.2f, .2f, .2f), Color(.6f, .6f, .6f), 0.f) // disabled
  )),
  m_btn_label(std::move(label)),
  m_action_on_mousedown()
{
}

void
ControlButton::draw(DrawingContext& context)
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
                            Vector((m_rect.get_left() + m_rect.get_right()) / 2,
                                   (m_rect.get_top() + m_rect.get_bottom()) / 2 - theme.font->get_height() / 2),
                            FontAlignment::ALIGN_CENTER,
                            LAYER_GUI,
                            theme.txt_color);
}

bool
ControlButton::on_mouse_button_up(const SDL_MouseButtonEvent& button)
{
  InterfaceControl::on_mouse_button_up(button);

  if (button.button != SDL_BUTTON_LEFT)
    return false;

  if (!m_mouse_hover)
    return false;

  if (m_on_change && !m_action_on_mousedown)
    m_on_change();

  return true;
}

bool
ControlButton::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{
  InterfaceControl::on_mouse_button_down(button);

  // TODO: Clean this code and use m_mouse_hover or something
  if (button.button == SDL_BUTTON_LEFT) {
    Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(button.x, button.y);
    if (!m_rect.contains(mouse_pos)) {
      set_focus(false);
    } else {
      set_focus(true);
      m_mouse_down = true;
      if (m_on_change && m_action_on_mousedown)
        m_on_change();
      return true;
    }
  }
  return false;
}

bool
ControlButton::on_key_up(const SDL_KeyboardEvent& key)
{
  if (!m_has_focus)
    return false;

  if (key.keysym.sym == SDLK_SPACE) {
    if (m_on_change)
      m_on_change();
    m_mouse_down = false;
    return true;
  }

  return false;
}

bool
ControlButton::on_key_down(const SDL_KeyboardEvent& key)
{
  if (!m_has_focus)
    return false;

  if (key.keysym.sym == SDLK_SPACE) {
    m_mouse_down = true;
    return true;
  }

  return false;
}

/* EOF */
