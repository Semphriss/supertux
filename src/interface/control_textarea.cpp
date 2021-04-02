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

#include "interface/control_textarea.hpp"

#include <iostream>

ControlTextarea::ControlTextarea() :
  ControlTextbox(),
  m_v_scrollbar(),
  m_h_scrollbar(),
  m_right_margin(0.f)
{
}

void
ControlTextarea::set_rect(const Rectf& rect)
{
  m_rect = rect;
  m_v_scrollbar.set_rect(rect);
  m_v_scrollbar.get_rect().set_left(m_v_scrollbar.get_rect().get_right() - 8.f);
  m_v_scrollbar.get_rect().set_bottom(m_v_scrollbar.get_rect().get_bottom() - get_current_theme().font->get_height() - 8.f);

  m_h_scrollbar.set_rect(rect);
  m_h_scrollbar.get_rect().set_top(m_h_scrollbar.get_rect().get_bottom() - 8.f);
  m_h_scrollbar.get_rect().move(Vector(0, -get_current_theme().font->get_height()));
  m_h_scrollbar.get_rect().set_right(m_h_scrollbar.get_rect().get_right() - 8.f);
  m_h_scrollbar.m_horizontal = true;

  m_v_scrollbar.m_theme = InterfaceThemeSet(
    InterfaceTheme(Resources::control_font, Color(.7f, .7f, .7f, 1.f), Color::BLACK, 0.f), // base
    InterfaceTheme(Resources::control_font, Color(.8f, .8f, .8f, 1.f), Color::BLACK, 0.f), // hover
    InterfaceTheme(Resources::control_font, Color(1.f, 1.f, 1.f, 1.f), Color::BLACK, 0.f), // active
    InterfaceTheme(Resources::control_font, Color(.9f, .9f, .9f, 1.f), Color::BLACK, 0.f), // focused
    InterfaceTheme(Resources::control_font, Color(.3f, .3f, .3f, 1.f), Color::BLACK, 0.f) // disabled
  );
  m_h_scrollbar.m_theme = m_v_scrollbar.m_theme;

  reset_scrollbar();
}

bool
ControlTextarea::event(const SDL_Event& ev)
{
  return m_enabled && (m_v_scrollbar.event(ev) || m_h_scrollbar.event(ev) || ControlTextbox::event(ev));
}

void
ControlTextarea::draw(DrawingContext& context)
{
  if (!m_visible)
    return;

  context.push_transform();
  context.transform().clip = context.transform().clip.intersect(Rect(m_rect));

  InterfaceControl::draw(context);

  const InterfaceTheme& theme = get_current_theme();

  context.color().draw_filled_rect(m_rect,
                                   theme.bkg_color,
                                   theme.round_corner,
                                   LAYER_GUI);

  if (m_caret_pos != m_secondary_caret_pos) {
    int caret_1 = std::min(m_caret_pos, m_secondary_caret_pos),
        caret_2 = std::max(m_caret_pos, m_secondary_caret_pos);

    int line_1 = get_line_num(caret_1),
        line_2 = get_line_num(caret_2);

    for (int i = line_1; i <= line_2; i++)
    {
      float hgt = theme.font->get_height() * static_cast<float>(i) + 5.f;
      float lft = m_right_margin + ((i != line_1) ? 0.f :
                  theme.font->get_text_width(get_line(i).substr(0, get_xpos(caret_1))));
      float rgt = m_right_margin + ((i != line_2) ? theme.font->get_text_width(get_line(i)) :
                  theme.font->get_text_width(get_line(i).substr(0, get_xpos(caret_2))));
      
      context.color().draw_filled_rect(Rectf(m_rect.p1() + Vector(lft + 5.f, hgt),
                                            m_rect.p1() + Vector(rgt + 5.f, hgt + theme.font->get_height())
                                            ),
                                      m_has_focus ? Color(1.f, 1.f, .9f, 0.75f)
                                                  : Color(1.f, 1.f, .9f, 0.5f),
                                      LAYER_GUI);
    }
  }

  draw_text(context);

  if (m_cursor_timer > 0 && m_has_focus) {
    float lgt = theme.font->get_text_width(get_line(get_line_num(m_caret_pos)).substr(0, get_xpos(m_caret_pos)));
    int hpos = m_caret_pos - get_xpos(m_caret_pos) - 1;
    float hgt = (hpos != -1) ?
                  theme.font->get_text_height(get_contents().substr(0, hpos)) :
                  0.f;
    hgt -= m_v_scrollbar.m_progress;
    lgt -= m_h_scrollbar.m_progress - m_right_margin;

    context.color().draw_line(m_rect.p1() + Vector(lgt + 6.f, hgt + 4.f),
                              m_rect.p1() + Vector(lgt + 6.f,
                                  hgt + theme.font->get_height() + 4.f),
                              theme.txt_color,
                              LAYER_GUI + 1);
  }

  context.pop_transform();

  m_v_scrollbar.draw(context);
  m_h_scrollbar.draw(context);
}

void
ControlTextarea::draw_text(DrawingContext& context)
{
  auto theme = get_current_theme();

  context.color().draw_text(theme.font,
                            get_contents(),
                            Vector(m_rect.get_left() + 5.f - m_h_scrollbar.m_progress + m_right_margin,
                                   m_rect.get_top() + 5.f - m_v_scrollbar.m_progress),
                            FontAlignment::ALIGN_LEFT,
                            LAYER_GUI + 1,
                            theme.txt_color);
}

bool
ControlTextarea::on_key_down(const SDL_KeyboardEvent& key)
{
  if (!m_has_focus)
    return false;

  // FIXME: Hardcoded; make CONTROL_CURSOR_TIMER from
  // src/interface/control_textbox.cpp accessible somehow, and use that
  m_cursor_timer = 0.5f;

  if (key.keysym.sym == SDLK_RETURN && (key.keysym.mod & (KMOD_CTRL | KMOD_ALT | KMOD_SHIFT)) == 0)
  {
    put_text("\n");
    reset_scrollbar();
    recenter_offset();
    return true;
  }
  else if (key.keysym.sym == SDLK_UP)
  {
    int line = get_line_num(m_caret_pos);
    if (line == 0)
    {
      m_caret_pos = 0;
    }
    else
    {
      auto font = get_current_theme().font;
      std::string s = get_line(line);
      float x = font->get_text_width(s.substr(0, get_xpos(m_caret_pos)));
      x += 5.f + m_right_margin;
      m_caret_pos = get_offset_from_pos(line - 1, get_pos_from_x(x, line - 1));
    }

    if (!m_shift_pressed)
      m_secondary_caret_pos = m_caret_pos;

    reset_scrollbar();
    recenter_offset();
    return true;
  }
  else if (key.keysym.sym == SDLK_DOWN)
  {
    int line = get_line_num(m_caret_pos);
    if (line >= get_line_num(static_cast<int>(get_contents().size())))
    {
      m_caret_pos = static_cast<int>(get_contents().size());
    }
    else
    {
      auto font = get_current_theme().font;
      std::string s = get_line(line);
      float x = font->get_text_width(s.substr(0, get_xpos(m_caret_pos)));
      x += 5.f + m_right_margin;;
      m_caret_pos = get_offset_from_pos(line + 1, get_pos_from_x(x, line + 1));
    }

    if (!m_shift_pressed)
      m_secondary_caret_pos = m_caret_pos;

    reset_scrollbar();
    recenter_offset();
    return true;
  }
  else if (key.keysym.sym == SDLK_PAGEDOWN)
  {
    auto font = get_current_theme().font;
    int nlines = static_cast<int>(m_rect.get_height() / font->get_height()) - 1;

    int line = get_line_num(m_caret_pos);
    if (line >= get_line_num(static_cast<int>(get_contents().size())) - nlines)
    {
      m_caret_pos = static_cast<int>(get_contents().size());
    }
    else
    {
      std::string s = get_line(line);
      float x = font->get_text_width(s.substr(0, get_xpos(m_caret_pos)));
      x += 5.f + m_right_margin;;
      m_caret_pos = get_offset_from_pos(line + nlines, get_pos_from_x(x, line + nlines));
    }

    if (!m_shift_pressed)
      m_secondary_caret_pos = m_caret_pos;

    reset_scrollbar();
    recenter_offset();
    return true;
  }
  else if (key.keysym.sym == SDLK_PAGEUP)
  {
    auto font = get_current_theme().font;
    int nlines = static_cast<int>(m_rect.get_height() / font->get_height()) - 1;

    int line = get_line_num(m_caret_pos);
    if (line < nlines)
    {
      m_caret_pos = 0;
    }
    else
    {
      std::string s = get_line(line);
      float x = font->get_text_width(s.substr(0, get_xpos(m_caret_pos)));
      x += 5.f + m_right_margin;;
      m_caret_pos = get_offset_from_pos(line - nlines, get_pos_from_x(x, line - nlines));
    }

    if (!m_shift_pressed)
      m_secondary_caret_pos = m_caret_pos;

    reset_scrollbar();
    recenter_offset();
    return true;
  }
  else if (key.keysym.sym == SDLK_HOME)
  {
    m_caret_pos = get_offset_from_pos(get_line_num(m_caret_pos), 0);
    if (!m_shift_pressed)
      m_secondary_caret_pos = m_caret_pos;

    reset_scrollbar();
    recenter_offset();
    return true;
  }
  else if (key.keysym.sym == SDLK_END)
  {
    m_caret_pos = get_offset_from_pos(get_line_num(m_caret_pos) + 1, 0) - 1;
    if (!m_shift_pressed)
      m_secondary_caret_pos = m_caret_pos;

    reset_scrollbar();
    recenter_offset();
    return true;
  }

  reset_scrollbar();
  recenter_offset();
  return ControlTextbox::on_key_down(key);
}

bool
ControlTextarea::on_mouse_wheel(const SDL_MouseWheelEvent& wheel)
{
  if (!m_mouse_hover)
    return false;

  m_v_scrollbar.m_progress -= static_cast<float>(wheel.y * 30);
  m_v_scrollbar.m_progress = math::clamp(m_v_scrollbar.m_progress,
                                       0.f,
                                       std::max(0.f, m_v_scrollbar.m_total_region
                                               - m_v_scrollbar.m_covered_region));

  m_h_scrollbar.m_progress -= static_cast<float>(wheel.x * 30);
  m_h_scrollbar.m_progress = math::clamp(m_h_scrollbar.m_progress,
                                       0.f,
                                       std::max(0.f, m_h_scrollbar.m_total_region
                                               - m_h_scrollbar.m_covered_region));

  return true;
}

int
ControlTextarea::get_text_position(const Vector& pos) const
{
  Vector p = pos - m_rect.p1() + Vector(m_h_scrollbar.m_progress, m_v_scrollbar.m_progress);
  int line = get_line_from_y(p.y);
  return get_offset_from_pos(line, get_pos_from_x(p.x, line));
}

void
ControlTextarea::recenter_offset()
{
  if (m_v_scrollbar.is_valid())
  {
    float y = static_cast<float>(get_line_num(m_caret_pos) + 1) *
                                  get_current_theme().font->get_height() + 5.f;

    m_v_scrollbar.m_progress = math::clamp(m_v_scrollbar.m_progress,
                                           y - m_rect.get_height() + 5.f + 8.f + get_current_theme().font->get_height(),
                                           y - 5.f - get_current_theme().font->get_height());
  }

  if (m_h_scrollbar.is_valid())
  {
    float x = get_current_theme().font->get_text_width(
      get_line(get_line_num(m_caret_pos)).substr(0, get_xpos(m_caret_pos))) + 5.f;

    m_h_scrollbar.m_progress = math::clamp(m_h_scrollbar.m_progress,
                                           x - m_rect.get_width() + 5.f + 8.f,
                                           x - 5.f);
  }
}

void
ControlTextarea::reset_scrollbar()
{
  auto theme = get_current_theme();
  m_v_scrollbar.m_total_region = theme.font->get_text_height(get_contents()) + 10.f;
  m_v_scrollbar.m_covered_region = m_rect.get_height() - theme.font->get_height() - 10.f;
  m_v_scrollbar.m_progress = math::clamp(m_v_scrollbar.m_progress,
                                       0.f,
                                       std::max(0.f, m_v_scrollbar.m_total_region
                                               - m_v_scrollbar.m_covered_region));

  m_h_scrollbar.m_total_region = theme.font->get_text_width(get_contents()) + 10.f;
  m_h_scrollbar.m_covered_region = m_rect.get_width() - 10.f;
  m_h_scrollbar.m_progress = math::clamp(m_h_scrollbar.m_progress,
                                       0.f,
                                       std::max(0.f, m_h_scrollbar.m_total_region
                                               - m_h_scrollbar.m_covered_region));
}

std::string
ControlTextarea::get_line(int line) const
{
  std::string s = get_contents();
  int this_line = get_pos(line);

  if (this_line == -1)
    return "";

  int next_line = get_pos(line + 1);
      next_line = (next_line == -1) ? static_cast<int>(s.size()) : next_line - 1;

  return s.substr(this_line, next_line - this_line);
}

int
ControlTextarea::get_line_num(int pos) const
{
  std::string s = get_contents().substr(0, pos);
  return static_cast<int>(std::count(s.begin(), s.end(), '\n'));
}

int
ControlTextarea::get_pos(int line) const
{
  std::string s = get_contents();
  int pos = -1;

  while (line--)
  {
    pos = static_cast<int>(s.find('\n', pos + 1));
    if (static_cast<size_t>(pos) == std::string::npos)
      return -1;
  }

  // Move the position after the \n
  pos++;

  return pos;
}

int
ControlTextarea::get_xpos(int pos) const
{
  std::string s = get_contents().substr(0, pos);
  return pos - static_cast<int>(s.find_last_of('\n')) - 1;
}

int
ControlTextarea::get_pos_from_x(float x, int line) const
{
  auto font = get_current_theme().font;
  std::string s = get_line(line);
  int xpos = 0;

  x -= 5.f + m_right_margin;

  for (int i = 1; i <= static_cast<int>(s.size()); i++)
  {
    float d1 = font->get_text_width(s.substr(0, xpos)),
          d2 = font->get_text_width(s.substr(0, i));

    if (std::abs(x - d1) < std::abs(x - d2))
      return xpos;

    xpos = i;
  }

  return xpos;
}

int
ControlTextarea::get_line_from_y(float y) const
{
  int line = static_cast<int>((y - 5.f) / get_current_theme().font->get_height());
  
  return std::min(get_line_num(static_cast<int>(get_contents().size())), line);
}

int
ControlTextarea::get_offset_from_pos(int line, int xpos) const
{
  int offset = 0;

  for (int i = 0; i < line; i++)
  {
    offset += static_cast<int>(get_line(i).size()) + 1;
  }

  return offset + xpos;
}

/* EOF */
