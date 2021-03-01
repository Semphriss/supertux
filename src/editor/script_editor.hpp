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

#ifndef HEADER_SUPERTUX_EDITOR_SCRIPT_EDITOR_HPP
#define HEADER_SUPERTUX_EDITOR_SCRIPT_EDITOR_HPP

#include "editor/editor.hpp"
#include "editor/script_editor_widget.hpp"
#include "editor/topbar_widget.hpp"
#include "supertux/screen.hpp"
#include "util/currenton.hpp"

#include <SDL.h>

class ScriptEditor final : public Screen,
                           public Currenton<ScriptEditor>
{
private:
  struct Version {
    std::string content;
    int caret_1, caret_2;
  };

public:
  ScriptEditor(Editor& editor, std::string* source);
  ~ScriptEditor();

  virtual void draw(Compositor&) override;
  virtual void update(float dt_sec, const Controller& controller) override;
  virtual void event(const SDL_Event& ev) override;
  virtual IntegrationStatus get_status() const override;

private:
  void quit();
  void save();

  void undo();
  void redo();

private:
  std::string* m_source;
  std::vector<Version> m_undo_stack, m_redo_stack;
  Editor& m_editor;
  std::vector<std::unique_ptr<Widget> > m_widgets;
  ScriptEditorWidget* m_script_editor;
  EditorTopbarWidget* m_topbar_widget;
  bool m_enabled;

private:
  ScriptEditor(const ScriptEditor&) = delete;
  ScriptEditor& operator=(const ScriptEditor&) = delete;
};

#endif

/* EOF */
