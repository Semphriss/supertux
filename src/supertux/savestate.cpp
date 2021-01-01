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

#include "supertux/savestate.hpp"

#include "supertux/sector.hpp"
#include "util/log.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"
#include "util/reader_object.hpp"
#include "util/writer.hpp"

Savestate::Savestate() :
  m_sector(nullptr),
  m_backup_data()
{
}

Savestate::Savestate(Sector* sector) :
  m_sector(sector),
  m_backup_data()
{
}

void
Savestate::save()
{
  if (!m_sector)
    throw std::runtime_error("Attempt to restore savestate with null sector");

  clear();
  Writer writer(m_backup_data);
  m_sector->backup(writer);
}

void
Savestate::clear()
{
  m_backup_data = std::stringstream();
}

void
Savestate::restore()
{
  if (!m_sector)
    throw std::runtime_error("Attempt to restore savestate with null sector");

  if (!to_string().empty())
  {
    std::stringstream s;
    s << to_string();
    ReaderDocument doc = ReaderDocument::from_stream(s);
    ReaderObject object = doc.get_root();
    m_sector->restore(object.get_mapping());
  }
}

/* EOF */
