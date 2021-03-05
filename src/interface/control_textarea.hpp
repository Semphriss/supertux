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

#ifndef HEADER_SUPERTUX_INTERFACE_CONTROL_TEXTAREA_HPP
#define HEADER_SUPERTUX_INTERFACE_CONTROL_TEXTAREA_HPP

#include "interface/control_scrollbar.hpp"
#include "interface/control_textbox.hpp"

class ControlTextarea : public ControlTextbox
{
public:
  ControlTextarea();

  virtual bool event(const SDL_Event& ev) override;
  virtual void draw(DrawingContext& context) override;
  virtual bool on_key_down(const SDL_KeyboardEvent& key) override;
  virtual bool on_mouse_wheel(const SDL_MouseWheelEvent& wheel) override;

  virtual void set_rect(const Rectf& rect) override;

  virtual int get_text_position(const Vector& pos) const override;
  virtual void draw_text(DrawingContext& context);
  virtual void recenter_offset() override;

protected:
  void reset_scrollbar();

  /** Get the @p line th line (without any line ending "\\n") */
  std::string get_line(int line) const;

  /** Get the line number of this position in the text */
  int get_line_num(int pos) const;

  /** Get the position in the text of this line */
  int get_pos(int line) const;

  /** Get the distance between this position and its closest preceeding line end */
  int get_xpos(int pos) const;

  /** Gets the char at the given X offset in the given line. @returns offset from beginning of line. */
  int get_pos_from_x(float x, int line) const;

  /** Gets the line at the given Y offset. Maxes to existing lines. @returns the line number. */
  int get_line_from_y(float y) const;

  /** @returns the offset from the beginning of the text corresponding to this line/xpos pair. */
  int get_offset_from_pos(int line, int xpos) const;

protected:
  ControlScrollbar m_v_scrollbar, m_h_scrollbar;
  float m_right_margin;

private:
  ControlTextarea(const ControlTextarea&) = delete;
  ControlTextarea& operator=(const ControlTextarea&) = delete;
};

#endif

/* EOF */
