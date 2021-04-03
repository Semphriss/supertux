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

#include "editor/settings_widget.hpp"

#include "editor/editor.hpp"
#include "editor/settings_message.hpp"
#include "interface/control_textbox.hpp"
#include "interface/label.hpp"
#include "object/moving_sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/game_object.hpp"
#include "supertux/globals.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

#include <iostream>

EditorSettingsWidget::EditorSettingsWidget(Editor& editor) :
  m_reset_components_request(false),
  m_editor(editor),
  m_scrollbar(),
  m_left(192),
  m_top(static_cast<float>(SCREEN_HEIGHT / 2)),
  m_rect(Rect(SCREEN_WIDTH - static_cast<int>(m_left), static_cast<int>(m_top),
              SCREEN_WIDTH, SCREEN_HEIGHT)),
  m_object(nullptr)
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
  m_scrollbar.m_on_scroll = [this] (float delta) {
    this->move_components(-delta);
  };
}

void
EditorSettingsWidget::draw(DrawingContext& context)
{
  if (m_reset_components_request)
  {
    m_reset_components_request = false;
    reset_components();
  }

  context.color().draw_filled_rect(m_rect, Color(.15f, .15f, .15f), LAYER_GUI - 3);

  context.push_transform();
  context.transform().clip = Rect(m_rect);

  context.color().draw_text(Resources::big_font,
                            (m_object) ? m_object->get_display_name() : "No selection",
                            Vector(8, 8 - m_scrollbar.m_progress) + m_rect.p1(),
                            ALIGN_LEFT,
                            LAYER_GUI - 2,
                            (m_object) ? Color::WHITE : Color(1.f, 1.f, 1.f, .3f));

  context.color().draw_text(Resources::small_font,
                            (m_object) ? m_object->get_name() : "",
                            Vector(160, 48 - m_scrollbar.m_progress) + m_rect.p1(),
                            ALIGN_CENTER,
                            LAYER_GUI - 2);

  auto ms = dynamic_cast<MovingSprite*>(m_object);
  if (ms)
  {
    auto sprite = SpriteManager::current()->create(ms->get_sprite_name());
    context.color().draw_surface(sprite->get_current_surface(),
                                 Vector(128, 8 - m_scrollbar.m_progress)
                                                                  + m_rect.p1(),
                                 LAYER_GUI - 2);
  }

  InterfaceContainer::draw(context);

  m_scrollbar.draw(context);

  context.pop_transform();
}

void
EditorSettingsWidget::update(float dt_sec)
{
  if (m_reset_components_request)
  {
    m_reset_components_request = false;
    reset_components();
  }

  // Shouldn't be needed, but just in case
  if (m_object && !m_object->is_valid())
    set_object(nullptr);
}

void
EditorSettingsWidget::resize()
{
  m_rect = Rect(SCREEN_WIDTH - static_cast<int>(m_left), static_cast<int>(m_top),
                SCREEN_WIDTH, SCREEN_HEIGHT);
  m_scrollbar.set_rect(Rect(SCREEN_WIDTH - 5, static_cast<int>(m_top),
                            SCREEN_WIDTH, SCREEN_HEIGHT));
  m_scrollbar.m_covered_region = m_rect.get_height();

  m_reset_components_request = true;
}

bool
EditorSettingsWidget::on_mouse_wheel(const SDL_MouseWheelEvent& wheel)
{
  if (!m_rect.contains(m_mouse_pos))
    return false;

  move_components(-static_cast<float>(wheel.y * 30));

  return true;
}

void
EditorSettingsWidget::set_object(GameObject* object)
{
  if (m_object != object)
    m_scrollbar.m_progress = 0;

  m_object = object;

  m_reset_components_request = true;
}

void
EditorSettingsWidget::reset_components()
{
  m_children.clear();

  m_editor.open_script_editor(nullptr);

  if (!m_object)
    return;

  // Reset scrollbar position if necessary
  m_scrollbar.m_progress = math::clamp(m_scrollbar.m_progress, 0.f,
                                       m_scrollbar.m_total_region - m_scrollbar.m_covered_region);
  if (!m_scrollbar.is_valid())
    m_scrollbar.m_progress = 0.f;

  // Keeps track of the total height of all created components
  float top = 70.f - m_scrollbar.m_progress;

  const auto& messages = m_object->validate();
  for (const auto& m : messages)
  {
    auto sm = std::make_unique<SettingsMessage>(Rectf(m_rect.get_left() + 10.f,
                                                      m_rect.get_top() + top,
                                                      m_rect.get_right() - 10.f,
                                                      m_rect.get_top() + top + 100.f),
                                                m.get_message(),
                                                m.get_level());
    sm->autoresize_bottom();
    top += sm->get_rect().get_height() + 10.f;
    m_children.push_back(std::move(sm));
  }

  top += 10.f;

  const auto& settings = m_object->get_settings();
  for (auto oo_ptr = settings.get_options().begin(); oo_ptr != settings.get_options().end(); oo_ptr++)
  {
    const auto& e = *(oo_ptr->get());

    if (e.get_flags() & OPTION_HIDDEN)
      continue;

    auto control = e.add_to_settings(m_rect.get_width(), m_editor, m_object);
    if (!control.get())
      continue;

    control->get_rect().move(m_rect.p1() + Vector(0, top));
    if (control->m_label)
      control->m_label->get_rect().move(m_rect.p1() + Vector(0, top));

    auto container = dynamic_cast<InterfaceContainer*>(control.get());
    if (container)
      for (auto& child : container->m_children)
        child->get_rect().move(m_rect.p1() + Vector(0, top));

    auto old_func = control->m_on_change;
    control->m_on_change = [this, old_func](){
      if (old_func)
        old_func();
      m_object->after_editor_set();
    };

    m_children.push_back(std::move(control));

    top += 20.f + margin;
  }

  m_scrollbar.m_total_region = top + m_scrollbar.m_progress;
  m_scrollbar.m_covered_region = m_rect.get_height();
}

void
EditorSettingsWidget::move_components(float delta_)
{
  float old_progress = m_scrollbar.m_progress;
  m_scrollbar.m_progress = math::clamp(m_scrollbar.m_progress + delta_,
                                       0.f,
                                       m_scrollbar.m_total_region - m_scrollbar.m_covered_region);

  if (!m_scrollbar.is_valid())
    m_scrollbar.m_progress = 0.f;

  float delta = old_progress - m_scrollbar.m_progress;

  // TODO: Make a "get children recursively" method
  for (auto& control : m_children)
  {
    control->get_rect().move(Vector(0, delta));
    if (control->m_label)
      control->m_label->get_rect().move(Vector(0, delta));

    auto container = dynamic_cast<InterfaceContainer*>(control.get());
    if (container)
      for (auto& child : container->m_children)
        child->get_rect().move(Vector(0, delta));
  }
}

/* EOF */
