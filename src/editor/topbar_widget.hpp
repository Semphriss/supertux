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

#ifndef HEADER_SUPERTUX_EDITOR_TOPBAR_WIDGET_HPP
#define HEADER_SUPERTUX_EDITOR_TOPBAR_WIDGET_HPP

#include "interface/container.hpp"

class Editor;

/** The toolbox is on the right side of the screen and allows
    selection of the current tool and contains the object or tile
    palette */
class EditorTopbarWidget final : public InterfaceContainer
{
public:
  struct MenuEntry
  {
    std::string id;
    std::string name;
    std::string icon;
    bool new_category;
    std::function<void()> callback;
  };

  struct MenuSection
  {
    std::string name;
    std::vector<MenuEntry> options;
  };

public:
  EditorTopbarWidget(Editor& editor, std::vector<MenuSection> menu);
  virtual ~EditorTopbarWidget() {}

  virtual void draw(DrawingContext& context) override;
  virtual bool on_mouse_button_up(const SDL_MouseButtonEvent& button) override;

  void reset_components();

  MenuEntry* get_entry_by_id(const std::string& id);

private:
  Editor& m_editor;
  std::vector<MenuSection> m_menu;

private:
  EditorTopbarWidget(const EditorTopbarWidget&) = delete;
  EditorTopbarWidget& operator=(const EditorTopbarWidget&) = delete;
};

#endif

/* EOF */
