//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#include "badguy/sspiky.hpp"

#include "object/player.hpp"
#include "sprite/sprite.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"

SSpiky::SSpiky(const ReaderMapping& reader) :
  WalkingBadguy(reader, "images/creatures/spiky/sleepingspiky.sprite", "left", "right"), state(SSPIKY_SLEEPING)
{
  walk_speed = 80;
  max_drop_height = 600;
}

void
SSpiky::initialize()
{
  state = SSPIKY_SLEEPING;
  m_physic.set_velocity_x(0);
  m_sprite->set_action(m_dir == Direction::LEFT ? "sleeping-left" : "sleeping-right");
}

void
SSpiky::collision_solid(const CollisionHit& hit)
{
  if (state != SSPIKY_WALKING) {
    BadGuy::collision_solid(hit);
    return;
  }
  WalkingBadguy::collision_solid(hit);
}

HitResponse
SSpiky::collision_badguy(BadGuy& badguy, const CollisionHit& hit)
{
  if (state != SSPIKY_WALKING) {
    return BadGuy::collision_badguy(badguy, hit);
  }
  return WalkingBadguy::collision_badguy(badguy, hit);
}

void
SSpiky::active_update(float dt_sec) {

  if (state == SSPIKY_WALKING) {
    WalkingBadguy::active_update(dt_sec);
    return;
  }

  if (state == SSPIKY_SLEEPING) {

    Player* player = get_nearest_player();
    if (player) {
      Rectf pb = player->get_bbox();

      bool inReach_left = (pb.get_right() >= m_col.m_bbox.get_right()-((m_dir == Direction::LEFT) ? 256 : 0));
      bool inReach_right = (pb.get_left() <= m_col.m_bbox.get_left()+((m_dir == Direction::RIGHT) ? 256 : 0));
      bool inReach_top = (pb.get_bottom() >= m_col.m_bbox.get_top());
      bool inReach_bottom = (pb.get_top() <= m_col.m_bbox.get_bottom());

      if (inReach_left && inReach_right && inReach_top && inReach_bottom) {
        // wake up
        m_sprite->set_action(m_dir == Direction::LEFT ? "waking-left" : "waking-right", 1);
        state = SSPIKY_WAKING;
      }
    }

    BadGuy::active_update(dt_sec);
  }

  if (state == SSPIKY_WAKING) {
    if (m_sprite->animation_done()) {
      // start walking
      state = SSPIKY_WALKING;
      WalkingBadguy::initialize();
    }

    BadGuy::active_update(dt_sec);
  }
}

void
SSpiky::freeze()
{
  WalkingBadguy::freeze();
  state = SSPIKY_WALKING; // if we get hit while sleeping, wake up :)
}

bool
SSpiky::is_freezable() const
{
  return true;
}

bool
SSpiky::is_flammable() const
{
  return state != SSPIKY_SLEEPING;
}

void
SSpiky::backup(Writer& writer) const
{
  BadGuy::backup(writer);

  writer.start_list(SSpiky::get_class());
  writer.write("state", static_cast<int>(state));
  writer.end_list(SSpiky::get_class());
}

void
SSpiky::restore(const ReaderMapping& reader)
{
  BadGuy::restore(reader);

  boost::optional<ReaderMapping> subreader(ReaderMapping(reader.get_doc(), reader.get_sexp()));

  if (reader.get(SSpiky::get_class().c_str(), subreader))
  {
    int s;
    if (subreader->get("state", s))
      state = static_cast<SSpikyState>(s);
  }
}

/* EOF */
