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
#include "util/fade_helper.hpp"
#include "util/gettext.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

EditorObjectWidget::EditorObjectWidget(Editor& editor) :
  m_editor(editor),
  m_scrollbar(),
  m_side_scrollbar(),
  m_left(192),
  m_bottom(static_cast<float>(SCREEN_HEIGHT / 2)),
  m_rect(Rect(SCREEN_WIDTH - static_cast<int>(m_left), 24.f,
              SCREEN_WIDTH, static_cast<int>(m_bottom))),
  m_object_info(new ObjectInfo()),
  m_tiles(new TileSelection()),
  m_object(),
  m_input_type(),
  m_tool_icon(SCREEN_WIDTH - m_left - 32.f, 32.f, SCREEN_WIDTH - m_left - 16.f, 48.f),
  m_secondary_tool_icon(SCREEN_WIDTH - m_left - 64.f, 32.f, SCREEN_WIDTH - m_left - 48.f, 48.f),
  m_tool_fade(),
  m_secondary_tool_fade()
{
  auto scrollbar_theme = InterfaceThemeSet(
    InterfaceTheme(Resources::control_font, Color(.7f, .7f, .7f, 1.f), Color::BLACK, 0.f), // base
    InterfaceTheme(Resources::control_font, Color(.8f, .8f, .8f, 1.f), Color::BLACK, 0.f), // hover
    InterfaceTheme(Resources::control_font, Color(1.f, 1.f, 1.f, 1.f), Color::BLACK, 0.f), // active
    InterfaceTheme(Resources::control_font, Color(.9f, .9f, .9f, 1.f), Color::BLACK, 0.f), // focused
    InterfaceTheme(Resources::control_font, Color(.3f, .3f, .3f, 1.f), Color::BLACK, 0.f) // disabled
  );

  m_side_scrollbar.m_horizontal = false;
  m_side_scrollbar.m_theme = scrollbar_theme;
  m_side_scrollbar.m_on_change = [this] { reset_sidebar(); };

  m_scrollbar.m_horizontal = false;
  m_scrollbar.m_theme = scrollbar_theme;
  m_scrollbar.m_on_change = [this] { reset_content(); };
}

void
EditorObjectWidget::draw(DrawingContext& context)
{
  context.color().draw_filled_rect(m_rect, Color(.15f, .15f, .15f), LAYER_GUI - 3);

  context.push_transform();
  context.transform().clip = Rect(m_rect);

  InterfaceControl::draw(context);

  m_scrollbar.draw(context);
  m_side_scrollbar.draw(context);

  context.color().draw_text(Resources::border_control_font,
                            _("Tiles"),
                            Vector(m_rect.get_left() + m_rect.get_width() / 4.f,
                                   40.f - Resources::border_control_font->get_height() / 2.f),
                            FontAlignment::ALIGN_CENTER,
                            LAYER_GUI + 2);

  context.color().draw_line(Vector(m_rect.get_left() + m_rect.get_width() / 2.f, 24.f),
                            Vector(m_rect.get_left() + m_rect.get_width() / 2.f, 56.f),
                            Color(0.f, 0.05f, 0.1f),
                            LAYER_GUI + 2);

  context.color().draw_text(Resources::border_control_font,
                            _("Objects"),
                            Vector(m_rect.get_right() - m_rect.get_width() / 4.f,
                                   40.f - Resources::border_control_font->get_height() / 2.f),
                            FontAlignment::ALIGN_CENTER,
                            LAYER_GUI + 2);

  context.pop_transform();

  context.color().draw_filled_rect(m_tool_icon, Color(.15f, .15f, .15f),
                                   m_tool_icon.get_width(), LAYER_GUI);

  context.color().draw_filled_rect(m_secondary_tool_icon, Color(.15f, .15f, .15f),
                                   m_secondary_tool_icon.get_width(), LAYER_GUI);
}

void
EditorObjectWidget::update(float dt_sec)
{
}

void
EditorObjectWidget::resize()
{
  m_rect = Rect(SCREEN_WIDTH - static_cast<int>(m_left), 24,
                SCREEN_WIDTH, static_cast<int>(m_bottom));

  m_side_scrollbar.m_covered_region = m_rect.get_height();
  m_side_scrollbar.set_rect(Rect(SCREEN_WIDTH - 5, 56,
                                 SCREEN_WIDTH, static_cast<int>(m_bottom)));

  m_scrollbar.m_covered_region = m_rect.get_height();
  m_scrollbar.set_rect(Rect(SCREEN_WIDTH - 37, 56,
                            SCREEN_WIDTH - 32, static_cast<int>(m_bottom)));

  m_tool_icon = Rectf(SCREEN_WIDTH - m_left - 32.f, 32.f, SCREEN_WIDTH - m_left - 16.f, 48.f);
  m_secondary_tool_icon = Rectf(SCREEN_WIDTH - m_left - 64.f, 32.f, SCREEN_WIDTH - m_left - 48.f, 48.f);

  reset_all();
}

void
EditorObjectWidget::setup()
{
  resize();
  m_tiles->set_tile(0);
}

bool
EditorObjectWidget::on_mouse_wheel(const SDL_MouseWheelEvent& wheel)
{
  if (!m_rect.contains(m_mouse_pos))
    return false;

  if (m_mouse_pos.x >= static_cast<float>(SCREEN_WIDTH - 32))
  {
    m_side_scrollbar.m_progress = math::clamp(m_side_scrollbar.m_progress - static_cast<float>(wheel.y * 30),
                                              0.f,
                                              m_side_scrollbar.m_total_region - m_side_scrollbar.m_covered_region);

    if (!m_side_scrollbar.is_valid())
      m_side_scrollbar.m_progress = 0.f;

    reset_sidebar();
  }
  else
  {
    m_scrollbar.m_progress = math::clamp(m_scrollbar.m_progress - static_cast<float>(wheel.y * 30),
                                        0.f,
                                        m_scrollbar.m_total_region - m_scrollbar.m_covered_region);

    if (!m_scrollbar.is_valid())
      m_scrollbar.m_progress = 0.f;

    reset_content();
  }

  return true;
}

bool
EditorObjectWidget::on_mouse_button_up(const SDL_MouseButtonEvent& button)
{
  // Use the button's x and y rather than InterfaceControl's m_mouse_pos because
  // we can't reliably assume m_mouse_pos has been updated to be the same as
  // the click location.
  Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(button.x, button.y);

  if (mouse_pos.y > 24.f && mouse_pos.y < 56.f)
  {
    if (mouse_pos.x > m_rect.get_middle().x)
    {
      set_input_type(InputType::OBJECT);
    }
    else
    {
      set_input_type(InputType::TILE);
    }
  }

  return m_rect.contains(mouse_pos);
}

bool
EditorObjectWidget::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{
  // Use the button's x and y rather than InterfaceControl's m_mouse_pos because
  // we can't reliably assume m_mouse_pos has been updated to be the same as
  // the click location.
  Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(button.x, button.y);
  return m_rect.contains(mouse_pos);
}

void
EditorObjectWidget::reset_content()
{
  m_scrollbar.m_total_region = 1000.f;
  m_scrollbar.m_covered_region = m_rect.get_height();
}

void
EditorObjectWidget::reset_sidebar()
{
  m_side_scrollbar.m_total_region = 500.f;
  m_side_scrollbar.m_covered_region = m_rect.get_height();
}

/* EOF */
