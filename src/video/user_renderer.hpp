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

#ifndef HEADER_SUPERTUX_VIDEO_USER_RENDERER_HPP
#define HEADER_SUPERTUX_VIDEO_USER_RENDERER_HPP

#include <string>

#include "video/blend.hpp"
#include "video/color.hpp"
#include "video/drawing_target.hpp"
#include "video/flip.hpp"

/** User-defined renderer for virtual canvas */
struct UserRenderer {
  /** Name of the canvas */
  std::string src_name;
  /** Source canvas type */
  DrawingTarget src_target = DrawingTarget::USER;
  /** Target canvas type */
  DrawingTarget dst_target = DrawingTarget::COLORMAP;
  /** Name of the target canvas (the canvas this canvas will be drawn onto) */
  std::string dst_name = "";
  /** Blend mode */
  Blend blend = Blend::BLEND;
  /** Flip mode */
  Flip flip = 0;
  /** Transparency */
  float alpha = 1.f;
  /** Color multiplier */
  Color color_mult = Color::WHITE;
};

#endif

/* EOF */
