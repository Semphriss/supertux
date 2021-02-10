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

#ifndef HEADER_SUPERTUX_INTERFACE_CONTROL_SCROLLBAR_HPP
#define HEADER_SUPERTUX_INTERFACE_CONTROL_SCROLLBAR_HPP

#include "interface/control.hpp"
#include "math/rect.hpp"
#include "math/vector.hpp"

class DrawingContext;

/** A generic template for a scrollbar */
class ControlScrollbar final : public InterfaceControl
{
public:
  ControlScrollbar();

  virtual void draw(DrawingContext& context) override;

  virtual bool on_mouse_motion(const SDL_MouseMotionEvent& motion) override;
  virtual bool on_mouse_button_up(const SDL_MouseButtonEvent& button) override;
  virtual bool on_mouse_button_down(const SDL_MouseButtonEvent& button) override;

  /** @returns true if the current bar is valid and will show and interact */
  bool is_valid();

  float get_value() { return m_progress + m_offset; }
  void bind_value(float* value) { m_value = value; }

public:
  // -------------------------------------------------------------------------
  // Scroll bar:           |     ==========                     |
  // -------------------------------------------------------------------------
  //
  //  m_total_region:      \____________________________________/
  //  m_covered_region:          \________/
  //  m_progress:          \____/

  /** The length (height) of the region to scroll */
  float m_total_region;

  /** The length (height) of the viewport for the region */
  float m_covered_region;

  /** The length (height) between the beginning of the viewport and the beginning of the region */
  float m_progress;

  /** Offset which will be added to m_progress to determine the value */
  float m_offset;

  /** `true` of the scroller is horizontal; `false` if it is vertical */
  bool m_horizontal;

  /** Same as m_on_change, but passes the delta */
  std::function<void(float)> m_on_scroll;

private:
  Rectf get_bar_rect();

  Vector last_mouse_pos;

  /** Can't use a m_mouse_clicking, so use this to override */
  bool m_mouse_dragging;

  /** The value of the progress bar. Differs from m_progress by the fact that it considers the offset. */
  float* m_value;

private:
  ControlScrollbar(const ControlScrollbar&) = delete;
  ControlScrollbar& operator=(const ControlScrollbar&) = delete;
};

#endif

/* EOF */
