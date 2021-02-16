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
#include "gui/dialog.hpp"
#include "gui/menu_manager.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/level.hpp"
#include "supertux/menu/editor_sectors_menu.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "util/gettext.hpp"
#include "video/compositor.hpp"

#include <iostream>

EditorTopbarWidget::EditorTopbarWidget(Editor& editor) :
  InterfaceContainer(),
  m_editor(editor),
  m_menu({
    {_("File"), {
      {"file:save", _("Save Level"), "", false, [this]{
        this->m_editor.check_save_prerequisites([this]() {
          this->m_editor.m_save_request = true;
        });
      }},
      {"file:test", _("Test Level"), "", false, [this]{
        this->m_editor.check_save_prerequisites([this]() {
          this->m_editor.m_test_pos = boost::none;
          this->m_editor.m_test_request = true;
        });
      }},
      {"file:share", _("Share Level"), "", false, [this]{
        auto dialog = std::make_unique<Dialog>();
        dialog->set_text(_("We encourage you to share your levels in the SuperTux forum.\nTo find your level, click the\n\"Open Level directory\" menu item.\nDo you want to go to the forum now?"));
        dialog->add_default_button(_("Yes"), [] {
          FileSystem::open_path("https://forum.freegamedev.net/viewforum.php?f=69");
        });
        dialog->add_cancel_button(_("No"));
        MenuManager::instance().set_dialog(std::move(dialog));
      }},
      {"file:open-dir", _("Open Level Directory"), "", false, [this]{
        this->m_editor.open_level_directory();
      }},
      {"file:change-level", _("Edit Another Level"), "", true, [this]{
        this->m_editor.check_unsaved_changes([] {
          MenuManager::instance().set_menu(MenuStorage::EDITOR_LEVEL_SELECT_MENU);
        });
      }},
      {"file:change-world", _("Edit Another World"), "", false, [this]{
        this->m_editor.check_unsaved_changes([] {
          MenuManager::instance().set_menu(MenuStorage::EDITOR_LEVELSET_SELECT_MENU);
        });
      }},
      {"file:exit", _("Exit Level Editor"), "", true, [this]{
        this->m_editor.m_quit_request = true;
      }}
    }},
    {_("Sector"), {
      {"sector:manage", _("Manage sectors..."), "", false, [this]{
        this->m_editor.disable_keyboard();
        MenuManager::instance().set_menu(MenuStorage::EDITOR_SECTORS_MENU);
      }},
      {"sector:create", _("Create sector"), "", false, []{
        EditorSectorsMenu::create_sector();
      }},
      {"sector:delete", _("Delete sector"), "", false, []{
        EditorSectorsMenu::delete_sector();
      }}
    }},
    {_("Settings"), {
      {"settings:grid-size-0", _("Grid size: ") + _("tiny tile (4px)"), "", false, [this]{
        EditorOverlayWidget::selected_snap_grid_size = 0;
        this->refresh_menu();
      }},
      {"settings:grid-size-1", _("Grid size: ") + _("small tile (8px)"), "", false, [this]{
        EditorOverlayWidget::selected_snap_grid_size = 1;
        this->refresh_menu();
      }},
      {"settings:grid-size-2", _("Grid size: ") + _("medium tile (16px)"), "", false, [this]{
        EditorOverlayWidget::selected_snap_grid_size = 2;
        this->refresh_menu();
      }},
      {"settings:grid-size-3", _("Grid size: ") + _("big tile (32px)"), "", false, [this]{
        EditorOverlayWidget::selected_snap_grid_size = 3;
        this->refresh_menu();
      }},
      {"settings:show-grid", _("Show Grid"), "", true, [this]{
        EditorOverlayWidget::render_grid = !EditorOverlayWidget::render_grid;
        this->refresh_menu();
      }},
      {"settings:grid-snap", _("Grid Snapping"), "", false, [this]{
        EditorOverlayWidget::snap_to_grid = !EditorOverlayWidget::snap_to_grid;
        this->refresh_menu();
      }},
      {"settings:render-bkg", _("Render Background"), "", false, [this]{
        EditorOverlayWidget::render_background = !EditorOverlayWidget::render_background;
        this->refresh_menu();
      }},
      {"settings:render-light", _("Render Light"), "", false, [this]{
        Compositor::s_render_lighting = !Compositor::s_render_lighting;
        this->refresh_menu();
      }},
      {"settings:autotile", _("Autotile Mode"), "", false, [this]{
        EditorOverlayWidget::autotile_mode = !EditorOverlayWidget::autotile_mode;
        this->refresh_menu();
      }},
      {"settings:autotile-help", _("Enable Autotile Help"), "", false, [this]{
        EditorOverlayWidget::autotile_help = !EditorOverlayWidget::autotile_help;
        this->refresh_menu();
      }},
    }},
  })
{
  refresh_menu();
}

void
EditorTopbarWidget::refresh_menu()
{
  get_entry_by_id("settings:grid-size-0")->icon = EditorOverlayWidget::selected_snap_grid_size == 0 ? "/images/engine/editor/arrow.png" : "";
  get_entry_by_id("settings:grid-size-1")->icon = EditorOverlayWidget::selected_snap_grid_size == 1 ? "/images/engine/editor/arrow.png" : "";
  get_entry_by_id("settings:grid-size-2")->icon = EditorOverlayWidget::selected_snap_grid_size == 2 ? "/images/engine/editor/arrow.png" : "";
  get_entry_by_id("settings:grid-size-3")->icon = EditorOverlayWidget::selected_snap_grid_size == 3 ? "/images/engine/editor/arrow.png" : "";

  get_entry_by_id("settings:show-grid")->icon = EditorOverlayWidget::render_grid ? "/images/engine/editor/arrow.png" : "";
  get_entry_by_id("settings:grid-snap")->icon = EditorOverlayWidget::snap_to_grid ? "/images/engine/editor/arrow.png" : "";
  get_entry_by_id("settings:render-bkg")->icon = EditorOverlayWidget::render_background ? "/images/engine/editor/arrow.png" : "";
  get_entry_by_id("settings:render-light")->icon = Compositor::s_render_lighting ? "/images/engine/editor/arrow.png" : "";
  get_entry_by_id("settings:autotile")->icon = EditorOverlayWidget::autotile_mode ? "/images/engine/editor/arrow.png" : "";
  get_entry_by_id("settings:autotile-help")->icon = EditorOverlayWidget::autotile_help ? "/images/engine/editor/arrow.png" : "";

  reset_components();
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
  InterfaceContainer::draw(context);

  context.color().draw_filled_rect(Rectf(0.f, 0.f, static_cast<float>(context.get_width()), 24.f),
                                    Color::BLACK, LAYER_GUI - 11);
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
