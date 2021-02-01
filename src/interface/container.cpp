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

#include "interface/container.hpp"

InterfaceContainer::InterfaceContainer() :
  InterfaceControl(),
  m_children()
{
}

bool
InterfaceContainer::event(const SDL_Event& ev)
{
  if (!m_enabled)
    return false;

  bool self_bubble = InterfaceControl::event(ev);

  for (auto& child : m_children)
    if (child->event(ev))
      return true;

  return self_bubble;
}

void
InterfaceContainer::update(float dt_sec)
{
  for (auto& child : m_children)
    child->update(dt_sec);
}

void
InterfaceContainer::draw(DrawingContext& context)
{
  if (!m_visible)
    return;

  context.push_transform();
  context.set_translation(context.get_translation() + m_rect.p1());
  //context.set_viewport(Rect(m_rect));

  for (auto& child : m_children)
    child->draw(context);

  context.pop_transform();
}

void
InterfaceContainer::notify_focus(InterfaceControl* source_child, std::vector<std::function<void()>> callbacks)
{
  for (auto& child : m_children)
  {
    if (child.get() != source_child)
    {
      auto cb = child->set_focus(false);
      callbacks.insert(callbacks.end(), cb.begin(), cb.end());
    }
  }

  set_focus(true, callbacks);
}

std::vector<std::function<void()>>
InterfaceContainer::set_focus(bool focus, std::vector<std::function<void()>> callbacks)
{
  if (!focus)
  {
    for (auto& child : m_children)
    {
      auto cb = child->set_focus(false);
      callbacks.insert(callbacks.end(), cb.begin(), cb.end());
    }
  }

  return InterfaceControl::set_focus(focus, callbacks);
}

InterfaceControl*
InterfaceContainer::get_focused_child()
{
  for (auto& child : m_children)
    if (child->has_focus())
      return child.get();

  return nullptr;
}

bool
InterfaceContainer::has_focus() const
{
  for (auto& child : m_children)
    if (child->has_focus())
      return true;

  return false;
}

/* EOF */
