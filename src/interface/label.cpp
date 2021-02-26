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

#include "interface/label.hpp"

#include "supertux/resources.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

#include <iostream>

InterfaceLabel::InterfaceLabel() :
  InterfaceControl(),
  m_multiline(false),
  m_v_align(VAlign::TOP),
  m_h_align(FontAlignment::ALIGN_LEFT),
  m_padding(5.f),
  m_label(),
  m_mouse_pos()
{
}

InterfaceLabel::InterfaceLabel(const Rectf& rect, const std::string& label) :
  InterfaceControl(),
  m_multiline(false),
  m_v_align(VAlign::TOP),
  m_h_align(FontAlignment::ALIGN_LEFT),
  m_padding(5.f),
  m_label(std::move(label)),
  m_mouse_pos()
{
  m_rect = rect;
}

void
InterfaceLabel::draw(DrawingContext& context)
{
  const auto& theme = get_current_theme();

  float hpos = 0.f;

  // FIXME: Should be a switch-case but I'm too lazy.
  if (m_h_align != FontAlignment::ALIGN_LEFT)
    hpos = m_rect.get_width() - m_padding * 2.f;
  if (m_h_align == FontAlignment::ALIGN_CENTER)
    hpos /= 2.f;

  hpos += m_rect.get_left() + m_padding;

  if (m_multiline)
  {
    const auto lines = get_lines();

    // height
    float h = 0.f;
    
    // FIXME: Should be a switch-case but I'm too lazy.
    if (m_v_align != VAlign::TOP)
      h = get_remainder_height(lines.size()) - m_padding * 2.f;
    if (m_v_align == VAlign::MIDDLE)
      h /= 2.f;

    h += m_rect.get_top() + m_padding;

    for (const auto& l : lines)
    {
      context.color().draw_text(theme.font,
                                l,
                                Vector(hpos, h),
                                m_h_align,
                                LAYER_GUI,
                                theme.txt_color);
      h += theme.font->get_height();
    }
  }
  else
  {
    context.color().draw_text(Resources::control_font,
                              get_truncated_text(),
                              Vector(hpos,
                                    (m_rect.get_top() + m_rect.get_bottom()) / 2 -
                                      Resources::control_font->get_height() / 2 + 1.f),
                              m_h_align,
                              LAYER_GUI,
                              Color::WHITE);

    if (!fits(m_label) && m_rect.contains(m_mouse_pos)) {
      context.push_transform();
      context.transform().clip = Rect(0, 0, context.get_width(), context.get_height());
      draw_tooltip(context, m_label);
      context.pop_transform();
    }
  }
}

bool
InterfaceLabel::fits(const std::string& text) const
{
  return Resources::control_font->get_text_width(text) <= m_rect.get_width() - m_padding * 2.f;
}

std::string
InterfaceLabel::get_truncated_text() const
{
  if (fits(m_label)) return m_label;

  std::string temp = m_label;
  while (!temp.empty() && !fits(temp + "..."))
    temp.pop_back();

  return temp + "...";
}

std::vector<std::string>
InterfaceLabel::get_lines() const
{
  if (fits(m_label))
    return {m_label};

  FontPtr font = get_current_theme().font;

  std::vector<std::string> lines;
  int index = 0;
  int line = 0;

  // cache
  const int max_lines = get_num_lines();

  // TODO: Searching for a fits() that work is linear - multiplied by each line.
  //       Make it so it does a binary search (complexity log n).
  while (index < m_label.size() && ++line <= max_lines)
  {
    std::string temp = m_label.substr(index);
    int pos = 0;
    if (line == max_lines)
    {
      while (temp.size() > pos && fits(temp.substr(0, pos) + "..."))
        pos++;

      if (!fits(temp.substr(0, pos) + "...") && temp.size() > pos)
        pos--;

      lines.push_back(temp + ((temp.size() > pos) ? "..." : ""));
    }
    else
    {
      int last_breakable = 0;

      while (temp.size() > pos && fits(temp.substr(0, pos)))
      {
        if (breakable_chars.find(temp.at(pos)) != std::string::npos)
          last_breakable = pos;
        pos++;
      }

      // Break between two words if possible
      if (last_breakable != 0 && pos != temp.size())
        pos = last_breakable;

      lines.push_back(temp.substr(0, pos));
    }

    index += pos;

    // Skip spaces at the end of lines
    while(index < m_label.size() && m_label.at(index) == ' ')
      index++;
  }

  return lines;
}

int
InterfaceLabel::get_num_lines() const
{
  FontPtr font = get_current_theme().font;
  return static_cast<int>(std::floor((m_rect.get_height() - m_padding * 2.f) / font->get_height()));
}

float
InterfaceLabel::get_remainder_height_total() const
{
  FontPtr font = get_current_theme().font;
  return std::fmod(m_rect.get_height() - m_padding * 2.f, font->get_height());
}

float
InterfaceLabel::get_remainder_height(int lines) const
{
  FontPtr font = get_current_theme().font;
  return get_remainder_height_total() +
            static_cast<float>(get_num_lines() - lines) * font->get_height();
}

void
InterfaceLabel::autoresize_bottom()
{
  FontPtr font = get_current_theme().font;

  int index = 0;
  int line = 0;

  // TODO: Searching for a fits() that work is linear - multiplied by each line.
  //       Make it so it does a binary search (complexity log n).
  while (index < m_label.size())
  {
    std::string temp = m_label.substr(index);
    int pos = 1;
    int last_breakable = 0;

    while (temp.size() > pos && fits(temp.substr(0, pos)))
    {
      if (breakable_chars.find(temp.at(pos)) != std::string::npos)
        last_breakable = pos;
      pos++;
    }

    //std::cout << last_breakable << std::endl;

    // Break between two words if possible
    if (last_breakable != 0 && pos != temp.size())
      pos = last_breakable;

    index += pos;

    // Skip spaces at the end of lines
    while(index < m_label.size() && m_label.at(index) == ' ')
      index++;

    line++;
  }

  m_rect.set_height(font->get_height() * static_cast<float>(line) + 10.f);
}

std::string
InterfaceLabel::find_fitting(std::string s, std::function<bool(std::string)> f) const
{
  int upper = s.size(), lower = 0, current;
  while (upper > lower + 1)
  {
    current = (upper + lower) / 2;
    if (f(s.substr(current)))
    {
      lower = current;
    }
    else
    {
      upper = current;
    }
  }

  return s.substr(lower);
}

/* EOF */
