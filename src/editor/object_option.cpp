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

#include "editor/object_option.hpp"

#include <string>
#include <utility>

#include <vector>
#include <sstream>

#include "editor/editor.hpp"
#include "editor/object_menu.hpp"
#include "gui/menu.hpp"
#include "gui/menu_badguy_select.hpp"
#include "gui/menu_filesystem.hpp"
#include "gui/menu_manager.hpp"
#include "interface/container.hpp"
#include "interface/control_button.hpp"
#include "interface/control_checkbox.hpp"
#include "interface/control_enum.hpp"
#include "interface/control_textbox.hpp"
#include "interface/control_textbox_float.hpp"
#include "interface/control_textbox_int.hpp"
#include "interface/label.hpp"
#include "object/tilemap.hpp"
#include "supertux/game_object.hpp"
#include "supertux/sector.hpp"
#include "util/gettext.hpp"
#include "util/writer.hpp"
#include "video/color.hpp"

namespace {

template<typename T>
std::string fmt_to_string(const T& v)
{
  std::ostringstream out;
  out << v;
  return out.str();
}

} // namespace

ObjectOption::ObjectOption(const std::string& text, const std::string& key, unsigned int flags) :
  m_text(text),
  m_key(key),
  m_flags(flags)
{
}

ObjectOption::~ObjectOption()
{
}

BoolObjectOption::BoolObjectOption(const std::string& text, bool* pointer, const std::string& key,
                                   boost::optional<bool> default_value,
                                   unsigned int flags) :
  ObjectOption(text, key, flags),
  m_pointer(pointer),
  m_default_value(std::move(default_value))
{
}

void
BoolObjectOption::add_to_menu(Menu& menu) const
{
  menu.add_toggle(-1, get_text(), m_pointer);
}

std::unique_ptr<InterfaceControl>
BoolObjectOption::add_to_settings(float width, Editor& editor, GameObject* go) const
{
  auto control = std::make_unique<ControlCheckbox>();
  control->set_rect(Rectf(width * 3.f / 4.f - 10.f, 0.f, width * 3.f / 4.f + 10.f, 20.f));
  control->bind_value(m_pointer);
  control->m_label = new InterfaceLabel();
  control->m_label->set_rect(Rectf(10.f, 0.f, width / 2.f - 5.f, 20.f));
  control->m_label->set_label(get_text());

  return control;
}

void
BoolObjectOption::save(Writer& writer) const
{
  if (!get_key().empty()) {
    if (m_default_value && *m_default_value == *m_pointer) {
      // skip
    } else {
      writer.write(get_key(), *m_pointer);
    }
  }
}

std::string
BoolObjectOption::to_string() const
{
  return *m_pointer ? _("true") : _("false");
}

IntObjectOption::IntObjectOption(const std::string& text, int* pointer, const std::string& key,
                                 boost::optional<int> default_value,
                                 unsigned int flags) :
  ObjectOption(text, key, flags),
  m_pointer(pointer),
  m_default_value(std::move(default_value))
{
}

void
IntObjectOption::save(Writer& writer) const
{
  if (!get_key().empty()) {
    if (m_default_value && *m_default_value == *m_pointer) {
      // skip
    } else {
      writer.write(get_key(), *m_pointer);
    }
  }
}

std::string
IntObjectOption::to_string() const
{
  return fmt_to_string(*m_pointer);
}

void
IntObjectOption::add_to_menu(Menu& menu) const
{
  menu.add_intfield(get_text(), m_pointer);
}

std::unique_ptr<InterfaceControl>
IntObjectOption::add_to_settings(float width, Editor& editor, GameObject* go) const
{
  auto control = std::make_unique<ControlTextboxInt>();
  control->set_rect(Rectf(width / 2.f + 5.f, 0.f, width - 10.f, 20.f));
  control->bind_value(m_pointer);
  control->m_label = new InterfaceLabel();
  control->m_label->set_rect(Rectf(10.f, 0.f, width / 2.f - 5.f, 20.f));
  control->m_label->set_label(get_text());

  return control;
}

RectfObjectOption::RectfObjectOption(const std::string& text, Rectf* pointer, const std::string& key,
                                     unsigned int flags) :
  ObjectOption(text, key, flags),
  m_pointer(pointer),
  m_width(m_pointer->get_width()),
  m_height(m_pointer->get_height())
{
}

void
RectfObjectOption::save(Writer& write) const
{
  write.write("width", m_width);
  write.write("height", m_height);
  // write.write("x", &pointer->p1.x);
  // write.write("y", &pointer->p1.y);
}

std::string
RectfObjectOption::to_string() const
{
  std::ostringstream out;
  out << *m_pointer;
  return out.str();
}

void
RectfObjectOption::add_to_menu(Menu& menu) const
{
  menu.add_floatfield(_("Width"), const_cast<float*>(&m_width));
  menu.add_floatfield(_("Height"), const_cast<float*>(&m_height));
}

std::unique_ptr<InterfaceControl>
RectfObjectOption::add_to_settings(float width, Editor& editor, GameObject* go) const
{
  auto container = std::make_unique<InterfaceContainer>();
  container->set_rect(Rectf(10.f, 0.f, width - 10.f, 20.f));

  auto control_w = std::make_unique<ControlTextboxFloat>();
  control_w->set_rect(Rectf(width / 2.f + 5.f, 0.f, width * 3.f / 4.f - 5.f, 20.f));
  control_w->bind_value(const_cast<float*>(&m_width));

  auto control_h = std::make_unique<ControlTextboxFloat>();
  control_h->set_rect(Rectf(width * 3.f / 4.f + 5.f, 0.f, width - 10.f, 20.f));
  control_h->bind_value(const_cast<float*>(&m_height));

  container->m_label = new InterfaceLabel();
  container->m_label->set_rect(Rectf(10.f, 0.f, width / 2.f - 5.f, 20.f));
  container->m_label->set_label(_("Width") + " / " + _("Height"));
  container->m_children.push_back(std::move(control_w));
  container->m_children.push_back(std::move(control_h));

  return container;
}

FloatObjectOption::FloatObjectOption(const std::string& text, float* pointer, const std::string& key,
                                     boost::optional<float> default_value,
                                     unsigned int flags) :
  ObjectOption(text, key, flags),
  m_pointer(pointer),
  m_default_value(std::move(default_value))
{
}

void
FloatObjectOption::save(Writer& writer) const
{
  if (!get_key().empty()) {
    if (m_default_value && *m_default_value == *m_pointer) {
      // skip
    } else {
      writer.write(get_key(), *m_pointer);
    }
  }
}

std::string
FloatObjectOption::to_string() const
{
  return fmt_to_string(*m_pointer);
}

void
FloatObjectOption::add_to_menu(Menu& menu) const
{
  menu.add_floatfield(get_text(), m_pointer);
}

std::unique_ptr<InterfaceControl>
FloatObjectOption::add_to_settings(float width, Editor& editor, GameObject* go) const
{
  auto control = std::make_unique<ControlTextboxFloat>();
  control->set_rect(Rectf(width / 2.f + 5.f, 0.f, width - 10.f, 20.f));
  control->bind_value(m_pointer);
  control->m_label = new InterfaceLabel();
  control->m_label->set_rect(Rectf(10.f, 0.f, width / 2.f - 5.f, 20.f));
  control->m_label->set_label(get_text());

  return control;
}

StringObjectOption::StringObjectOption(const std::string& text, std::string* pointer, const std::string& key,
                                       boost::optional<std::string> default_value,
                                       unsigned int flags) :
  ObjectOption(text, key, flags),
  m_pointer(pointer),
  m_default_value(std::move(default_value))
{
}

void
StringObjectOption::save(Writer& writer) const
{
  if (!get_key().empty()) {
    if ((m_default_value && *m_default_value == *m_pointer) || m_pointer->empty()) {
      // skip
    } else {
      writer.write(get_key(), *m_pointer, (get_flags() & OPTION_TRANSLATABLE));
    }
  }
}

std::string
StringObjectOption::to_string() const
{
  return *m_pointer;
}

void
StringObjectOption::add_to_menu(Menu& menu) const
{
  menu.add_textfield(get_text(), m_pointer);
}

std::unique_ptr<InterfaceControl>
StringObjectOption::add_to_settings(float width, Editor& editor, GameObject* go) const
{
  auto control = std::make_unique<ControlTextbox>();
  control->set_rect(Rectf(width / 2.f + 5.f, 0.f, width - 10.f, 20.f));
  control->bind_string(&(*m_pointer));
  control->m_label = new InterfaceLabel();
  control->m_label->set_rect(Rectf(10.f, 0.f, width / 2.f - 5.f, 20.f));
  control->m_label->set_label(get_text());

  return control;
}

StringSelectObjectOption::StringSelectObjectOption(const std::string& text, int* pointer,
                                                   const std::vector<std::string>& select,
                                                   boost::optional<int> default_value,
                                                   const std::string& key, unsigned int flags) :
  ObjectOption(text, key, flags),
  m_pointer(pointer),
  m_select(select),
  m_default_value(std::move(default_value))
{
}

void
StringSelectObjectOption::save(Writer& writer) const
{
  if (!get_key().empty()) {
    if (m_default_value && *m_default_value == *m_pointer) {
      // skip
    } else {
      writer.write(get_key(), *m_pointer);
    }
  }
}

std::string
StringSelectObjectOption::to_string() const
{
  int* selected_id = static_cast<int*>(m_pointer);
  if (*selected_id >= int(m_select.size()) || *selected_id < 0) {
    return _("invalid"); //Test whether the selected ID is valid
  } else {
    return m_select[*selected_id];
  }
}

void
StringSelectObjectOption::add_to_menu(Menu& menu) const
{
  int& selected_id = *m_pointer;
  if ( selected_id >= static_cast<int>(m_select.size()) || selected_id < 0 ) {
    selected_id = 0; // Set the option to zero when not selectable
  }
  menu.add_string_select(-1, get_text(), m_pointer, m_select);
}

std::unique_ptr<InterfaceControl>
StringSelectObjectOption::add_to_settings(float width, Editor& editor, GameObject* go) const
{
  auto control = std::make_unique<ControlEnum<int>>();
  control->set_rect(Rectf(width / 2.f + 5.f, 0.f, width - 10.f, 20.f));

  int i = 0;
  for (const auto& label : m_select)
    control->add_option(i++, label);

  control->bind_value(m_pointer);

  control->m_label = new InterfaceLabel();
  control->m_label->set_rect(Rectf(10.f, 0.f, width / 2.f - 5.f, 20.f));
  control->m_label->set_label(get_text());

  return control;
}

EnumObjectOption::EnumObjectOption(const std::string& text, int* pointer,
                                   const std::vector<std::string>& labels,
                                   const std::vector<std::string>& symbols,
                                   boost::optional<int> default_value,
                                   const std::string& key, unsigned int flags) :
  ObjectOption(text, key, flags),
  m_pointer(pointer),
  m_labels(labels),
  m_symbols(symbols),
  m_default_value(std::move(default_value))
{
}

void
EnumObjectOption::save(Writer& writer) const
{
  if (0 <= *m_pointer && *m_pointer < int(m_symbols.size()) &&
      !get_key().empty())
  {
    if (m_default_value && *m_default_value == *m_pointer) {
      // skip
    } else {
      writer.write(get_key(), m_symbols[*m_pointer]);
    }
  }
}

std::string
EnumObjectOption::to_string() const
{
  if (0 <= *m_pointer && *m_pointer < int(m_labels.size())) {
    return m_labels[*m_pointer];
  } else {
    return _("invalid");
  }
}

void
EnumObjectOption::add_to_menu(Menu& menu) const
{
  if (*m_pointer >= static_cast<int>(m_labels.size()) || *m_pointer < 0 ) {
    *m_pointer = 0; // Set the option to zero when not selectable
  }
  menu.add_string_select(-1, get_text(), m_pointer, m_labels);
}

std::unique_ptr<InterfaceControl>
EnumObjectOption::add_to_settings(float width, Editor& editor, GameObject* go) const
{
  auto control = std::make_unique<ControlEnum<int>>();
  control->set_rect(Rectf(width / 2.f + 5.f, 0.f, width - 10.f, 20.f));

  int i = 0;
  for (const auto& label : m_labels)
    control->add_option(i++, label);

  control->bind_value(m_pointer);

  control->m_label = new InterfaceLabel();
  control->m_label->set_rect(Rectf(10.f, 0.f, width / 2.f - 5.f, 20.f));
  control->m_label->set_label(get_text());

  return control;
}


ScriptObjectOption::ScriptObjectOption(const std::string& text, std::string* pointer, const std::string& key,
                                       unsigned int flags) :
  ObjectOption(text, key, flags),
  m_pointer(pointer)
{
}

void
ScriptObjectOption::save(Writer& writer) const
{
  auto& value = *m_pointer;
  if (!value.empty())
  {
    if (!get_key().empty()) {
      writer.write(get_key(), value);
    }
  }
}

std::string
ScriptObjectOption::to_string() const
{
  if (!m_pointer->empty()) {
    return "...";
  }
  return "";
}

void
ScriptObjectOption::add_to_menu(Menu& menu) const
{
  menu.add_script(get_text(), m_pointer);
}

std::unique_ptr<InterfaceControl>
ScriptObjectOption::add_to_settings(float width, Editor& editor, GameObject* go) const
{
  auto control = std::make_unique<ControlTextbox>();
  control->set_rect(Rectf(width / 2.f + 5.f, 0.f, width - 10.f, 20.f));
  control->bind_string(m_pointer);
  control->m_label = new InterfaceLabel();
  control->m_label->set_rect(Rectf(10.f, 0.f, width / 2.f - 5.f, 20.f));
  control->m_label->set_label(get_text());

  return control;
}

FileObjectOption::FileObjectOption(const std::string& text, std::string* pointer,
                                   boost::optional<std::string> default_value,
                                   const std::string& key,
                                   std::vector<std::string> filter,
                                   const std::string& basedir,
                                   unsigned int flags) :
  ObjectOption(text, key, flags),
  m_pointer(pointer),
  m_default_value(std::move(default_value)),
  m_filter(std::move(filter)),
  m_basedir(basedir)
{
}

void
FileObjectOption::save(Writer& writer) const
{
  if (m_default_value && *m_default_value == *m_pointer) {
    // skip
  } else {
    auto& value = *m_pointer;
    if (!value.empty())
    {
      if (!get_key().empty()) {
        writer.write(get_key(), value);
      }
    }
  }
}

std::string
FileObjectOption::to_string() const
{
  return *m_pointer;
}

void
FileObjectOption::add_to_menu(Menu& menu) const
{
  menu.add_file(get_text(), m_pointer, m_filter, m_basedir);
}

std::unique_ptr<InterfaceControl>
FileObjectOption::add_to_settings(float width, Editor& editor, GameObject* go) const
{
  auto control = std::make_unique<ControlButton>();
  control->set_rect(Rectf(10.f, 0.f, width - 10.f, 20.f));
  control->m_btn_label = get_text();

  // Make member variables into standalone variables so that the lambda can
  // outlive the FileObjectOption
  auto pointer = m_pointer;
  auto filter = m_filter;
  auto basedir = m_basedir;
  control->m_on_change = [pointer, filter, basedir] {
    MenuManager::instance().push_menu(std::make_unique<FileSystemMenu>(pointer, filter, basedir));
  };

  return control;
}

ColorObjectOption::ColorObjectOption(const std::string& text, Color* pointer, const std::string& key,
                                     boost::optional<Color> default_value, bool use_alpha,
                                     unsigned int flags) :
  ObjectOption(text, key, flags),
  m_pointer(pointer),
  m_default_value(std::move(default_value)),
  m_use_alpha(use_alpha)
{
}

void
ColorObjectOption::save(Writer& writer) const
{
  if (!get_key().empty()) {
    if (m_default_value && *m_default_value == *m_pointer) {
      // skip
    } else {
      auto vec = m_pointer->toVector();
      if (!m_use_alpha || vec.back() == 1.0f) {
        vec.pop_back();
      }
      writer.write(get_key(), vec);
    }
  }
}

std::string
ColorObjectOption::to_string() const
{
  return m_pointer->to_string();
}

void
ColorObjectOption::add_to_menu(Menu& menu) const
{
  menu.add_color(get_text(), m_pointer);
}

std::unique_ptr<InterfaceControl>
ColorObjectOption::add_to_settings(float width, Editor& editor, GameObject* go) const
{
  auto container = std::make_unique<InterfaceContainer>();
  container->set_rect(Rectf(10.f, 0.f, width - 10.f, 44.f));

  auto control_r = std::make_unique<ControlTextboxFloat>();
  control_r->set_rect(Rectf(width / 2.f + 5.f, 0.f, width * 3.f / 4.f - 5.f, 20.f));
  control_r->bind_value(const_cast<float*>(&(m_pointer->red)));

  auto control_g = std::make_unique<ControlTextboxFloat>();
  control_g->set_rect(Rectf(width * 3.f / 4.f + 5.f, 0.f, width - 10.f, 20.f));
  control_g->bind_value(const_cast<float*>(&(m_pointer->green)));

  auto control_b = std::make_unique<ControlTextboxFloat>();
  control_b->set_rect(Rectf(width / 2.f + 5.f, 24.f, width * 3.f / 4.f - 5.f, 44.f));
  control_b->bind_value(const_cast<float*>(&(m_pointer->blue)));

  auto control_a = std::make_unique<ControlTextboxFloat>();
  control_a->set_rect(Rectf(width * 3.f / 4.f + 5.f, 24.f, width - 10.f, 44.f));
  control_a->bind_value(const_cast<float*>(&(m_pointer->alpha)));

  container->m_label = new InterfaceLabel();
  container->m_label->set_rect(Rectf(10.f, 0.f, width / 2.f - 5.f, 20.f));
  container->m_label->set_label(get_text());

  return container;
}

BadGuySelectObjectOption::BadGuySelectObjectOption(const std::string& text, std::vector<std::string>* pointer, const std::string& key,
                                                   unsigned int flags) :
  ObjectOption(text, key, flags),
  m_pointer(pointer)
{
}

void
BadGuySelectObjectOption::save(Writer& writer) const
{
  if (!get_key().empty()) {
    writer.write(get_key(), *m_pointer);
  }
}

std::string
BadGuySelectObjectOption::to_string() const
{
  return fmt_to_string(m_pointer->size());
}

void
BadGuySelectObjectOption::add_to_menu(Menu& menu) const
{
  menu.add_badguy_select(get_text(), m_pointer);
}

std::unique_ptr<InterfaceControl>
BadGuySelectObjectOption::add_to_settings(float width, Editor& editor, GameObject* go) const
{
  auto control = std::make_unique<ControlButton>();
  control->set_rect(Rectf(10.f, 0.f, width - 10.f, 20.f));
  control->m_btn_label = get_text();

  // Make member variables into standalone variables so that the lambda can
  // outlive the FileObjectOption
  auto pointer = m_pointer;
  control->m_on_change = [pointer] {
    MenuManager::instance().push_menu(std::make_unique<BadguySelectMenu>(pointer));
  };

  return control;
}

TilesObjectOption::TilesObjectOption(const std::string& text, TileMap* tilemap, const std::string& key,
                                     unsigned int flags) :
  ObjectOption(text, key, flags),
  m_tilemap(tilemap)
{
}

void
TilesObjectOption::save(Writer& write) const
{
  write.write("width", m_tilemap->get_width());
  write.write("height", m_tilemap->get_height());
  write.write("tiles", m_tilemap->get_tiles(), m_tilemap->get_width());
}

std::string
TilesObjectOption::to_string() const
{
  return {};
}

void
TilesObjectOption::add_to_menu(Menu& menu) const
{
}

std::unique_ptr<InterfaceControl>
TilesObjectOption::add_to_settings(float width, Editor& editor, GameObject* go) const
{
  return {};
}

PathObjectOption::PathObjectOption(const std::string& text, Path* path, const std::string& key,
                                   unsigned int flags) :
  ObjectOption(text, key, flags),
  m_path(path)
{
}

void
PathObjectOption::save(Writer& write) const
{
  m_path->save(write);
}

std::string
PathObjectOption::to_string() const
{
  return {};
}

void
PathObjectOption::add_to_menu(Menu& menu) const
{
}

std::unique_ptr<InterfaceControl>
PathObjectOption::add_to_settings(float width, Editor& editor, GameObject* go) const
{
  return {};
}

PathRefObjectOption::PathRefObjectOption(const std::string& text, const std::string& path_ref, const std::string& key,
                                   unsigned int flags) :
  ObjectOption(text, key, flags),
  m_path_ref(path_ref)
{
}

void
PathRefObjectOption::save(Writer& writer) const
{
  if (!m_path_ref.empty()) {
    writer.write(get_key(), m_path_ref);
  }
}

std::string
PathRefObjectOption::to_string() const
{
  return m_path_ref;
}

void
PathRefObjectOption::add_to_menu(Menu& menu) const
{
}

std::unique_ptr<InterfaceControl>
PathRefObjectOption::add_to_settings(float width, Editor& editor, GameObject* go) const
{
  return {};
}

SExpObjectOption::SExpObjectOption(const std::string& text, const std::string& key, sexp::Value& value,
                                   unsigned int flags) :
  ObjectOption(text, key, flags),
  m_sx(value)
{
}

void
SExpObjectOption::save(Writer& writer) const
{
  if (!m_sx.is_nil()) {
    writer.write(get_key(), m_sx);
  }
}

std::string
SExpObjectOption::to_string() const
{
  return m_sx.str();
}

void
SExpObjectOption::add_to_menu(Menu& menu) const
{
}

std::unique_ptr<InterfaceControl>
SExpObjectOption::add_to_settings(float width, Editor& editor, GameObject* go) const
{
  return {};
}

RemoveObjectOption::RemoveObjectOption() :
  ObjectOption(_("Remove"), "", 0)
{
}

std::string
RemoveObjectOption::to_string() const
{
  return {};
}

void
RemoveObjectOption::add_to_menu(Menu& menu) const
{
  menu.add_entry(ObjectMenu::MNID_REMOVE, get_text());
}

std::unique_ptr<InterfaceControl>
RemoveObjectOption::add_to_settings(float width, Editor& editor, GameObject* go) const
{
  auto control = std::make_unique<ControlButton>();
  control->set_rect(Rectf(10.f, 0.f, width - 10.f, 20.f));
  control->m_btn_label = get_text();

  // Make member variables into standalone variables so that the lambda can
  // outlive the FileObjectOption
  control->m_on_change = [&editor, go] {
    editor.delete_markers();
    editor.m_reactivate_request = true;
    go->remove_me();
    editor.m_settings_widget->set_object(nullptr);
  };

  return control;
}

TestFromHereOption::TestFromHereOption() :
  ObjectOption(_("Test from here"), "", 0)
{
}

std::string
TestFromHereOption::to_string() const
{
  return {};
}

void
TestFromHereOption::add_to_menu(Menu& menu) const
{
  menu.add_entry(ObjectMenu::MNID_TEST_FROM_HERE, get_text());
}

std::unique_ptr<InterfaceControl>
TestFromHereOption::add_to_settings(float width, Editor& editor, GameObject* go) const
{
  auto control = std::make_unique<ControlButton>();
  control->set_rect(Rectf(10.f, 0.f, width - 10.f, 20.f));
  control->m_btn_label = get_text();

  // Make member variables into standalone variables so that the lambda can
  // outlive the FileObjectOption
  control->m_on_change = [&editor, go] {
    const MovingObject *here = dynamic_cast<const MovingObject *>(go);
    editor.m_test_pos = std::make_pair(editor.get_sector()->get_name(),
                                        here->get_pos());
    editor.m_test_request = true;
  };

  return control;
}

ParticleEditorOption::ParticleEditorOption() :
  ObjectOption(_("Open Particle Editor"), "", 0)
{
}

std::string
ParticleEditorOption::to_string() const
{
  return {};
}

void
ParticleEditorOption::add_to_menu(Menu& menu) const
{
  menu.add_entry(ObjectMenu::MNID_OPEN_PARTICLE_EDITOR, get_text());
}

std::unique_ptr<InterfaceControl>
ParticleEditorOption::add_to_settings(float width, Editor& editor, GameObject* go) const
{
  auto control = std::make_unique<ControlButton>();
  control->set_rect(Rectf(10.f, 0.f, width - 10.f, 20.f));
  control->m_btn_label = get_text();

  // Make member variables into standalone variables so that the lambda can
  // outlive the FileObjectOption
  control->m_on_change = [&editor] {
    editor.m_particle_editor_request = true;
  };

  return control;
}

ButtonOption::ButtonOption(const std::string& text, std::function<void()> callback) :
  ObjectOption(text, "", 0),
  m_callback(std::move(callback))
{
}

std::string
ButtonOption::to_string() const
{
  return {};
}

void
ButtonOption::add_to_menu(Menu& menu) const
{
  menu.add_entry(get_text(), m_callback);
}

std::unique_ptr<InterfaceControl>
ButtonOption::add_to_settings(float width, Editor& editor, GameObject* go) const
{
  auto control = std::make_unique<ControlButton>();
  control->set_rect(Rectf(10.f, 0.f, width - 10.f, 20.f));
  control->m_btn_label = get_text();
  control->m_on_change = m_callback;

  return control;
}

/* EOF */
