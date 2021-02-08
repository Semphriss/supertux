//  SuperTux
//  Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
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

#include "editor/layers_widget.hpp"

#include "editor/editor.hpp"
#include "editor/layer_icon.hpp"
#include "editor/object_menu.hpp"
#include "editor/tip.hpp"
#include "gui/menu_manager.hpp"
#include "math/vector.hpp"
#include "object/camera.hpp"
#include "object/path_gameobject.hpp"
#include "object/tilemap.hpp"
#include "supertux/colorscheme.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/moving_object.hpp"
#include "supertux/resources.hpp"
#include "supertux/sector.hpp"
#include "video/drawing_context.hpp"
#include "video/renderer.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

EditorLayersWidget::EditorLayersWidget(Editor& editor) :
  m_editor(editor),
  m_layer_icons(),
  m_selected_tilemap(),
  m_Ypos(448),
  m_Width(512),
  m_hovered_item(HoveredItem::NONE),
  m_hovered_layer(-1),
  m_object_tip(),
  m_has_mouse_focus(false),
  m_scrollbar(),
  m_right_margin(192)
{
  m_scrollbar.set_rect(Rect(0, SCREEN_HEIGHT - 6,
                            SCREEN_WIDTH, SCREEN_HEIGHT));
  m_scrollbar.m_horizontal = true;
  m_scrollbar.m_theme = InterfaceThemeSet(
    InterfaceTheme(Resources::control_font, Color(.7f, .7f, .7f, 1.f), Color::BLACK, 0.f), // base
    InterfaceTheme(Resources::control_font, Color(.8f, .8f, .8f, 1.f), Color::BLACK, 0.f), // hover
    InterfaceTheme(Resources::control_font, Color(1.f, 1.f, 1.f, 1.f), Color::BLACK, 0.f), // active
    InterfaceTheme(Resources::control_font, Color(.9f, .9f, .9f, 1.f), Color::BLACK, 0.f), // focused
    InterfaceTheme(Resources::control_font, Color(.3f, .3f, .3f, 1.f), Color::BLACK, 0.f) // disabled
  );
  update_scrollbar();
}

void
EditorLayersWidget::draw(DrawingContext& context)
{
  context.push_transform();
  context.transform().clip = Rect(0, m_Ypos, SCREEN_WIDTH - m_right_margin, SCREEN_HEIGHT);

  if (m_object_tip) {
    auto position = get_layer_coords(m_hovered_layer);
    m_object_tip->draw_up(context, position);
  }

  context.color().draw_filled_rect(Rectf(Vector(0, static_cast<float>(m_Ypos)),
                                         Vector(static_cast<float>(m_Width), static_cast<float>(SCREEN_HEIGHT))),
                                   Color::BLACK,
                                   0.0f,
                                   LAYER_GUI - 10);

  Rectf target_rect = Rectf(0, 0, 0, 0);
  bool draw_rect = true;

  switch (m_hovered_item)
  {
    case HoveredItem::LAYERS:
      {
        Vector coords = get_layer_coords(m_hovered_layer);
        target_rect = Rectf(coords, coords + Vector(32, 32));
      }
      break;

    default:
      draw_rect = false;
      break;
  }

  if (draw_rect)
  {
    context.color().draw_filled_rect(target_rect, Color(0.9f, 0.9f, 1.0f, 0.6f), 0.0f,
                                       LAYER_GUI-5);
  }

  if (!m_editor.is_level_loaded()) {
    return;
  }

  int pos = 0;
  for (const auto& layer_icon : m_layer_icons) {
    if (layer_icon->is_valid()) {
      layer_icon->draw(context, get_layer_coords(pos));
    }
    pos++;
  }

  m_scrollbar.draw(context);

  context.pop_transform();
}

void
EditorLayersWidget::update(float dt_sec)
{
  auto it = m_layer_icons.begin();
  while (it != m_layer_icons.end())
  {
    auto layer_icon = (*it).get();
    if (!layer_icon->is_valid())
      it = m_layer_icons.erase(it);
    else
      ++it;
  }

  update_scrollbar();
}

bool
EditorLayersWidget::event(const SDL_Event& event)
{
  if (!m_scrollbar.event(event))
  {
    return Widget::event(event);
  }
  else
  {
    return true;
  }
}

bool
EditorLayersWidget::on_mouse_button_up(const SDL_MouseButtonEvent& button)
{
  return false;
}

bool
EditorLayersWidget::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{
  if (button.button == SDL_BUTTON_LEFT)
  {
    switch (m_hovered_item)
    {
      case HoveredItem::LAYERS:
        if (m_hovered_layer >= m_layer_icons.size())
          return false;
        
        if (m_layer_icons[m_hovered_layer]->is_tilemap()) {
          if (m_selected_tilemap) {
            m_selected_tilemap->m_editor_active = false;
          }
          m_selected_tilemap = static_cast<TileMap*>(m_layer_icons[m_hovered_layer]->get_layer());
          m_selected_tilemap->m_editor_active = true;
          m_editor.edit_path(m_selected_tilemap->get_path(), m_selected_tilemap);
        } else {
          auto cam = dynamic_cast<Camera*>(m_layer_icons[m_hovered_layer]->get_layer());
          if (cam) {
            m_editor.edit_path(cam->get_path(), cam);
          }
        }

        m_editor.m_settings_widget->set_object(m_layer_icons[m_hovered_layer]->get_layer());

        return true;

      default:
        return false;
    }
  }
  else if (button.button == SDL_BUTTON_RIGHT)
  {
    switch (m_hovered_item)
    {
      case HoveredItem::LAYERS:

        if (m_hovered_layer >= m_layer_icons.size())
          return false;

        {
          auto h = &(m_layer_icons[m_hovered_layer]->get_layer()->m_editor_hidden);
          *h = !*h;
        }

        return true;

      default:
        return false;
    }
  }
  else
  {
    return false;
  }
}

bool
EditorLayersWidget::on_mouse_motion(const SDL_MouseMotionEvent& motion)
{
  Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(motion.x, motion.y);
  float x = mouse_pos.x;
  float y = mouse_pos.y - static_cast<float>(m_Ypos);
  if (y < 0 || x > static_cast<float>(m_Width)) {
    m_hovered_item = HoveredItem::NONE;
    m_object_tip = nullptr;
    m_has_mouse_focus = false;
    return false;
  }

  m_has_mouse_focus = true;

  unsigned int new_hovered_layer = get_layer_pos(mouse_pos);
  if (m_hovered_layer != new_hovered_layer || m_hovered_item != HoveredItem::LAYERS) {
    m_hovered_layer = new_hovered_layer;
    update_tip();
  }

  m_hovered_item = HoveredItem::LAYERS;

  return true;
}


bool
EditorLayersWidget::on_mouse_wheel(const SDL_MouseWheelEvent& wheel)
{
  return false;
}

bool
EditorLayersWidget::has_mouse_focus() const
{
  return m_has_mouse_focus;
}

void
EditorLayersWidget::resize()
{
  m_Ypos = SCREEN_HEIGHT - 32;
  m_Width = SCREEN_WIDTH - m_right_margin;

  m_scrollbar.set_rect(Rect(0, SCREEN_HEIGHT - 6,
                            SCREEN_WIDTH - m_right_margin, SCREEN_HEIGHT));

  update_scrollbar();
}

void
EditorLayersWidget::setup()
{
  resize();
}

void
EditorLayersWidget::refresh()
{
  m_selected_tilemap = nullptr;
  m_layer_icons.clear();

  bool tsel = false;
  for (auto& i : m_editor.get_sector()->get_objects())
  {
    auto* go = i.get();
    auto* mo = dynamic_cast<MovingObject*>(go);
    if (!mo && go->has_settings()) {
      if (!dynamic_cast<PathGameObject*>(go)) {
        add_layer(go);
      }

      auto tm = dynamic_cast<TileMap*>(go);
      if (tm) {
        if ( !tm->is_solid() || tsel ) {
          tm->m_editor_active = false;
        } else {
          m_selected_tilemap = tm;
          tm->m_editor_active = true;
          tsel = true;
        }
      }
    }
  }

  sort_layers();
}

void
EditorLayersWidget::sort_layers()
{
  std::sort(m_layer_icons.begin(), m_layer_icons.end(),
            [](const std::unique_ptr<LayerIcon>& lhs, const std::unique_ptr<LayerIcon>& rhs) {
              return lhs->get_zpos() < rhs->get_zpos();
            });

  update_scrollbar();
}

void
EditorLayersWidget::add_layer(GameObject* layer)
{
  auto icon = std::make_unique<LayerIcon>(layer);
  int z_pos = icon->get_zpos();

  // The icon is inserted to the correct position.
  for (auto i = m_layer_icons.begin(); i != m_layer_icons.end(); ++i) {
    const auto& li = i->get();
    if (li->get_zpos() < z_pos) {
      m_layer_icons.insert(i, move(icon));
      return;
    }
  }

  m_layer_icons.push_back(move(icon));

  update_scrollbar();
}

void
EditorLayersWidget::update_tip()
{
  if ( m_hovered_layer >= m_layer_icons.size() ) {
    m_object_tip = nullptr;
    return;
  }
  m_object_tip = std::make_unique<Tip>(*m_layer_icons[m_hovered_layer]->get_layer());
}

Vector
EditorLayersWidget::get_layer_coords(const int pos) const
{
  return Vector(static_cast<float>(pos * 32) - m_scrollbar.m_progress,
                static_cast<float>(m_Ypos));
}

int
EditorLayersWidget::get_layer_pos(const Vector& coords) const
{
  return static_cast<int>((coords.x + m_scrollbar.m_progress) / 32.0f);
}

void
EditorLayersWidget::update_scrollbar()
{
  m_scrollbar.m_total_region = static_cast<float>(m_layer_icons.size()) * 32.f;
  m_scrollbar.m_covered_region = static_cast<float>(SCREEN_WIDTH - m_right_margin);
  m_scrollbar.m_progress = math::clamp(m_scrollbar.m_progress, 0.f, m_scrollbar.m_total_region - m_scrollbar.m_covered_region);

  if (!m_scrollbar.is_valid())
    m_scrollbar.m_progress = 0.f;
}
/* EOF */
