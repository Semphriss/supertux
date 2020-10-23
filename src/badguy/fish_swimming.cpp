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

#include "badguy/fish_swimming.hpp"

#include "sprite/sprite.hpp"

FishSwimming::FishSwimming(const ReaderMapping& reader) :
  BadGuy(reader, "images/creatures/fish_swimming/fish_swimming.sprite"),
  state(FishState::SWIMMING),
  turn_timer(0.f)
{
  m_physic.enable_gravity(false);
}

void
FishSwimming::initialize()
{
  m_physic.set_velocity_x(m_dir == Direction::LEFT ? -256 : 256);
  m_sprite->set_action(m_dir == Direction::LEFT ? "left" : "right");
  state = FishState::SWIMMING;
}

void
FishSwimming::collision_solid(const CollisionHit& hit)
{
  if (m_sprite->get_action() == "iced-left" ||
     m_sprite->get_action() == "iced-right")
  {
    return;
  }

  if (hit.top || hit.bottom) {
    // TODO
  } else if (hit.left || hit.right) {
    if (m_frozen)
    {
      m_physic.set_velocity_x(0);
      return;
    }

    m_dir = (m_dir == Direction::LEFT ? Direction::RIGHT : Direction::LEFT);
    m_sprite->set_action(m_dir == Direction::LEFT ? "r-turn-l" : "l-turn-r");
    m_physic.set_velocity_x(0);
    state = FishState::TURNING;
    turn_timer = .25f;
  }
}

void
FishSwimming::active_update(float dt_sec) {
  BadGuy::active_update(dt_sec);

  if (turn_timer > 0.f) {
    turn_timer -= dt_sec;
    if (turn_timer <= 0.f) {
      turn_timer = 0.f;
      m_sprite->set_action(m_dir == Direction::LEFT ? "left" : "right");
      m_physic.set_velocity_x(m_physic.get_velocity_x()/2 + (m_dir == Direction::LEFT ? -128.f : 128.f));
      state = FishState::SWIMMING;
    }
  }

  if (state == FishState::SWIMMING) {
    m_physic.set_velocity_x(m_physic.get_velocity_x()/1.02f + (m_dir == Direction::LEFT ? -1.f : 1.f));
  }
}

void
FishSwimming::freeze()
{
  BadGuy::freeze();
  m_physic.enable_gravity(true);
  m_physic.set_velocity_x(0);
}

void
FishSwimming::unfreeze()
{
  BadGuy::unfreeze();
  m_physic.enable_gravity(false);
  initialize();
}

bool
FishSwimming::is_freezable() const
{
  return true;
}


/* EOF */
