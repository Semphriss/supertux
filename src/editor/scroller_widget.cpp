//  SuperTux
//  Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
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

#include "editor/scroller_widget.hpp"

#include <math.h>

#include "editor/editor.hpp"
#include "video/drawing_context.hpp"
#include "video/renderer.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

namespace {

const float SIZE = 96;

}

bool EditorScrollerWidget::rendered = false;

EditorScrollerWidget::EditorScrollerWidget(Editor& editor) :
  m_editor(editor),
  m_scrolling(),
  m_scrolling_vec(0, 0),
  m_mouse_pos(0, 0),
  m_rect(8, 40, 8 + SIZE, 40 + SIZE)
{
}

bool
EditorScrollerWidget::is_scrolling() const
{
  return m_scrolling && m_rect.contains(m_mouse_pos);
}

void
EditorScrollerWidget::draw(DrawingContext& context)
{
  if (!rendered) return;

  context.color().draw_filled_rect(m_rect,
                                   Color(.15f, .15f, .15f, .8f),
                                   (m_rect.p2() - m_rect.p1()).norm(),
                                   LAYER_GUI - 21);
  context.color().draw_filled_rect(Rectf(m_rect.get_middle(),
                                         m_rect.get_middle()).grown(8.f),
                                   Color(0.f, 0.f, 0.f, .5f),
                                   8,
                                   LAYER_GUI - 20);
  if (is_scrolling())
    draw_arrow(context, m_mouse_pos);

  draw_arrow(context, m_rect.get_middle() - Vector(0, m_rect.get_height() / 3.f));
  draw_arrow(context, m_rect.get_middle() + Vector(0, m_rect.get_height() / 3.f));
  draw_arrow(context, m_rect.get_middle() - Vector(m_rect.get_width() / 3.f, 0));
  draw_arrow(context, m_rect.get_middle() + Vector(m_rect.get_width() / 3.f, 0));
}

void
EditorScrollerWidget::draw_arrow(DrawingContext& context, const Vector& pos)
{
  Vector dir = pos - m_rect.get_middle();
  if (dir.x != 0 || dir.y != 0) {
    // draw a triangle
    dir = dir.unit() * 8;
    Vector dir2 = Vector(-dir.y, dir.x);
    context.color().draw_triangle(pos + dir, pos - dir + dir2, pos - dir - dir2,
                                  Color(0.f, 0.f, 0.f, .5f), LAYER_GUI - 20);
  }
}

void
EditorScrollerWidget::update(float dt_sec)
{
  if (!rendered || !is_scrolling())
    return;

  m_editor.scroll(m_scrolling_vec * 32.0f * dt_sec);
}

bool
EditorScrollerWidget::on_mouse_button_up(const SDL_MouseButtonEvent& button)
{
  m_scrolling = false;
  return false;
}

bool
EditorScrollerWidget::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{
  if (!rendered || !m_rect.contains(m_mouse_pos))
    return false;

  if (button.button == SDL_BUTTON_LEFT) {
    m_scrolling = true;
    return true;
  }

  return false;
}

bool
EditorScrollerWidget::on_mouse_motion(const SDL_MouseMotionEvent& motion)
{
  if (!rendered)
    return false;

  m_mouse_pos = VideoSystem::current()->get_viewport().to_logical(motion.x, motion.y);
  if (m_rect.contains(m_mouse_pos)) {
    m_scrolling_vec = m_mouse_pos - m_rect.get_middle();
    if (m_scrolling_vec.x != 0 || m_scrolling_vec.y != 0) {
      float norm = m_scrolling_vec.norm();
      m_scrolling_vec *= powf(static_cast<float>(M_E), norm / 16.0f - 1.0f);
    }
  }

  return false;
}

bool
EditorScrollerWidget::on_key_down(const SDL_KeyboardEvent& key)
{
  if (key.keysym.sym == SDLK_F9)
    rendered = !rendered;

  return false;
}

/* EOF */
