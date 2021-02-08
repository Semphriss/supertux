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

#ifndef HEADER_SUPERTUX_EDITOR_SETTINGS_WIDGET_HPP
#define HEADER_SUPERTUX_EDITOR_SETTINGS_WIDGET_HPP

#include "interface/container.hpp"
#include "interface/control_scrollbar.hpp"

class DrawingContext;
class Editor;
class GameObject;

/** A panel to hold an object's settings. */
class EditorSettingsWidget final : public InterfaceContainer
{
private:
  static constexpr const float margin = 8.f;

public:
  EditorSettingsWidget(Editor& editor);
  virtual ~EditorSettingsWidget() {}

  virtual void draw(DrawingContext& context) override;
  virtual void update(float dt_sec) override;
  virtual void resize() override;
  virtual bool on_mouse_wheel(const SDL_MouseWheelEvent& wheel) override;

  virtual bool event(const SDL_Event& event) override { return m_scrollbar.event(event) || InterfaceContainer::event(event); }


  void set_object(GameObject* object);
  void reset_components();
  void set_left(float left) { m_left = left; }
  void set_top(float top) { m_top = top; }

private:
  Editor& m_editor;
  ControlScrollbar m_scrollbar;
  float m_left, m_top;
  Rectf m_rect;
  GameObject* m_object;

private:
  EditorSettingsWidget(const EditorSettingsWidget&) = delete;
  EditorSettingsWidget& operator=(const EditorSettingsWidget&) = delete;
};

#endif

/* EOF */
