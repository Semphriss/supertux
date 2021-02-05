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

#include "interface/control.hpp"

#include "interface/container.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

InterfaceControl::InterfaceControl() :
  m_on_change(),
  m_on_focus(),
  m_label(),
  m_parent(nullptr),
  m_enabled(true),
  m_visible(true),
  m_theme(),
  m_has_focus(),
  m_mouse_hover(false),
  m_mouse_down(false),
  m_mouse_pos(),
  m_rect()
{
}

InterfaceControl::InterfaceControl(InterfaceThemeSet theme) :
  m_on_change(),
  m_on_focus(),
  m_label(),
  m_parent(nullptr),
  m_enabled(true),
  m_visible(true),
  m_theme(theme),
  m_has_focus(),
  m_mouse_hover(false),
  m_mouse_down(false),
  m_mouse_pos(),
  m_rect()
{
}

std::vector<std::function<void()>>
InterfaceControl::set_focus(bool focus, std::vector<std::function<void()>> callbacks)
{
  bool changed = m_has_focus != focus;

  m_has_focus = focus;

  if (m_on_focus && changed)
    callbacks.push_back(m_on_focus);

  if (focus && m_parent)
    m_parent->notify_focus(this, callbacks);

  if (!m_parent)
  {
    for (auto& func : callbacks)
      func();

    return {};
  }

  return callbacks;
}

void
InterfaceControl::draw_tooltip(DrawingContext& context, const std::string& text) const
{
  auto theme = get_current_theme();

  float w = theme.font->get_text_width(text) + 10.f;
  float h = theme.font->get_height() + 10.f;

  // Compression of a bazillion lines because I like messy stuff              /s
  Vector pos(
    (w > static_cast<float>(context.get_width()) - m_mouse_pos.x)
      ? std::max(0.f, m_mouse_pos.x - w)
      : m_mouse_pos.x,
    (h > static_cast<float>(context.get_height()) - m_mouse_pos.y - 32.f)
      ? std::max(0.f, m_mouse_pos.y - h)
      : m_mouse_pos.y + 32.f
  );

  Rectf box(pos, pos + Vector(w, h));

  context.color().draw_filled_rect(box,
                                   Color(.1f, .1f, .1f, .8f),
                                   LAYER_GUI + 10);
  context.color().draw_filled_rect(box.grown(-2.f),
                                   Color(1.f, 1.f, 1.f, .1f),
                                   LAYER_GUI + 10);
  context.color().draw_text(Resources::control_font,
                            text,
                            pos + Vector(5.f, 5.f),
                            FontAlignment::ALIGN_LEFT,
                            LAYER_GUI + 11,
                            Color::WHITE);
}

const InterfaceTheme&
InterfaceControl::get_current_theme() const
{
  if (!m_enabled)
    return m_theme.disabled;

  if (m_mouse_down && m_mouse_hover)
    return m_theme.active;

  if (m_has_focus)
    return m_theme.focused;

  if (m_mouse_hover)
    return m_theme.hover;

  return m_theme.base;
}

bool
InterfaceControl::on_mouse_motion(const SDL_MouseMotionEvent& motion)
{
  if (m_label)
    m_label->on_mouse_motion(motion);

  m_mouse_pos = VideoSystem::current()->get_viewport().to_logical(motion.x, motion.y);
  m_mouse_hover = m_rect.contains(m_mouse_pos);

  return false;
}
