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

#ifndef HEADER_SUPERTUX_EDITOR_OBJECT_WIDGET_HPP
#define HEADER_SUPERTUX_EDITOR_OBJECT_WIDGET_HPP

#include "editor/object_info.hpp"
#include "editor/tile_selection.hpp"
#include "interface/container.hpp"
#include "interface/control_scrollbar.hpp"

class DrawingContext;
class Editor;
class GameObject;

/** A panel to hold an object's settings. */
class EditorObjectWidget final : public InterfaceContainer
{
public:
  enum class InputType {
    NONE, TILE, OBJECT
  };

private:
  static constexpr const float margin = 8.f;

public:
  EditorObjectWidget(Editor& editor);
  virtual ~EditorObjectWidget() {}

  virtual void draw(DrawingContext& context) override;
  virtual void update(float dt_sec) override;
  virtual void resize() override;
  virtual bool on_mouse_wheel(const SDL_MouseWheelEvent& wheel) override;

  virtual bool event(const SDL_Event& event) override { return m_scrollbar.event(event) || InterfaceContainer::event(event); }

  void reset_components();
  void set_left(float left) { m_left = left; }
  void set_bottom(float bottom) { m_bottom = bottom; }

  void update_mouse_icon() {/**/}

  int get_tileselect_select_mode() const { return 0; /**/ }
  int get_tileselect_move_mode() const { return 0; /**/ }

  void select_tilegroup(int id) {/**/}
  void select_objectgroup(int id) {/**/}

  const ObjectInfo& get_object_info() const { return *m_object_info; }
  InputType get_input_type() const { return m_input_type; }
  void set_input_type(InputType input_type) { m_input_type = input_type; }

  std::string get_object() const { return m_object; }
  TileSelection* get_tiles() const { return m_tiles.get(); }

private:
  Editor& m_editor;
  ControlScrollbar m_scrollbar;
  float m_left, m_bottom;
  Rectf m_rect;
  std::unique_ptr<ObjectInfo> m_object_info;
  std::unique_ptr<TileSelection> m_tiles;
  std::string m_object;
  InputType m_input_type;

private:
  EditorObjectWidget(const EditorObjectWidget&) = delete;
  EditorObjectWidget& operator=(const EditorObjectWidget&) = delete;
};

#endif

/* EOF */
