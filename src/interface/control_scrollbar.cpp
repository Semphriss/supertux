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

#include "interface/control_scrollbar.hpp"

#include <math.h>

#include "editor/editor.hpp"
#include "video/drawing_context.hpp"
#include "video/renderer.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

static const float SCROLLBAR_MARGIN = 0.f;

ControlScrollbar::ControlScrollbar() :
  m_total_region(),
  m_covered_region(),
  m_progress(),
  m_offset(),
  m_horizontal(),
  last_mouse_pos(),
  m_mouse_dragging(),
  m_value(nullptr)
{
}

void
ControlScrollbar::draw(DrawingContext& context)
{
  if (!is_valid())
    return;

  m_mouse_hover = m_mouse_hover || m_mouse_dragging;

  auto theme = get_current_theme();

  context.color().draw_filled_rect(m_rect, theme.bkg_color, theme.round_corner, LAYER_GUI + 5);
  context.color().draw_filled_rect(get_bar_rect(), theme.txt_color, theme.round_corner, LAYER_GUI + 5);
}

bool
ControlScrollbar::on_mouse_motion(const SDL_MouseMotionEvent& motion)
{
  if (!is_valid())
    return false;

  InterfaceControl::on_mouse_motion(motion);

  if (!m_mouse_dragging)
    return false;

  Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(motion.x, motion.y);
  float delta = m_horizontal ?
        ((mouse_pos - last_mouse_pos).x / m_rect.get_width() * m_total_region) :
        ((mouse_pos - last_mouse_pos).y / m_rect.get_height() * m_total_region);
  m_progress = math::clamp(m_progress + delta, 0.f, m_total_region - m_covered_region);
  if (m_value)
    *m_value = get_value();

  if (m_on_change)
    m_on_change();

  last_mouse_pos = mouse_pos;
  return false;
}

bool
ControlScrollbar::on_mouse_button_up(const SDL_MouseButtonEvent& button)
{
  if (!is_valid())
    return false;

  InterfaceControl::on_mouse_button_up(button);

  if (m_mouse_dragging)
    SDL_CaptureMouse(SDL_FALSE);

  m_mouse_dragging = false;

  return false;
}

bool
ControlScrollbar::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{
  if (!is_valid())
    return false;

  InterfaceControl::on_mouse_button_down(button);

  last_mouse_pos = VideoSystem::current()->get_viewport().to_logical(button.x, button.y);
  m_mouse_dragging = m_rect.contains(last_mouse_pos);

  if (m_mouse_dragging)
    if (SDL_CaptureMouse(SDL_TRUE) != 0)
      log_warning << "Cannot track mouse outside of main window" << std::endl;

  return m_mouse_dragging;
}

Rectf
ControlScrollbar::get_bar_rect()
{
  float scale = (m_horizontal ? m_rect.get_width() : m_rect.get_height()) / m_total_region;

  if (m_horizontal)
  {
    Rectf bar(m_rect.get_left() + m_progress * scale,
              m_rect.get_top(),
              m_rect.get_left() + (m_progress + m_covered_region) * scale,
              m_rect.get_bottom());

    if (bar.get_width() < 2.f * SCROLLBAR_MARGIN + 1.f)
      bar.set_right(bar.get_left() + 2.f * SCROLLBAR_MARGIN + 1.f);

    if (bar.get_height() < 2.f * SCROLLBAR_MARGIN + 1.f)
      bar.set_bottom(bar.get_top() + 2.f * SCROLLBAR_MARGIN + 1.f);

    bar = std::move(bar.grown(-SCROLLBAR_MARGIN));

    return bar;
  }
  else
  {
    Rectf bar(m_rect.get_left(),
              m_rect.get_top() + m_progress * scale,
              m_rect.get_right(),
              m_rect.get_top() + (m_progress + m_covered_region) * scale);

    if (bar.get_width() < 2.f * SCROLLBAR_MARGIN + 1.f)
      bar.set_right(bar.get_left() + 2.f * SCROLLBAR_MARGIN + 1.f);

    if (bar.get_height() < 2.f * SCROLLBAR_MARGIN + 1.f)
      bar.set_bottom(bar.get_top() + 2.f * SCROLLBAR_MARGIN + 1.f);

    bar = std::move(bar.grown(-SCROLLBAR_MARGIN));

    return bar;
  }
  
}

bool
ControlScrollbar::is_valid()
{
  return m_covered_region < m_total_region;
}

/* EOF */
