//  DartTrap - Shoots a Dart at regular intervals
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
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

#include "badguy/darttrap.hpp"

#include "audio/sound_manager.hpp"
#include "audio/sound_source.hpp"
#include "badguy/dart.hpp"
#include "editor/editor.hpp"
#include "sprite/sprite.hpp"
#include "supertux/sector.hpp"
#include "util/log.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"

namespace {
const float MUZZLE_Y = 25; /**< [px] muzzle y-offset from top */
}

DartTrap::DartTrap(const ReaderMapping& reader) :
  BadGuy(reader, "images/creatures/darttrap/darttrap.sprite", LAYER_TILES-1),
  enabled(true),
  initial_delay(),
  fire_delay(),
  ammo(),
  state(IDLE),
  fire_timer()
{
  reader.get("enabled", enabled, true);
  reader.get("initial-delay", initial_delay, 0.0f);
  reader.get("fire-delay", fire_delay, 2.0f);
  reader.get("ammo", ammo, -1);
  m_countMe = false;
  SoundManager::current()->preload("sounds/dartfire.wav");
  if (m_start_dir == Direction::AUTO) { log_warning << "Setting a DartTrap's direction to AUTO is no good idea" << std::endl; }
  state = IDLE;
  set_colgroup_active(COLGROUP_DISABLED);

  if (!Editor::is_active()) {
    if (initial_delay == 0) initial_delay = 0.1f;
  }
}

void
DartTrap::initialize()
{
  m_sprite->set_action(m_dir == Direction::LEFT ? "idle-left" : "idle-right");
}

void
DartTrap::activate()
{
  fire_timer.start(initial_delay);
}

HitResponse
DartTrap::collision_player(Player& , const CollisionHit& )
{
  return ABORT_MOVE;
}

void
DartTrap::active_update(float )
{
  if (!enabled) {
    return;
  }
  switch (state) {
    case IDLE:
      if ((ammo != 0) && (fire_timer.check())) {
        if (ammo > 0) ammo--;
        load();
        fire_timer.start(fire_delay);
      }
      break;

    case LOADING:
      if (m_sprite->animation_done()) {
        fire();
      }
      break;

    default:
      break;
  }
}

void
DartTrap::load()
{
  state = LOADING;
  m_sprite->set_action(m_dir == Direction::LEFT ? "loading-left" : "loading-right", 1);
}

void
DartTrap::fire()
{
  float px = get_pos().x;
  if (m_dir == Direction::RIGHT) px += 5;
  float py = get_pos().y;
  py += MUZZLE_Y;

  SoundManager::current()->play("sounds/dartfire.wav", get_pos());
  Sector::get().add<Dart>(Vector(px, py), m_dir, this);
  state = IDLE;
  m_sprite->set_action(m_dir == Direction::LEFT ? "idle-left" : "idle-right");
}

ObjectSettings
DartTrap::get_settings()
{
  ObjectSettings result = BadGuy::get_settings();

  result.add_float(_("Initial delay"), &initial_delay, "initial-delay");
  result.add_bool(_("Enabled"), &enabled, "enabled", true);
  result.add_float(_("Fire delay"), &fire_delay, "fire-delay");
  result.add_int(_("Ammo"), &ammo, "ammo");

  result.reorder({"initial-delay", "fire-delay", "ammo", "direction", "x", "y"});

  return result;
}

void
DartTrap::backup(Writer& writer) const
{
  BadGuy::backup(writer);

  writer.start_list(DartTrap::get_class());
  writer.write("enabled", enabled);
  writer.write("initial_delay", initial_delay);
  writer.write("fire_delay", fire_delay);
  writer.write("ammo", ammo);
  writer.write("state", static_cast<int>(state));
  writer.start_list("fire_timer");
  fire_timer.backup(writer);
  writer.end_list("fire_timer");
  writer.end_list(DartTrap::get_class());
}

void
DartTrap::restore(const ReaderMapping& reader)
{
  BadGuy::restore(reader);

  boost::optional<ReaderMapping> subreader(ReaderMapping(reader.get_doc(), reader.get_sexp()));

  if (reader.get(DartTrap::get_class().c_str(), subreader))
  {
    subreader->get("enabled", enabled);
    subreader->get("initial_delay", initial_delay);
    subreader->get("fire_delay", fire_delay);
    subreader->get("ammo", ammo);
    int s;
    if(subreader->get("state", s))
      state = static_cast<State>(s);

    boost::optional<ReaderMapping> subreader2(ReaderMapping(reader.get_doc(), reader.get_sexp()));
    if (subreader->get("fire_timer", subreader2))
      fire_timer.restore(*subreader2);
  }
}

/* EOF */
