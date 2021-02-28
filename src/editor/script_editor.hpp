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

#ifndef HEADER_SUPERTUX_EDITOR_SCRIPT_EDITOR_HPP
#define HEADER_SUPERTUX_EDITOR_SCRIPT_EDITOR_HPP

#include "interface/control_textarea.hpp"

#include "squirrel/squirrel_util.hpp"

class ScriptEditor : public ControlTextarea
{
public:
  static ScriptEditor* squirrel_callback_editor;

public:
  ScriptEditor();

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

private:
  void refresh_script_validation();

private:
  float m_validate_timer;

  int m_err_line;
  int m_err_col;
  std::string m_err_msg;

private:
  ScriptEditor(const ScriptEditor&) = delete;
  ScriptEditor& operator=(const ScriptEditor&) = delete;
};

#endif

/* EOF */
