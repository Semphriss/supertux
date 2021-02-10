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

#include "editor/object_widget.hpp"

#include "editor/object_info.hpp"
#include "supertux/globals.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

EditorObjectWidget::EditorObjectWidget(Editor& editor) :
  m_editor(editor),
  m_scrollbar(),
  m_left(192),
  m_bottom(static_cast<float>(SCREEN_HEIGHT / 2)),
  m_rect(Rect(SCREEN_WIDTH - static_cast<int>(m_left), 0,
              SCREEN_WIDTH, static_cast<int>(m_bottom))),
  m_object_info(new ObjectInfo()),
  m_tiles(),
  m_object(),
  m_input_type()
{
  m_scrollbar.set_rect(Rect(SCREEN_WIDTH - 5, SCREEN_HEIGHT / 2,
                            SCREEN_WIDTH, SCREEN_HEIGHT));
  m_scrollbar.m_horizontal = false;
  m_scrollbar.m_theme = InterfaceThemeSet(
    InterfaceTheme(Resources::control_font, Color(.7f, .7f, .7f, 1.f), Color::BLACK, 0.f), // base
    InterfaceTheme(Resources::control_font, Color(.8f, .8f, .8f, 1.f), Color::BLACK, 0.f), // hover
    InterfaceTheme(Resources::control_font, Color(1.f, 1.f, 1.f, 1.f), Color::BLACK, 0.f), // active
    InterfaceTheme(Resources::control_font, Color(.9f, .9f, .9f, 1.f), Color::BLACK, 0.f), // focused
    InterfaceTheme(Resources::control_font, Color(.3f, .3f, .3f, 1.f), Color::BLACK, 0.f) // disabled
  );
  m_scrollbar.m_on_change = [this] { reset_components(); };

  resize();
}

void
EditorObjectWidget::draw(DrawingContext& context)
{
  context.color().draw_filled_rect(m_rect, Color(.15f, .15f, .15f), LAYER_GUI - 3);

  context.push_transform();
  context.transform().clip = Rect(m_rect);

  InterfaceContainer::draw(context);

  m_scrollbar.draw(context);

  context.pop_transform();
}

void
EditorObjectWidget::update(float dt_sec)
{
}

void
EditorObjectWidget::resize()
{
  m_rect = Rect(SCREEN_WIDTH - static_cast<int>(m_left), 0,
                SCREEN_WIDTH, static_cast<int>(m_bottom));
  m_scrollbar.set_rect(Rect(SCREEN_WIDTH - 5, 0,
                            SCREEN_WIDTH, static_cast<int>(m_bottom)));
  m_scrollbar.m_covered_region = m_rect.get_height();

  reset_components();
}

bool
EditorObjectWidget::on_mouse_wheel(const SDL_MouseWheelEvent& wheel)
{
  if (!m_rect.contains(m_mouse_pos))
    return false;

  m_scrollbar.m_progress = math::clamp(m_scrollbar.m_progress - static_cast<float>(wheel.y * 30),
                                       0.f,
                                       m_scrollbar.m_total_region - m_scrollbar.m_covered_region);

  reset_components();

  return true;
}

void
EditorObjectWidget::reset_components()
{
  m_children.clear();

  m_scrollbar.m_total_region = 1000.f;
  m_scrollbar.m_covered_region = m_rect.get_height();
}

/* EOF */
