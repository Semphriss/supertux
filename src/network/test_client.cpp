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

#include "network/test_client.hpp"

#include "control/controller.hpp"
#include "network/client.hpp"
#include "util/log.hpp"

#include <iostream>

using namespace network;

TestClient::TestClient() :
  m_client()
{
  m_client = new Client(3474, "127.0.0.1", [](Connection* c, const std::string& data) {
    log_warning << "Received data from server : {{" << data << "}}" << std::endl;
  });
}

TestClient::~TestClient()
{
  m_client->close();
  m_client->destroy();
}

void
TestClient::setup()
{
  m_client->init();
}

void
TestClient::update(float dt_sec, const Controller& controller)
{
  if (!m_client->is_closed())
  {
    std::string ctrl_state = "";

    for(int i = static_cast<int>(Control::LEFT); i < static_cast<int>(Control::CONTROLCOUNT); i++)
    {
      ctrl_state += std::to_string(controller.hold(static_cast<Control>(i)));
    }

    m_client->send(ctrl_state);
  }
}

/* EOF */
