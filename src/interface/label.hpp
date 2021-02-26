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

#ifndef HEADER_SUPERTUX_INTERFACE_LABEL_HPP
#define HEADER_SUPERTUX_INTERFACE_LABEL_HPP

#include <SDL.h>

#include "interface/control.hpp"
#include "video/drawing_context.hpp"

class InterfaceLabel : public InterfaceControl
{
public:
  enum class VAlign {
    TOP,
    MIDDLE,
    BOTTOM
  };

public:
  InterfaceLabel();
  InterfaceLabel(const Rectf& rect, const std::string& label);
  virtual ~InterfaceLabel() {}

  virtual void draw(DrawingContext& context) override;

  void set_label(const std::string& label) { m_label = label; }
  std::string get_label() const { return m_label; }

  /**
   * Checks whether a single line fits.
   * @see fits_multiline()
   */
  bool fits(const std::string& text) const;

  /**
   * Checks whether a text would fit, spanning multiple lines. Unused internally.
   * @see fits()
   */
  bool fits_multiline(const std::string& text) const;

  /**
   * @returns the largest single line fitting in the box. Adds ellipsis if necessary.
   * @see get_lines()
   */
  std::string get_truncated_text() const;

  /**
   * @returns the text splitted in lines that fit m_rect. Adds ellipsis if necessary.
   * @see get_truncated_text()
   */
  std::vector<std::string> get_lines() const;

  /**
   * @returns the maximum amount of lines fitting in the box. The number can
   *          vary during runtime, based on whether the theme uses multiple
   *          fonts with different sizes.
   */
  int get_num_lines() const;

  /** Sets m_rect.bottom so that the text fits (multiline only) */
  void autoresize_bottom();

private:
  /**
   * @returns the remaining portion of the height not covered by text,
   *          if X @p lines are used.
   */
  float get_remainder_height(int lines) const;

  /**
   * @returns the remaining portion of the height if all possible lines were
   *          used.
   */
  float get_remainder_height_total() const;

  /** @returns the biggest subset of @p s so that @p f (subset) = @c true. */
  std::string find_fitting(std::string s, std::function<bool(std::string)> f) const;
  /** word-based version of @see find_fitting() */
  std::string find_fitting_word(std::string s, std::function<bool(std::string)> f) const;

public:
  bool m_multiline;
  VAlign m_v_align;
  FontAlignment m_h_align;
  float m_padding;

protected:
  /** The text of the label */
  std::string m_label;

private:
  Vector m_mouse_pos;

private:
  InterfaceLabel(const InterfaceLabel&) = delete;
  InterfaceLabel& operator=(const InterfaceLabel&) = delete;
};

#endif

/* EOF */
