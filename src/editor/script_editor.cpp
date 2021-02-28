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

#include "editor/script_editor.hpp"

#include "gui/mousecursor.hpp"
#include "supertux/globals.hpp"
#include "supertux/screen_fade.hpp"
#include "supertux/screen_manager.hpp"
#include "util/gettext.hpp"
#include "video/compositor.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

bool
ScriptEditor::is_active()
{
  return ScriptEditor::current() != nullptr;
}

ScriptEditor::ScriptEditor(Editor& editor, std::string* source) :
  m_editor(editor),
  m_widgets(),
  m_script_editor(),
  m_topbar_widget()
{
  std::vector<EditorTopbarWidget::MenuSection> menu = {
    {_("File"), {
      {"file:exit", _("Exit script editor"), "", false, [this]{
        ScreenManager::current()->pop_screen();
      }},
    }},
  };

  auto topbar_widget = std::make_unique<EditorTopbarWidget>(m_editor, menu);
  auto script_editor = std::make_unique<ScriptEditorWidget>();
  script_editor->bind_string(source);

  m_topbar_widget = topbar_widget.get();
  m_script_editor = script_editor.get();
  m_script_editor->set_rect(Rectf(Rect(0, 24, SCREEN_WIDTH, SCREEN_HEIGHT)));

  m_widgets.push_back(std::move(topbar_widget));
  m_widgets.push_back(std::move(script_editor));

  m_topbar_widget->reset_components();
}

ScriptEditor::~ScriptEditor()
{
}

void
ScriptEditor::draw(Compositor& compositor)
{
  auto& context = compositor.make_context();

  for (auto& w : m_widgets)
    w->draw(context);

  MouseCursor::current()->draw(context);
}

void
ScriptEditor::update(float dt_sec, const Controller& controller)
{
  for (auto& w : m_widgets)
    w->update(dt_sec);
}

void
ScriptEditor::event(const SDL_Event& ev)
{
  for (auto& w : m_widgets)
    if (w->event(ev))
      return;

  if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE)
    ScreenManager::current()->pop_screen();
}

IntegrationStatus
ScriptEditor::get_status() const
{
  return m_editor.get_status();
}

/* EOF */
