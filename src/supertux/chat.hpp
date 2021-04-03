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

#ifndef HEADER_SUPERTUX_SUPERTUX_CHAT_HPP
#define HEADER_SUPERTUX_SUPERTUX_CHAT_HPP

#include <string>

#include <SDL.h>

#include "interface/control_textbox.hpp"
#include "math/rectf.hpp"
#include "video/color.hpp"
#include "video/drawing_context.hpp"

// TODO: Extend src/interface/control.hpp ?
class Chat final
{
public:
  struct Message {
    std::string author;
    std::string message;
    Color color;

    void update(float dt_sec)
    {
      alpha += dt_sec;
      alpha = std::min(1.f, alpha);
    }
  private:
    float alpha = 0.f;
  };

public:
  Chat();

  bool event(const SDL_Event& ev);
  void draw(DrawingContext& context);
  void update(float dt_sec);

  void push_message(const Message& message);

private:
  Rectf m_rect;
  std::vector<Message> m_messages;
  ControlTextbox m_textbox;
  float m_visibility_time;

private:
  Chat(const Chat&) = delete;
  Chat& operator=(const Chat&) = delete;
};

#endif

/* EOF */
