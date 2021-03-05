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

#ifndef HEADER_SUPERTUX_EDITOR_SCRIPT_EDITOR_WIDGET_HPP
#define HEADER_SUPERTUX_EDITOR_SCRIPT_EDITOR_WIDGET_HPP

#include "interface/control_textarea.hpp"

#include "squirrel/squirrel_util.hpp"

class ScriptEditorWidget : public ControlTextarea
{
public:
  static ScriptEditorWidget* squirrel_callback_editor;

public:
  ScriptEditorWidget();

  virtual void draw(DrawingContext& context) override;
  virtual void update(float dt_sec) override;
  virtual bool event(const SDL_Event& ev) override;

  virtual void draw_text(DrawingContext& context) override;

  void set_error(int l, int c, std::string m)
  {
    m_err_line = l;
    m_err_col = c;
    m_err_msg = m;
  }

  /** Publicly accessible bridge to get_contents() */
  std::string get_contents_hot() { return get_contents(); }

private:
  void refresh_script_validation();
  float calculate_margin();

private:
  float m_validate_timer;

  int m_err_line;
  int m_err_col;
  std::string m_err_msg;

private:
  ScriptEditorWidget(const ScriptEditorWidget&) = delete;
  ScriptEditorWidget& operator=(const ScriptEditorWidget&) = delete;
};

#endif

/* EOF */
