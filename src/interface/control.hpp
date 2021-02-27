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

#ifndef HEADER_SUPERTUX_INTERFACE_CONTROL_HPP
#define HEADER_SUPERTUX_INTERFACE_CONTROL_HPP

#include <SDL.h>

#include "control/input_manager.hpp"
#include "editor/widget.hpp"
#include "interface/theme.hpp"
#include "video/drawing_context.hpp"

class InterfaceContainer;
class InterfaceLabel;

// List of characters that must be rendered but that can break the line after
// them. The space character is included.
// Please only put characters which are commonly *not* followed by a space, as
// a space will make the preceeding character breakable anyways.
const std::string breakable_chars = "- ";

class InterfaceControl : public Widget
{
public:
  InterfaceControl();
  InterfaceControl(InterfaceThemeSet theme);
  virtual ~InterfaceControl();

  virtual bool event(const SDL_Event& ev) override { if (!m_enabled) return false; return Widget::event(ev); }
  virtual void draw(DrawingContext& context) override;
  virtual bool on_mouse_button_up(const SDL_MouseButtonEvent& button) override { m_mouse_down = false; return false; }
  virtual bool on_mouse_button_down(const SDL_MouseButtonEvent& button) override { m_mouse_down = true; return false; }
  virtual bool on_mouse_motion(const SDL_MouseMotionEvent& motion) override;

  /** Sets this control's focus status. */
  virtual std::vector<std::function<void()>> set_focus(bool focus, std::vector<std::function<void()>> callbacks = {});
  virtual bool has_focus() const { return m_has_focus; }

  virtual void draw_tooltip(DrawingContext& context, const std::string& text) const;

  virtual void set_rect(const Rectf& rect) { m_rect = rect; }
  Rectf& get_rect() { return m_rect; }

  const InterfaceTheme& get_current_theme() const;

public:
  /**
   * Optional; a function that will be called each time the bound value
   * is modified.
   */
  std::function<void()> m_on_change;

  /**
   * Optional; a function that will be called when the control gains or loses
   * user focus. Argument is true when the control has gained the focus.
   * 
   * FIXME: AVOID USING - There is no guarantee as to when exactly the function
   *        will be called. Some controls might have had their focus updated and
   *        some others might have not. Do not read/write focus from this.
   */
  std::function<void()> m_on_focus;

  /** Optional; the label associated with the control */
  InterfaceLabel* m_label;

  /** A pointer to the parent container, or null if not in any container. */
  InterfaceContainer* m_parent;

  /** Whether this control is interactible. */
  bool m_enabled;

  /** Whether this control is visible on the screen. */
  bool m_visible;

  InterfaceThemeSet m_theme;

protected:
  /** Whether or not the user has this InterfaceControl as focused. */
  bool m_has_focus;
  /** Whether or not the mouse hovers this component. */
  bool m_mouse_hover;
  /** Whether or not the mouse is left-clicking. */
  bool m_mouse_down;
  /** The position of the mouse in the window */
  Vector m_mouse_pos;
  /** The rectangle where the InterfaceControl should be rendered. */
  Rectf m_rect;

private:
  InterfaceControl(const InterfaceControl&) = delete;
  InterfaceControl& operator=(const InterfaceControl&) = delete;
};

#endif

/* EOF */
