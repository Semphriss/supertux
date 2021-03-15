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

#ifndef HEADER_SUPERTUX_EDITOR_CANVAS_EDITOR_HPP
#define HEADER_SUPERTUX_EDITOR_CANVAS_EDITOR_HPP

#include "editor/editor.hpp"
#include "supertux/screen.hpp"

class CanvasEditor final : public Screen
{
private:
  struct CanvasPanel {
    std::string name;
    float x, y, w, h;
  };

public:
  CanvasEditor(Editor& editor, Sector& sector);

  virtual void draw(Compositor&) override;
  virtual void update(float dt_sec, const Controller& controller) override;
  virtual void event(const SDL_Event& ev) override;
  virtual IntegrationStatus get_status() const override;

private:
  bool toggle_panel(CanvasPanel* panel);
  bool panel_selected(CanvasPanel* panel) { return std::find(m_selected_panels.begin(), m_selected_panels.end(), panel) != m_selected_panels.end(); }

  void draw_arrow(DrawingContext& context, CanvasPanel& from, CanvasPanel& to, std::string label = "");

private:
  Editor& m_editor;
  Sector& m_sector;

  std::vector<CanvasPanel> m_panels;
  CanvasPanel m_colormap_panel, m_lightmap_panel;

  Vector m_mouse_pos;
  std::vector<CanvasPanel*> m_selected_panels;
  bool m_dragging;

private:
  CanvasEditor(const CanvasEditor&) = delete;
  CanvasEditor& operator=(const CanvasEditor&) = delete;
};

#endif

/* EOF */
