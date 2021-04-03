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

#include "supertux/chat.hpp"

#include "supertux/resources.hpp"

#include <iostream>

static const float CHAT_FADE_BKG_START = 0.f;
static const float CHAT_FADE_BKG_END = .5f;
static const float CHAT_FADE_TEXT_START = 5.f;
static const float CHAT_FADE_TEXT_END = 7.f;

Chat::Chat() :
  m_rect(16.f, 16.f, 320.f, 240.f),
  m_messages(),
  m_textbox(),
  m_visibility_time(std::max({CHAT_FADE_BKG_START, CHAT_FADE_BKG_END, CHAT_FADE_TEXT_START, CHAT_FADE_TEXT_END}))
{
  InterfaceThemeSet theme(
    InterfaceTheme(Resources::chat_font, Color::WHITE, Color(0.f, 0.f, 0.f, .1f), 0.f),
    InterfaceTheme(Resources::chat_font, Color::WHITE, Color(0.f, 0.f, 0.f, .2f), 0.f),
    InterfaceTheme(Resources::chat_font, Color::WHITE, Color(0.f, 0.f, 0.f, .3f), 0.f),
    InterfaceTheme(Resources::chat_font, Color::WHITE, Color(0.f, 0.f, 0.f, .3f), 0.f),
    InterfaceTheme(Resources::chat_font, Color(1.f, 1.f, 1.f, 0.5f), Color(0.5f, 0.5f, 0.5f, .3f), 0.f)
  );
  m_textbox.m_theme = theme;

  Rectf text_rect = m_rect;
  text_rect.set_top(text_rect.get_bottom() - 32.f);
  m_textbox.set_rect(text_rect.grown(-4.f));

  m_textbox.m_layer = 1271;

  // I'm not making this variable public   ~Semphris
  auto* textbox = &m_textbox;
  m_textbox.m_on_change = [this, textbox](){
    Message m;
    m.author = "Me";
    m.color = Color::MAGENTA;
    m.message = textbox->get_string();
    push_message(m);
    textbox->set_string("");
  };

  m_rect.set_bottom(m_rect.get_bottom() - 36.f);
}

void
Chat::push_message(const Message& message)
{
  m_messages.push_back(message);
  m_visibility_time = std::min(m_visibility_time, CHAT_FADE_BKG_END);
}

void
Chat::update(float dt_sec)
{
  m_textbox.update(dt_sec);

  for (auto& message : m_messages)
  {
    message.update(dt_sec);
  }

  if (rand() < RAND_MAX / 1024)
  {
    std::string s, ts;
    int j = rand() % 5 + 2;
    for (int i = 0; i < j; i++)
    {
      std::cin >> ts;
      s += " " + ts;
    }

    Message m;
    m.author = "Bot";
    m.color = Color::CYAN;
    m.message = s;
    push_message(m);
  }

  if (!m_textbox.has_focus())
    m_visibility_time += dt_sec;
}

void
Chat::draw(DrawingContext& context)
{
  m_rect.move(Vector(0, context.get_height() * .9f - m_rect.get_height() - m_rect.get_top()));
  m_textbox.get_rect().move(Vector(0, m_rect.get_bottom() + 8.f - m_textbox.get_rect().get_top()));

  const float bkg_a = math::clamp(1.f - (m_visibility_time - CHAT_FADE_BKG_START) /
                            (CHAT_FADE_BKG_END - CHAT_FADE_BKG_START), 0.f, 1.f);
  const float txt_a = math::clamp(1.f - (m_visibility_time - CHAT_FADE_TEXT_START) /
                          (CHAT_FADE_TEXT_END - CHAT_FADE_TEXT_START), 0.f, 1.f);

  context.push_transform();
  context.transform().alpha = bkg_a;
  context.color().draw_filled_rect(m_textbox.get_rect().grown(4.f),
                                   Color(0.f, 0.f, 0.f, .2f),
                                   1270);
  context.color().draw_filled_rect(m_rect,
                                   Color(0.f, 0.f, 0.f, .2f),
                                   1270);
  m_textbox.draw(context);
  context.pop_transform();


  context.push_transform();
  context.transform().clip = Rect(m_rect);
  context.transform().alpha = txt_a;

  float top = m_rect.get_top();
  for (const auto& m : m_messages)
  {
    context.color().draw_text(Resources::chat_font,
                              m.author,
                              Vector(m_rect.get_left() + 5.f, top),
                              FontAlignment::ALIGN_LEFT,
                              1271,
                              m.color);
    context.color().draw_text(Resources::chat_font,
                              ": " + m.message,
                              Vector(m_rect.get_left() + 5.f + 
                                        Resources::chat_font->get_text_width(m.author),
                                     top),
                              FontAlignment::ALIGN_LEFT,
                              1271,
                              (!m.author.empty()) ? Color::WHITE : m.color);
    top += Resources::chat_font->get_height();
  }

  context.pop_transform();
}

bool
Chat::event(const SDL_Event& event)
{
  if (m_textbox.event(event) || m_textbox.has_focus())
  {
    if (event.type == SDL_KEYDOWN && (event.key.keysym.sym == SDLK_RETURN ||
                                          event.key.keysym.sym == SDLK_RETURN2))
      m_textbox.set_focus(false);

    m_visibility_time = 0.f;

    return true;
  }

  if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_3)
  {
    m_textbox.set_focus(true);
    return true;
  }

  return false;
}

/* EOF */
