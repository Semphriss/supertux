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

#include "network/test_server.hpp"

#include <functional>

#include "network/server.hpp"
#include "util/base64.hpp"
#include "util/sha1.hpp"
#include "util/log.hpp"

//using namespace network;

bool
TestServer::handle_websocket_header(network::Connection* connection,
                                    const std::string& data)
{
  std::string remainder = data;

  bool has_connection_header = false;
  bool has_upgrade_header = false;
  bool has_valid_protocol = false;
  std::string upgrade_key = "";
  int protocol_version = 0;

  while (remainder.find("\r\n") != std::string::npos)
  {
    std::string header = remainder.substr(0, remainder.find("\r\n"));
    log_warning << header << std::endl;
    remainder = remainder.substr(remainder.find("\r\n") + 2);

    if (header == "Connection: Upgrade")
    {
      has_connection_header = true;
    }
    else if (header == "Upgrade: websocket")
    {
      has_upgrade_header = true;
    }
    else if (header == "Sec-WebSocket-Protocol: supertux")
    {
      has_valid_protocol = true;
    }
    else if (header.substr(0, 19) == "Sec-WebSocket-Key: ")
    {
      upgrade_key = header.substr(19);
    }
    else if (header.substr(0, 23) == "Sec-WebSocket-Version: ")
    {
      try {
        protocol_version = std::stoi(header.substr(23));
      } catch(...) {}
    }
    else if (header.substr(0, 8) == "Origin: ")
    {
      log_warning << "WebSocket comes from: " << header.substr(8) << std::endl;
    }
  }

  if (has_connection_header && has_upgrade_header && has_valid_protocol && !upgrade_key.empty() && protocol_version == 1)
  {
    SHA1 sha1;
    sha1.update(upgrade_key);
    std::string hash = sha1.final();
    std::string response_key = base64_encode(hash.c_str(), hash.size());
    connection->send("HTTP/1.1 101 Switching Protocols"
                     "\r\nUpgrade: websocket"
                     "\r\nConnection: Upgrade"
                     "\r\nSec-WebSocket-Accept: " + response_key +
                     "\r\nSec-WebSocket-Protocol: supertux\r\n\r");
    return true;
  }
  else
  {
    connection->send("HTTP/1.1 400 Bad Request\r\n\r\n");
    connection->close();
    return false;
  }
}

TestServer::TestServer() :
  m_server(3474,
          [this](network::ConnectionPtr c){ on_connect(std::move(c)); },
          [this](network::Connection* c, const std::string& d) { on_receive(c, d); }),
  m_pool(std::make_unique<network::ConnectionPool>())
{
}

TestServer::~TestServer()
{
  m_server.stop();
}

void
TestServer::setup()
{
  m_server.start();
  log_warning << "Server started" << std::endl;
}

void
TestServer::update(float dt_sec, const Controller& controller)
{
}

void
TestServer::on_connect(network::ConnectionPtr connection)
{
  log_warning << "A new client has connected to the server: " << connection->get_uuid() << std::endl;
  m_pool->add_connection(std::move(connection));
}

void
TestServer::on_receive(network::Connection* connection, const std::string& data)
{
  if (data.size() == 0)
    return;

  std::string write_data = data;

  // WASM: Detect WebSockets
  if (!connection->m_properties["is_websocket"])
  {

    if (data[0] == 'G')
    {

      connection->m_properties["is_websocket"] = new bool;
      *static_cast<bool*>(connection->m_properties["is_websocket"]) = true;

      auto match = data.find("\r\n\r\n");
      if (match != std::string::npos)
      {
        if (!handle_websocket_header(connection, data.substr(0, match)))
          return;

        *static_cast<std::string*>(connection->m_properties["websocket_header_buffer"]) = "\r\n\r\n";

        write_data = data.substr(match + 2);
        if (write_data.size() == 0)
          return;
      }
      else
      {
        connection->m_properties["websocket_header_buffer"] = new std::string(data + "\n");
        return;
      }
    }
    else
    {

      connection->m_properties["is_websocket"] = new bool;
      *static_cast<bool*>(connection->m_properties["is_websocket"]) = false;
    }
  }
  else if (*static_cast<bool*>(connection->m_properties["is_websocket"])
          && *static_cast<std::string*>(connection->m_properties["websocket_header_buffer"]) != "\r\n\r\n")
  {

    *static_cast<std::string*>(connection->m_properties["websocket_header_buffer"]) += data + "\n";

    auto full_buffer = *static_cast<std::string*>(connection->m_properties["websocket_header_buffer"]);

    auto match = full_buffer.find("\r\n\r\n");
    if (match != std::string::npos)
    {
      if (!handle_websocket_header(connection, full_buffer.substr(0, match + 2)))
        return;

      *static_cast<std::string*>(connection->m_properties["websocket_header_buffer"]) = "\r\n\r\n";

      write_data = full_buffer.substr(match + 4);
      if (write_data.size() == 0)
        return;
    }
    else
    {
      return;
    }
  }

  m_pool->send_all_except(connection->get_uuid() + ": " + write_data, connection);
}

/* EOF */
