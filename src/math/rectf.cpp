//  SuperTux
//  Copyright (C) 2016 Ingo Ruhnke <grumbel@gmail.com>
//                2021 A. Semphris <semphris@protonmail.com>
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

#include "math/rectf.hpp"

#include <ostream>

#include "math/rect.hpp"

Rectf::Rectf(const Rect& rect) :
  m_p1(static_cast<float>(rect.left),
     static_cast<float>(rect.top)),
  m_size(static_cast<float>(rect.get_width()),
         static_cast<float>(rect.get_height()))
{
}

std::ostream& operator<<(std::ostream& out, const Rectf& rect)
{
  out << "Rectf("
      << rect.get_left() << ", " << rect.get_top() << ", "
      << rect.get_right() << ", " << rect.get_bottom()
      << ")";
  return out;
}

std::tuple<Vector, Vector>
Rectf::clip_line(const Vector& p1, const Vector& p2)
{
  if (contains(p1) && contains(p2))
    return {p1, p2};
  
  if (p1.x == p2.x)
  {
    if (!math::is_in_range(p1.x, get_left(), get_right()))
      return {Vector(), Vector()};

    return {
      Vector(p1.x, math::clamp(p1.y, get_top(), get_bottom())),
      Vector(p2.x, math::clamp(p2.y, get_top(), get_bottom()))
    };
  }
  else if (p1.y == p2.y)
  {
    if (!math::is_in_range(p1.y, get_top(), get_bottom()))
      return {Vector(), Vector()};

    return {
      Vector(math::clamp(p1.x, get_left(), get_right()), p1.y),
      Vector(math::clamp(p2.x, get_left(), get_right()), p2.y)
    };
  }
  else
  {
    float a = (p2.y - p1.y) / (p2.x - p1.x);
    float b = p1.y - a * p1.x;

    Vector r1 = p1, r2 = p2;
    if (math::is_in_range(a * get_left() + b, get_top(), get_bottom()))
    {
      if (p1.x < get_left() && p2.x > get_left())
        r1 = Vector(get_left(), a * get_left() + b);
      if (p2.x < get_left() && p1.x > get_left())
        r2 = Vector(get_left(), a * get_left() + b);
    }

    if (math::is_in_range(a * get_right() + b, get_top(), get_bottom()))
    {
      if (p1.x < get_right() && p2.x > get_right())
        r2 = Vector(get_right(), a * get_right() + b);
      if (p2.x < get_right() && p1.x > get_right())
        r1 = Vector(get_right(), a * get_right() + b);
    }

    if (math::is_in_range((get_top() - b) / a, get_left(), get_right()))
    {
      if (p1.y < get_top() && p2.y > get_top())
        r1 = Vector((get_top() - b) / a, get_top());
      if (p2.y < get_top() && p1.y > get_top())
        r2 = Vector((get_top() - b) / a, get_top());
    }

    if (math::is_in_range((get_bottom() - b) / a, get_left(), get_right()))
    {
      if (p1.y < get_bottom() && p2.y > get_bottom())
        r2 = Vector((get_bottom() - b) / a, get_bottom());
      if (p2.y < get_bottom() && p1.y > get_bottom())
        r1 = Vector((get_bottom() - b) / a, get_bottom());
    }

    if (r1 == p1 && r2 == p2)
      return {Vector(), Vector()};
    
    return {r1, r2};
  }
  
}
/* EOF */
