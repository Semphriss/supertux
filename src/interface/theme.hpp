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

#ifndef HEADER_SUPERTUX_INTERFACE_THEME_HPP
#define HEADER_SUPERTUX_INTERFACE_THEME_HPP

#include "supertux/resources.hpp"
#include "video/color.hpp"
#include "video/font_ptr.hpp"

/** Holds rendering settings for elements */
class InterfaceTheme final
{
public:
  InterfaceTheme() :
    // Initializing the font is needed to avoid segfaults if someone forgets to
    // assign a theme to a control
    font(Resources::control_font),
    txt_color(),
    bkg_color(),
    round_corner()
  {
  }

  InterfaceTheme(FontPtr font_, Color txt_color_, Color bkg_color_,
                 float round_corner_) :
    font(font_),
    txt_color(txt_color_),
    bkg_color(bkg_color_),
    round_corner(round_corner_)
  {
  }

public:
  FontPtr font;
  Color txt_color;
  Color bkg_color;
  float round_corner;
};

class InterfaceThemeSet final
{
public:
  InterfaceThemeSet() :
    base(), hover(), active(), focused(), disabled()
  {
  }

  InterfaceThemeSet(InterfaceTheme base_, InterfaceTheme hover_,
                    InterfaceTheme active_, InterfaceTheme focused_,
                    InterfaceTheme disabled_) :
    base(base_),
    hover(hover_),
    active(active_),
    focused(focused_),
    disabled(disabled_)
  {
  }

public:
  /** The theme to use when the control is not focused. */
  InterfaceTheme base;
  /** The theme to use when the control is hovered by the mouse. */
  InterfaceTheme hover;
  /** The theme to use when the control is currently undergoing an action. */
  InterfaceTheme active;
  /** The theme to use when the control has the focus. */
  InterfaceTheme focused;
  /** The theme to use when the control is active but not focused. */
  InterfaceTheme disabled;
};

#endif

/* EOF */
