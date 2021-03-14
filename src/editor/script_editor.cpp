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

#include "version.h"

#include "gui/dialog.hpp"
#include "gui/mousecursor.hpp"
#include "supertux/globals.hpp"
#include "supertux/screen_fade.hpp"
#include "supertux/screen_manager.hpp"
#include "util/gettext.hpp"
#include "video/compositor.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

ScriptEditor::ScriptEditor(Editor& editor, std::string* source) :
  m_source(source),
  m_undo_stack(),
  m_redo_stack(),
  m_editor(editor),
  m_widgets(),
  m_script_editor(),
  m_topbar_widget(),
  m_enabled(true)
{
  std::vector<EditorTopbarWidget::MenuSection> menu = {
    {_("File"), {
      {"file:save", _("Save script"), "", false, [this]{
        this->save();
      }},
      {"file:exit", _("Exit script editor"), "", true, [this]{
        this->quit();
      }},
    }},
    {_("Edit"), {
      {"edit:undo", _("Undo"), "", false, [this]{
        this->undo();
      }},
      {"edit:redo", _("Redo"), "", false, [this]{
        this->redo();
      }},
    }},
    {_("Settings"), {
      {"settings:autocomplete", _("Enable autocomplete"), "", false, [this]{
        this->undo();
      }},
    }},
    {_("Help"), {
      {"help:tutorial", _("Scripting reference"), "", false, [this]{
        /*Dialog::show_confirmation("This will open the following link in your browser:\n"
                                  SUPERTUX_LINK_SCRIPTING_REFERENCE "\n"
                                  "Continue?", []{
          FileSystem::open_path(SUPERTUX_LINK_SCRIPTING_REFERENCE);
        });*/
      }},
    }},
  };

  auto topbar_widget = std::make_unique<EditorTopbarWidget>(m_editor, menu);
  auto script_editor = std::make_unique<ScriptEditorWidget>();

  m_topbar_widget = topbar_widget.get();
  m_script_editor = script_editor.get();
  m_script_editor->set_rect(Rectf(Rect(0, 24, SCREEN_WIDTH, SCREEN_HEIGHT)));

  m_widgets.push_back(std::move(topbar_widget));
  m_widgets.push_back(std::move(script_editor));

  m_topbar_widget->reset_components();

  m_script_editor->set_string(*source);

  m_undo_stack.push_back({
    *source,
    m_script_editor->get_caret_pos(),
    m_script_editor->get_secondary_caret_pos()
  });
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
  if (!m_enabled)
    return;

  for (auto& w : m_widgets)
    if (w->event(ev))
      break;

  if (m_script_editor->get_contents_hot() != m_undo_stack.back().content)
  {
    m_undo_stack.push_back({
      m_script_editor->get_contents_hot(),
      m_script_editor->get_caret_pos(),
      m_script_editor->get_secondary_caret_pos()
    });
    m_redo_stack.clear();
  }
  else
  {
    m_undo_stack.back().caret_1 = m_script_editor->get_caret_pos();
    m_undo_stack.back().caret_2 = m_script_editor->get_secondary_caret_pos();
  }

  if (ev.type == SDL_KEYDOWN)
  {
    if (ev.key.keysym.sym == SDLK_ESCAPE)
      quit();

    if (ev.key.keysym.sym == SDLK_s && (ev.key.keysym.mod & KMOD_CTRL))
      save();

    if (ev.key.keysym.sym == SDLK_z && (ev.key.keysym.mod & KMOD_CTRL))
      undo();

    if (ev.key.keysym.sym == SDLK_y && (ev.key.keysym.mod & KMOD_CTRL))
      redo();
  }
}

void
ScriptEditor::quit()
{
  if (m_script_editor->get_contents_hot() == *m_source && m_redo_stack.empty())
  {
    ScreenManager::current()->pop_screen();
  }
  else
  {
    auto dialog = std::make_unique<Dialog>();
    dialog->set_text(_("This script contains unsaved changes, do you want to save?"));
    dialog->add_default_button(_("Yes"), [this] {
      this->save();
      ScreenManager::current()->pop_screen();
    });
    dialog->add_button(_("No"), [this] {
      ScreenManager::current()->pop_screen();
    });
    dialog->add_cancel_button(_("Cancel"), [this] {
      m_enabled = true;
    });
    MenuManager::instance().set_dialog(std::move(dialog));
  }
}

void
ScriptEditor::save()
{
  *m_source = m_script_editor->get_contents_hot();
}

void 
ScriptEditor::undo()
{
  if (m_undo_stack.size() <= 1)
    return;

  m_redo_stack.push_back(m_undo_stack.back());
  m_undo_stack.pop_back();

  m_script_editor->set_string(m_undo_stack.back().content);
  m_script_editor->set_caret_pos(m_undo_stack.back().caret_1);
  m_script_editor->set_secondary_caret_pos(m_undo_stack.back().caret_2);
}

void
ScriptEditor::redo()
{
  if (m_redo_stack.size() <= 0)
    return;

  m_undo_stack.push_back(m_redo_stack.back());
  m_redo_stack.pop_back();

  m_script_editor->set_string(m_undo_stack.back().content);
  m_script_editor->set_caret_pos(m_undo_stack.back().caret_1);
  m_script_editor->set_secondary_caret_pos(m_undo_stack.back().caret_2);
}

IntegrationStatus
ScriptEditor::get_status() const
{
  return m_editor.get_status();
}

/* EOF */
