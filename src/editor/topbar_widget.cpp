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

#include "editor/topbar_widget.hpp"

#include "editor/editor.hpp"
#include "editor/topbar_entry.hpp"
#include "video/compositor.hpp"

#include <iostream>

EditorTopbarWidget::EditorTopbarWidget(Editor& editor, std::vector<MenuSection> menu) :
  InterfaceContainer(),
  m_editor(editor),
  m_menu(menu)
{
}

void
EditorTopbarWidget::reset_components()
{
  m_children.clear();

  InterfaceThemeSet button_theme = InterfaceThemeSet(
    InterfaceTheme(Resources::control_font, Color(.8f, .8f, .8f), Color(1.f, 1.f, 1.f, 0.f), 0.f), // base
    InterfaceTheme(Resources::control_font, Color(.9f, .9f, .9f), Color(1.f, 1.f, 1.f, .1f), 0.f), // hover
    InterfaceTheme(Resources::control_font, Color(1.f, 1.f, 1.f), Color(1.f, 1.f, 1.f, .3f), 0.f), // active
    InterfaceTheme(Resources::control_font, Color(1.f, 1.f, 1.f), Color(1.f, 1.f, 1.f, .2f), 0.f), // focused
    InterfaceTheme(Resources::control_font, Color(.5f, .5f, .5f), Color(1.f, 1.f, 1.f, .0f), 0.f) // disabled
  );

  InterfaceThemeSet submenu_theme = InterfaceThemeSet(
    InterfaceTheme(Resources::control_font, Color(.8f, .8f, .8f), Color(.2f, .2f, .2f, 1.f), 0.f), // base
    InterfaceTheme(Resources::control_font, Color(.9f, .9f, .9f), Color(.3f, .3f, .3f, 1.f), 0.f), // hover
    InterfaceTheme(Resources::control_font, Color(1.f, 1.f, 1.f), Color(.4f, .4f, .4f, 1.f), 0.f), // active
    InterfaceTheme(Resources::control_font, Color(1.f, 1.f, 1.f), Color(.4f, .4f, .4f, 1.f), 0.f), // focused
    InterfaceTheme(Resources::control_font, Color(.5f, .5f, .5f), Color(.3f, .3f, .3f, 1.f), 0.f) // disabled
  );

  const float margin = 12.f;
  const float submenu_margin = 4.f;
  float x = 48.f;
  for (const auto& category : m_menu)
  {
    const float btn_width = button_theme.base.font->get_text_width(category.name) + 2.f * margin;

    float submenu_width = btn_width;
    for (const auto& option : category.options)
      submenu_width = std::max(submenu_width, submenu_theme.base.font->get_text_width(option.name) + 2.f * submenu_margin + 25.f); // 25.f: Icon margin

    float y = 24.f;

    auto submenu_container = std::make_unique<InterfaceContainer>();
    auto submenu_ptr = submenu_container.get();
    for (const auto& option : category.options)
    {
      auto entry = std::make_unique<TopbarEntry>(option.name, option.icon);
      entry->separator = option.new_category;
      entry->set_rect(Rectf(x, y, x + submenu_width, y + 24.f));
      entry->m_theme = submenu_theme;
      entry->m_on_change = [submenu_ptr, option] {
        submenu_ptr->m_enabled = false;
        submenu_ptr->m_visible = false;
        option.callback();
      };

      entry->m_parent = submenu_ptr;
      submenu_container->m_children.push_back(std::move(entry));
      y += 24.f;
    }

    submenu_ptr->m_on_focus = std::function<void()>([submenu_ptr](){
      submenu_ptr->m_enabled = submenu_ptr->has_focus();
      submenu_ptr->m_visible = submenu_ptr->has_focus();
    });

    submenu_ptr->m_enabled = false;
    submenu_ptr->m_visible = false;

    auto title = std::make_unique<ControlButton>(category.name);
    title->set_rect(Rectf(x, 0.f, x + btn_width, 24.f));
    title->m_theme = button_theme;
    title->m_action_on_mousedown = true;
    auto title_ptr = title.get();
    title->m_on_change = std::function<void()>([this, submenu_ptr, title_ptr](){
      for (auto& c : m_children)
      {
        if (dynamic_cast<InterfaceContainer*>(c.get()))
        {
          c->m_enabled = false;
          c->m_visible = false;
        }
      }

      submenu_ptr->m_enabled = !submenu_ptr->m_enabled;
      submenu_ptr->m_visible = submenu_ptr->m_enabled;
    });

    title->m_parent = this;
    submenu_container->m_parent = this;
    m_children.push_back(std::move(title));
    m_children.push_back(std::move(submenu_container));

    x += btn_width;
  }
}

void
EditorTopbarWidget::draw(DrawingContext& context)
{
  context.color().draw_filled_rect(Rectf(0.f, 0.f, static_cast<float>(context.get_width()), 24.f),
                                    Color::BLACK, LAYER_GUI - 11);

  InterfaceContainer::draw(context);
}

bool
EditorTopbarWidget::on_mouse_button_up(const SDL_MouseButtonEvent& button)
{
  bool r = InterfaceContainer::on_mouse_button_up(button);

  // Hacky way to hide the submenus then the user clicks elsewhere on the screen
  for (auto& c : m_children)
    if (c->has_focus())
      return r;

  for (auto& c : m_children)
  {
    auto d = dynamic_cast<InterfaceContainer*>(c.get());
    if (d)
    {
      d->m_enabled = false;
      d->m_visible = false;
    }
  }

  return r;
}

EditorTopbarWidget::MenuEntry*
EditorTopbarWidget::get_entry_by_id(const std::string& id)
{
  for (auto& section : m_menu)
    for (auto& entry : section.options)
      if (entry.id == id)
        return &entry;

  return nullptr;
}

/* EOF */
