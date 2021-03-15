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

#include "editor/canvas_editor.hpp"

#include "gui/mousecursor.hpp"
#include "supertux/globals.hpp"
#include "supertux/resources.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/screen_fade.hpp"
#include "supertux/sector.hpp"
#include "video/compositor.hpp"
#include "video/viewport.hpp"
#include "video/video_system.hpp"

CanvasEditor::CanvasEditor(Editor& editor, Sector& sector) :
  m_editor(editor),
  m_sector(sector),
  m_panels(),
  m_colormap_panel({_("Normal"), 150.f, 150.f, Resources::control_font->get_text_width(_("Normal")) + 10.f, Resources::control_font->get_height() + 4.f}),
  m_lightmap_panel({_("Lightmap"), 150.f, 200.f, Resources::control_font->get_text_width(_("Lightmap")) + 10.f, Resources::control_font->get_height() + 4.f}),
  m_mouse_pos(),
  m_selected_panels(),
  m_dragging(false)
{
  float hgt = 10.f;

  for (const auto& r : m_sector.m_canvases)
  {
    if (std::any_of(m_panels.begin(), m_panels.end(), [r](const CanvasPanel& c) {
                                                        return c.name == r.src_name;
                                                      }))
      continue;

    CanvasPanel c;

    c.name = r.src_name;

    c.x = 10.f;
    c.y = 10.f + hgt;
    c.w = Resources::control_font->get_text_width(r.src_name) + 10.f;
    c.h = Resources::control_font->get_height() + 4.f;

    hgt += c.h + 10.f;

    m_panels.push_back(c);
  }
}

void
CanvasEditor::draw(Compositor& compositor)
{
  auto& context = compositor.make_context();

  MouseCursor::current()->draw(context);

  context.color().draw_filled_rect(Rectf(0, 0, context.get_width(), context.get_height()),
                                   Color(.8f, .8f, .8f),
                                   LAYER_GUI - 10);

  for (const auto& p : m_panels)
  {
    context.color().draw_filled_rect(Rectf(p.x, p.y, p.x + p.w, p.y + p.h), Color(.15f, .15f, .15f), LAYER_GUI);
    context.color().draw_text(Resources::control_font, p.name, Vector(p.x + 5.f, p.y + 2.f), FontAlignment::ALIGN_LEFT, LAYER_GUI);
  }

  for (auto* p : m_selected_panels)
  {
    context.color().draw_filled_rect(Rectf(p->x, p->y, p->x + p->w, p->y + p->h).grown(2.f), Color::WHITE, LAYER_GUI - 1);
  }

  int i = 1;
  for (const auto& link : m_sector.m_canvases)
  {
    CanvasPanel* src, * dst;
  
    switch (link.src_target)
    {
      case DrawingTarget::COLORMAP:
        src = &m_colormap_panel;
        break;
      case DrawingTarget::LIGHTMAP:
        src = &m_lightmap_panel;
        break;
      case DrawingTarget::USER:
      {
        auto src_it = std::find_if(m_panels.begin(), m_panels.end(), [link](const CanvasPanel& panel){
          return panel.name == link.src_name;
        });
        if (src_it == m_panels.end())
        {
          throw std::runtime_error("Couldn't find source canvas '" + link.src_name + "'");
        }
        src = src_it.base();
        break;
      }
      default:
        throw std::runtime_error("Invalid target");
    }

    switch (link.dst_target)
    {
      case DrawingTarget::COLORMAP:
        dst = &m_colormap_panel;
        break;
      case DrawingTarget::LIGHTMAP:
        dst = &m_lightmap_panel;
        break;
      case DrawingTarget::USER:
      {
        auto dst_it = std::find_if(m_panels.begin(), m_panels.end(), [link](const CanvasPanel& panel){
          return panel.name == link.dst_name;
        });
        if (dst_it == m_panels.end())
        {
          throw std::runtime_error("Couldn't find destination canvas '" + link.dst_name + "'");
        }
        dst = dst_it.base();
        break;
      }
      default:
        throw std::runtime_error("Invalid target");
    }

    draw_arrow(context, *src, *dst, std::to_string(i));
  }

  context.color().draw_filled_rect(Rectf(m_colormap_panel.x, m_colormap_panel.y,
                                         m_colormap_panel.x + m_colormap_panel.w,
                                         m_colormap_panel.y + m_colormap_panel.h),
                                   Color(.3f, .3f, .3f),
                                   LAYER_GUI);
  context.color().draw_text(Resources::control_font, m_colormap_panel.name,
                            Vector(m_colormap_panel.x + 5.f, m_colormap_panel.y + 2.f),
                            FontAlignment::ALIGN_LEFT, LAYER_GUI,
                            Color(.7f, .7f, .7f));

  context.color().draw_filled_rect(Rectf(m_lightmap_panel.x, m_lightmap_panel.y,
                                         m_lightmap_panel.x + m_lightmap_panel.w,
                                         m_lightmap_panel.y + m_lightmap_panel.h),
                                   Color(.3f, .3f, .3f),
                                   LAYER_GUI);
  context.color().draw_text(Resources::control_font, m_lightmap_panel.name,
                            Vector(m_lightmap_panel.x + 5.f, m_lightmap_panel.y + 2.f),
                            FontAlignment::ALIGN_LEFT, LAYER_GUI,
                            Color(.7f, .7f, .7f));

  context.color().draw_line(Vector(m_colormap_panel.x + m_colormap_panel.w / 2.f,
                                   m_colormap_panel.y + m_colormap_panel.h / 2.f),
                            Vector(m_lightmap_panel.x + m_lightmap_panel.w / 2.f,
                                   m_lightmap_panel.y + m_lightmap_panel.h / 2.f),
                            Color(.3f, .3f, .3f),
                            LAYER_GUI - 2);
  // TODO: Draw the triangle "head" of the arrow
}

void
CanvasEditor::update(float dt_sec, const Controller& controller)
{
}

void
CanvasEditor::event(const SDL_Event& ev)
{
  if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE)
  {
    ScreenManager::current()->pop_screen();
  }
  else if (ev.type == SDL_MOUSEBUTTONDOWN && ev.button.button == SDL_BUTTON_LEFT)
  {
    m_dragging = true;

    m_mouse_pos = VideoSystem::current()->get_viewport().to_logical(ev.button.x, ev.button.y);

    CanvasPanel* target = nullptr;

    if (Rectf(m_colormap_panel.x, m_colormap_panel.y, m_colormap_panel.x +
                    m_colormap_panel.w, m_colormap_panel.y + m_colormap_panel.h)
                    .contains(m_mouse_pos))
      target = &m_colormap_panel;

    if (Rectf(m_lightmap_panel.x, m_lightmap_panel.y, m_lightmap_panel.x +
                    m_lightmap_panel.w, m_lightmap_panel.y + m_lightmap_panel.h)
                    .contains(m_mouse_pos))
      target = &m_lightmap_panel;

    for (auto& panel : m_panels)
      if (Rectf(panel.x, panel.y, panel.x + panel.w, panel.y + panel.h).contains(m_mouse_pos))
        target = &panel;

    if (!(SDL_GetModState() & KMOD_CTRL) && !panel_selected(target))
        m_selected_panels.clear();

    if (target && !panel_selected(target))
      toggle_panel(target);
  }
  else if (ev.type == SDL_MOUSEBUTTONUP && ev.button.button == SDL_BUTTON_LEFT)
  {
    m_dragging = false;
  }
  else if (ev.type == SDL_MOUSEMOTION && m_dragging)
  {
    Vector pos = VideoSystem::current()->get_viewport().to_logical(ev.motion.x, ev.motion.y);
    Vector delta = pos - m_mouse_pos;
    m_mouse_pos = pos;

    // Prevent user from scrolling too far away
    if (std::none_of(m_panels.begin(), m_panels.end(), [](const CanvasPanel& panel){
      return panel.x < SCREEN_WIDTH;
    }) && m_colormap_panel.x >= SCREEN_WIDTH && m_lightmap_panel.x >= SCREEN_WIDTH && delta.x > 0)
      delta.x = 0;

    if (std::none_of(m_panels.begin(), m_panels.end(), [](const CanvasPanel& panel){
      return panel.x + panel.w > 0;
    }) && m_colormap_panel.x + m_colormap_panel.w <= 0 && m_lightmap_panel.x + m_lightmap_panel.w <= 0  && delta.x < 0)
      delta.x = 0;

    if (std::none_of(m_panels.begin(), m_panels.end(), [](const CanvasPanel& panel){
      return panel.y < SCREEN_HEIGHT;
    }) && m_colormap_panel.y >= SCREEN_HEIGHT && m_lightmap_panel.y >= SCREEN_HEIGHT && delta.y > 0)
      delta.y = 0;

    if (std::none_of(m_panels.begin(), m_panels.end(), [](const CanvasPanel& panel){
      return panel.y + panel.h > 0;
    }) && m_colormap_panel.y + m_colormap_panel.h <= 0 && m_lightmap_panel.y + m_lightmap_panel.h <= 0 && delta.y < 0)
      delta.y = 0;


    if (!m_selected_panels.empty())
    {
      for (auto* panel : m_selected_panels)
      {
        panel->x += delta.x;
        panel->y += delta.y;
      }
    }
    else
    {
      for (auto& panel : m_panels)
      {
        panel.x += delta.x;
        panel.y += delta.y;
      }
      m_lightmap_panel.x += delta.x;
      m_lightmap_panel.y += delta.y;
      m_colormap_panel.x += delta.x;
      m_colormap_panel.y += delta.y;
    }
  }
}

IntegrationStatus
CanvasEditor::get_status() const
{
  return m_editor.get_status();
}

bool
CanvasEditor::toggle_panel(CanvasPanel* panel)
{
  for (auto it = m_selected_panels.begin(); it != m_selected_panels.end(); it++)
  {
    if (*it == panel)
    {
      m_selected_panels.erase(it);
      return false;
    }
  }

  m_selected_panels.push_back(panel);
  return true;
}

void
CanvasEditor::draw_arrow(DrawingContext& context, CanvasPanel& src, CanvasPanel& dst, std::string label)
{
  Vector pt_src = Vector(src.x + src.w / 2.f,
                         src.y + src.h / 2.f),
         pt_dst = Vector(dst.x + dst.w / 2.f,
                         dst.y + dst.h / 2.f);

  context.color().draw_line(pt_src, pt_dst, Color::BLACK, LAYER_GUI - 2);
  context.color().draw_text(Resources::border_control_font,
                            label,
                            (pt_src + pt_dst) / 2.f - Vector(0, Resources::border_control_font->get_height() / 2.f),
                            FontAlignment::ALIGN_CENTER,
                            LAYER_GUI);

  auto pts = Rectf(dst.x, dst.y, dst.x + dst.w, dst.y + dst.h).clip_line(pt_src, pt_dst);

  // draw a triangle
  Vector dir = (Vector(dst.x + dst.w / 2.f, dst.y + dst.h / 2.f) -
                  Vector(src.x + src.w / 2.f, src.y + src.h / 2.f)).unit() * 8;
  Vector dir2 = Vector(-dir.y, dir.x);
  Vector pos = std::get<0>(pts) - dir;
  context.color().draw_triangle(pos + dir * 1.5, pos - dir + dir2, pos - dir - dir2,
                                  Color::BLACK, LAYER_GUI - 2);
}

/* EOF */
