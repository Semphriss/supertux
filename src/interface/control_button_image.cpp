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

#include "interface/control_button_image.hpp"

#include "sprite/sprite_manager.hpp"
#include "sprite/sprite.hpp"

ControlButtonImage::ControlButtonImage(std::string filename, std::string label) :
  ControlButton(label),
  m_sprite(SpriteManager::current()->create(filename))
{
}

ControlButtonImage::ControlButtonImage(SpritePtr sprite, std::string label) :
  ControlButton(label),
  m_sprite(std::move(sprite))
{
}

void
ControlButtonImage::draw(DrawingContext& context)
{
  if (!m_visible)
    return;

  InterfaceControl::draw(context);

  const InterfaceTheme& theme = get_current_theme();

  context.color().draw_filled_rect(m_rect,
                                   theme.bkg_color,
                                   theme.round_corner,
                                   LAYER_GUI);

  float w = static_cast<float>(m_sprite->get_width());
  float h = static_cast<float>(m_sprite->get_height());
  float x = m_rect.p1().x, y = m_rect.p1().y;

  if (w / h < m_rect.get_width() / m_rect.get_height())
  {
    // Height is our constaint
    float factor = h / m_rect.get_height();
    w /= factor;
    h /= factor;
    x = m_rect.get_middle().x - w / 2.f;
  }
  else
  {
    // Width is our constaint
    float factor = w / m_rect.get_width();
    w /= factor;
    h /= factor;
    y = m_rect.get_middle().y - h / 2.f;
  }

  m_sprite->draw(context.color(), Rectf(x, y, x + w, y + h), LAYER_GUI + 1);

  if (m_mouse_hover && !m_btn_label.empty())
    draw_tooltip(context, m_btn_label);
}

/* EOF */
