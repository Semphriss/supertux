//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include "video/compositor.hpp"

#include "math/rect.hpp"
#include "util/log.hpp"
#include "video/drawing_request.hpp"
#include "video/drawing_target.hpp"
#include "video/painter.hpp"
#include "video/renderer.hpp"
#include "video/video_system.hpp"

bool Compositor::s_render_lighting = true;

Compositor::Compositor(VideoSystem& video_system) :
  m_video_system(video_system),
  m_obst(),
  m_drawing_contexts()
{
  obstack_init(&m_obst);
}

Compositor::~Compositor()
{
  m_drawing_contexts.clear();
  obstack_free(&m_obst, nullptr);
}

DrawingContext&
Compositor::make_context(bool overlay)
{
  m_drawing_contexts.emplace_back(new DrawingContext(m_video_system, m_obst, overlay));
  return *m_drawing_contexts.back();
}

std::pair<std::vector<std::unique_ptr<TextureRequest>>,
          std::vector<std::unique_ptr<TextureRequest>>>
Compositor::paint_user_canvases()
{
  std::pair<std::vector<std::unique_ptr<TextureRequest>>,
            std::vector<std::unique_ptr<TextureRequest>>> requests;

  // Draw each canvas on their own renderer
  for (const std::string& canvas_name : m_video_system.get_user_renderers())
  {
    auto* renderer = m_video_system.get_user_renderer(canvas_name);

    assert(renderer);

    renderer->start_draw();
    Painter& painter = renderer->get_painter();

    // Forces the background to be invisible, not solid black. This is already
    // done in GLTextureRenderer (not in GLScreenRenderer though), but it's done
    // again for good practice.
    painter.clear(Color::INVISIBLE);

    for (auto& ctx : m_drawing_contexts)
    {
      if (!ctx->has_user_canvas(canvas_name))
        continue;

      painter.set_clip_rect(ctx->get_viewport());
      ctx->get_canvas(DrawingTarget::USER, canvas_name).render(*renderer, Canvas::BELOW_LIGHTMAP);
      painter.clear_clip_rect();
    }

    renderer->end_draw();
  }

  // Mix each canvas through linking data
  for (const auto& link : m_video_system.get_user_renderers_data())
  {
    // FIXME: Lightmap/Colormap canvases cannot be read from here (segfault when getting texture)
    if (link.src_target != DrawingTarget::USER)
      continue;

    auto* renderer_src = m_video_system.get_renderer(link.src_target, link.src_name);
    assert(renderer_src);

    const auto& texture = renderer_src->get_texture();

    auto request = std::make_unique<TextureRequest>();

    request->type = TEXTURE;
    request->flip = link.flip;
    request->alpha = link.alpha;
    request->blend = link.blend;

    request->srcrects.emplace_back(0, 0,
                                  static_cast<float>(texture->get_image_width()),
                                  static_cast<float>(texture->get_image_height()));
    request->dstrects.emplace_back(Vector(0, 0), renderer_src->get_logical_size());
    request->angles.emplace_back(0.0f);

    request->texture = texture.get();
    request->color = link.color_mult;

    if (link.dst_target == DrawingTarget::USER)
    {
      auto* renderer_dst = m_video_system.get_renderer(link.dst_target, link.dst_name);
      assert(renderer_dst);

      renderer_dst->start_draw();
      auto& painter = renderer_dst->get_painter();
      painter.draw_texture(*request);
      renderer_dst->end_draw();
    }
    else if (link.dst_target == DrawingTarget::COLORMAP)
    {
      requests.first.push_back(std::move(request));
    }
    else if (link.dst_target == DrawingTarget::LIGHTMAP)
    {
      requests.second.push_back(std::move(request));
    }
    else
    {
      log_fatal << "DrawingTarget enum incomplete" << std::endl;
      throw std::runtime_error("DrawingTarget enum incomplete");
    }
  }

  return requests;
}

void
Compositor::render()
{
  auto& lightmap = m_video_system.get_lightmap();

  bool use_lightmap = s_render_lighting &&
              std::any_of(m_drawing_contexts.begin(), m_drawing_contexts.end(),
                                  [](std::unique_ptr<DrawingContext>& ctx){
                                    return ctx->use_lightmap();
                                  });

  const auto user_canvas_requests = paint_user_canvases();

  // prepare lightmap
  if (use_lightmap)
  {
    lightmap.start_draw();
    Painter& painter = lightmap.get_painter();

    for (auto& ctx : m_drawing_contexts)
    {
      if (!ctx->is_overlay())
      {
        painter.set_clip_rect(ctx->get_viewport());
        painter.clear(ctx->get_ambient_color());

        ctx->light().render(lightmap, Canvas::ALL);

        painter.clear_clip_rect();
      }
    }

    for (const auto& req : user_canvas_requests.second)
      painter.draw_texture(*req);

    lightmap.end_draw();
  }

  auto back_renderer = m_video_system.get_back_renderer();
  if (back_renderer)
  {
    back_renderer->start_draw();

    Painter& painter = back_renderer->get_painter();

    for (auto& ctx : m_drawing_contexts)
    {
      painter.set_clip_rect(ctx->get_viewport());
      ctx->color().render(*back_renderer, Canvas::BELOW_LIGHTMAP);
      painter.clear_clip_rect();
    }

    back_renderer->end_draw();
  }

  // compose the screen
  {
    auto& renderer = m_video_system.get_renderer();

    renderer.start_draw();
    Painter& painter = renderer.get_painter();

    for (auto& ctx : m_drawing_contexts)
    {
      painter.set_clip_rect(ctx->get_viewport());
      ctx->color().render(renderer, Canvas::BELOW_LIGHTMAP);
      painter.clear_clip_rect();
    }

    for (const auto& req : user_canvas_requests.first)
      painter.draw_texture(*req);

    if (use_lightmap)
    {
      TexturePtr texture = lightmap.get_texture();

      if (texture)
      {
        TextureRequest request;

        request.type = TEXTURE;
        request.flip = 0;
        request.alpha = 1.0f;
        request.blend = Blend::MOD;

        request.srcrects.emplace_back(0, 0,
                                      static_cast<float>(texture->get_image_width()),
                                      static_cast<float>(texture->get_image_height()));
        request.dstrects.emplace_back(Vector(0, 0), lightmap.get_logical_size());
        request.angles.emplace_back(0.0f);

        request.texture = texture.get();
        request.color = Color::WHITE;

        painter.draw_texture(request);
      }
    }

    // Render overlay elements
    for (auto& ctx : m_drawing_contexts)
    {
      painter.set_clip_rect(ctx->get_viewport());
      ctx->color().render(renderer, Canvas::ABOVE_LIGHTMAP);
      painter.clear_clip_rect();
    }

    renderer.end_draw();
  }

  // cleanup
  for (auto& ctx : m_drawing_contexts)
  {
    ctx->clear();
  }
  m_video_system.flip();

  obstack_free(&m_obst, nullptr);
  obstack_init(&m_obst);
}

/* EOF */
