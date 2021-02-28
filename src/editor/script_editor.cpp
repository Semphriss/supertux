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

#include "editor/script_editor.hpp"

#include "math/util.hpp"
#include "util/log.hpp"

const std::vector<std::string> keywords {
  "base",
  "break",
  "case",
  "catch",
  "class",
  "clone",
  "continue",
  "const",
  "default",
  "delete",
  "else",
  "enum",
  "extends",
  "for",
  "foreach",
  "function",
  "if",
  "in",
  "local",
  "null",
  "resume",
  "return",
  "switch",
  "this",
  "throw",
  "try",
  "typeof",
  "while",
  "yield",
  "constructor",
  "instanceof",
  "true",
  "false",
  "static",
};

/** Time before the script revalidates the squirrel code */
const float VALIDATE_SCRIPT_DELAY = 0.f;
/** Number of spaces in a tab */
const int TAB_SPACES = 2;

ScriptEditor* ScriptEditor::squirrel_callback_editor = nullptr;

ScriptEditor::ScriptEditor() :
  ControlTextarea(),
  m_validate_timer(0.f),
  m_err_line(),
  m_err_col(),
  m_err_msg()
{
  m_theme.active.font = Resources::editor_font;
  m_theme.base.font = Resources::editor_font;
  m_theme.hover.font = Resources::editor_font;
  m_theme.disabled.font = Resources::editor_font;
  m_theme.focused.font = Resources::editor_font;
}

void
ScriptEditor::draw(DrawingContext& context)
{
  if (!m_visible)
    return;

  ControlTextarea::draw(context);

  auto theme = get_current_theme();

  Rectf status_bar = m_rect;
  status_bar.set_top(status_bar.get_bottom() - theme.font->get_height());

  context.color().draw_filled_rect(status_bar, theme.bkg_color, LAYER_GUI + 1);
  context.color().draw_line(status_bar.p1(),
                            Vector(status_bar.p2().x,
                                   status_bar.p1().y),
                            Color(1.f, 1.f, 1.f, .25f),
                            LAYER_GUI + 2);
  context.color().draw_line(status_bar.p1() - Vector(0.f, 1.f),
                            Vector(status_bar.p2().x,
                                   status_bar.p1().y - 1.f),
                            Color(0.f, 0.f, 0.f, .25f),
                            LAYER_GUI + 2);

  std::ostringstream s;
  s << (get_line_num(m_caret_pos) + 1) << ", " << (get_xpos(m_caret_pos) + 1);
  std::string caretlabel = s.str();

  context.color().draw_text(theme.font,
                            s.str(),
                            Vector(status_bar.p2().x - 5.f, status_bar.p1().y),
                            FontAlignment::ALIGN_RIGHT,
                            LAYER_GUI + 1,
                            Color::BLACK);

  if (!m_err_msg.empty())
  {
    // Draw error message
    Rectf st = status_bar;
    st.set_right(st.get_right() - theme.font->get_text_width(caretlabel) - 10.f);

    context.push_transform();
    context.transform().clip = context.transform().clip.intersect(Rect(st));

    std::ostringstream s2;
    s2 << m_err_msg << " (line " << m_err_line << ", col " << m_err_col << ")";
    context.color().draw_text(theme.font,
                              s2.str(),
                              status_bar.p1(),
                              FontAlignment::ALIGN_LEFT,
                              LAYER_GUI + 1,
                              Color::BLACK);
    context.pop_transform();
    
    int err_line = m_err_line - 1;
    context.color().draw_line(Vector(m_rect.p1().x + 5.f - m_h_scrollbar.m_progress + theme.font->get_text_width(get_line(err_line).substr(0, m_err_col - 1)),
                                     m_rect.p1().y + 5.f - m_v_scrollbar.m_progress + theme.font->get_height() * (err_line + 1) - 4.f),
                              Vector(m_rect.p1().x + 5.f - m_h_scrollbar.m_progress + theme.font->get_text_width(get_line(err_line).substr(0, m_err_col)),
                                     m_rect.p1().y + 5.f - m_v_scrollbar.m_progress + theme.font->get_height() * (err_line + 1) - 4.f),
                              Color::RED,
                              LAYER_GUI);
  }
}

void
ScriptEditor::draw_text(DrawingContext& context)
{
  auto theme = get_current_theme();

  // Using http://www.squirrel-lang.org/doc/squirrel3.html#d0e61

  // Regular text
  Color tx_col = Color(0.f, 0.f, 0.f);

  // Keywords
  Color kw_col = Color(.3f, .1f, .4f);

  // Symbols (parentheses, brackets, semicolons, etc.)
  Color sy_col = Color(.5f, .4f, .1f);

  // Comments
  Color cm_col = Color(.1f, .4f, .0f);

  // Strings
  Color st_col = Color(.5f, .1f, .4f);

  // Numbers
  Color nb_col = Color(.1f, .4f, .5f);

  // Boolean keywords (true, false)
  Color bl_col = Color(.1f, .1f, .5f);

  float top = 0.f;
  float left = 0.f;

  int pos = 0;
  std::string s = get_contents();

  while (pos < s.length())
  {
    // Skip whitespace
    while (pos < s.length() && std::string("\n \t").find(s.at(pos)) != std::string::npos)
    {
      if (s.at(pos) == '\n')
      {
        top += theme.font->get_height();
        left = 0.f;
      }
      else
      {
        left += theme.font->get_text_width(std::string(1, s.at(pos)));
      }
      pos++;
    }

    // If we reached the end, break out
    if (pos >= s.length())
      break;

    // Attempt to read an identifier, a keyword, or an operator/symbol
    std::string word;
    bool is_word = std::string("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_").find(s.at(pos)) != std::string::npos;

    while (pos < s.length() && std::string("\n \t").find(s.at(pos)) == std::string::npos)
    {
      bool local_is_word = std::string("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_").find(s.at(pos)) != std::string::npos;

      if (local_is_word != is_word || std::string("\n \t").find(s.at(pos)) != std::string::npos)
        break;

      word.push_back(s.at(pos));
      pos++;
    }

    if (is_word)
    {
      if (math::is_in_range(get_offset_from_pos(m_err_line - 1, m_err_col - 1), pos - static_cast<int>(word.size()), pos))
      {
        // It's an error!
        context.color().draw_text(theme.font,
                                  word,
                                  Vector(m_rect.p1().x + left + 5.f - m_h_scrollbar.m_progress,
                                         m_rect.p1().y + top + 5.f - m_v_scrollbar.m_progress),
                                  FontAlignment::ALIGN_LEFT,
                                  LAYER_GUI + 1,
                                  Color::RED);
        left += theme.font->get_text_width(word);
      }
      else if (word == "true" || word == "false")
      {
        // It's a boolean constant
        context.color().draw_text(theme.font,
                                  word,
                                  Vector(m_rect.p1().x + left + 5.f - m_h_scrollbar.m_progress,
                                         m_rect.p1().y + top + 5.f - m_v_scrollbar.m_progress),
                                  FontAlignment::ALIGN_LEFT,
                                  LAYER_GUI + 1,
                                  bl_col);
        left += theme.font->get_text_width(word);
      }
      else if (std::find(keywords.begin(), keywords.end(), word) != keywords.end())
      {
        // It's a keyword
        context.color().draw_text(theme.font,
                                  word,
                                  Vector(m_rect.p1().x + left + 5.f - m_h_scrollbar.m_progress,
                                         m_rect.p1().y + top + 5.f - m_v_scrollbar.m_progress),
                                  FontAlignment::ALIGN_LEFT,
                                  LAYER_GUI + 1,
                                  kw_col);
        left += theme.font->get_text_width(word);
      }
      else if (std::string("0123456789").find(word.at(0)) != std::string::npos)
      {
        // It's a number
        context.color().draw_text(theme.font,
                                  word,
                                  Vector(m_rect.p1().x + left + 5.f - m_h_scrollbar.m_progress,
                                         m_rect.p1().y + top + 5.f - m_v_scrollbar.m_progress),
                                  FontAlignment::ALIGN_LEFT,
                                  LAYER_GUI + 1,
                                  nb_col);
        left += theme.font->get_text_width(word);
      }
      else
      {
        // It's an identifier
        context.color().draw_text(theme.font,
                                  word,
                                  Vector(m_rect.p1().x + left + 5.f - m_h_scrollbar.m_progress,
                                         m_rect.p1().y + top + 5.f - m_v_scrollbar.m_progress),
                                  FontAlignment::ALIGN_LEFT,
                                  LAYER_GUI + 1,
                                  tx_col);
        left += theme.font->get_text_width(word);
      }
    }
    else
    {
      if (word.find("//") == 0 || word.find("#") == 0)
      {
        // It's a comment, rewind to the begining of the comment (in case there
        // were other symbols after the "//") and read the whole comment.
        pos -= (word.size() - ((word.find("#") == 0) ? 1 : 2));
        word = (word.find("#") == 0) ? "#" : "//";

        bool is_backslash = false;

        while (pos < s.length())
        {
          char c = s.at(pos);

          if (!is_backslash)
          {
            if (s.at(pos) == '\\')
            {
              is_backslash = true;
            }
            else if (s.at(pos) == '\n')
            {
              break;
            }
          }
          else
          {
            // We read the escaped character, we may continue.
            // TODO: Different color for escaped characters in comments?
            is_backslash = false;
          }

          word.push_back(c);
          pos++;
        }

        bool is_error = math::is_in_range(get_offset_from_pos(m_err_line - 1, m_err_col - 1),
                                          pos - static_cast<int>(word.size()),
                                          pos);

        // Print the comment, then continue.
        while (true)
        {
          size_t pos2 = word.find('\n');
          std::string l = word.substr(0, pos2);
          context.color().draw_text(theme.font,
                                    l,
                                    Vector(m_rect.p1().x + left + 5.f - m_h_scrollbar.m_progress,
                                          m_rect.p1().y + top + 5.f - m_v_scrollbar.m_progress),
                                    FontAlignment::ALIGN_LEFT,
                                    LAYER_GUI + 1,
                                    is_error ? Color::RED : cm_col);

          if (pos2 == std::string::npos)
            break;

          top += theme.font->get_height();
          left = 0.f;

          pos2++;
          word = word.substr(pos2);
        }
      }
      else if (word.find("/*") == 0)
      {
        // It's a comment block, rewind to the begining of the block (in case
        // there were other symbols after the "/*") and read the whole block.
        pos -= (word.size() - 2);
        word = "/*";

        bool is_backslash = false;
        bool is_star = false;

        while (pos < s.length())
        {
          char c = s.at(pos);

          if (!is_backslash)
          {
            if (c == '\\')
            {
              is_backslash = true;
            }
            else if (c == '/' && is_star)
            {
              word.push_back(c);
              pos++;
              break;
            }

            if (s.at(pos) == '*')
            {
              is_star = true;
            }
            else
            {
              is_star = false;
            }
          }
          else
          {
            // We read the escaped character, we may continue.
            // TODO: Different color for escaped characters in comments?
            is_backslash = false;
            is_star = false;
          }

          word.push_back(c);
          pos++;
        }

        bool is_error = math::is_in_range(get_offset_from_pos(m_err_line - 1, m_err_col - 1),
                                          pos - static_cast<int>(word.size()),
                                          pos);

        // Print the block, then continue.
        while (true)
        {
          size_t pos2 = word.find('\n');
          std::string l = word.substr(0, pos2);
          context.color().draw_text(theme.font,
                                    l,
                                    Vector(m_rect.p1().x + left + 5.f - m_h_scrollbar.m_progress,
                                          m_rect.p1().y + top + 5.f - m_v_scrollbar.m_progress),
                                    FontAlignment::ALIGN_LEFT,
                                    LAYER_GUI + 1,
                                    is_error ? Color::RED : cm_col);

          left += theme.font->get_text_width(l);

          if (pos2 == std::string::npos)
            break;

          top += theme.font->get_height();
          left = 0.f;

          pos2++;
          word = word.substr(pos2);
        }
      }
      else if (word.find("\"") == 0 || word.find("'") == 0 || word.find("@\"") == 0)
      {
        // It's a string, rewind to the begining of the comment (in case there
        // were other symbols after the "//") and read the whole string.
        bool quote = word.find("'") == 0;
        bool verbatim = word.find("@\"") == 0;
        pos -= (word.size() - 1);
        word = word.substr(0, 1);
        if (word.at(0) == '@')
        {
          word.push_back('"');
          pos++;
        }

        bool is_backslash = false;

        while (pos < s.length())
        {
          char c = s.at(pos);

          if (!is_backslash || verbatim)
          {
            if (c == '\\' && !verbatim)
            {
              is_backslash = true;
            }
            else if (c == ((word.at(0) == '@') ? '"' : word.at(0)))
            {
              word.push_back(c);
              pos++;
              break;
            }
          }
          else
          {
            // We read the escaped character, we may continue.
            // TODO: Different color for escaped characters in strings?
            is_backslash = false;
          }

          word.push_back(c);
          pos++;
        }

        bool is_error = math::is_in_range(get_offset_from_pos(m_err_line - 1, m_err_col - 1),
                                          pos - static_cast<int>(word.size()),
                                          pos);

        // Print the string, then continue.
        while (true)
        {
          size_t pos2 = word.find('\n');
          std::string l = word.substr(0, pos2);
          context.color().draw_text(theme.font,
                                    l,
                                    Vector(m_rect.p1().x + left + 5.f - m_h_scrollbar.m_progress,
                                          m_rect.p1().y + top + 5.f - m_v_scrollbar.m_progress),
                                    FontAlignment::ALIGN_LEFT,
                                    LAYER_GUI + 1,
                                    is_error ? Color::RED : st_col);

          left += theme.font->get_text_width(l);

          if (pos2 == std::string::npos)
            break;

          top += theme.font->get_height();
          left = 0.f;

          pos2++;
          word = word.substr(pos2);
        }
      }
      else
      {
        // It's a regular symbol
        context.color().draw_text(theme.font,
                                  word,
                                  Vector(m_rect.p1().x + left + 5.f - m_h_scrollbar.m_progress,
                                         m_rect.p1().y + top + 5.f - m_v_scrollbar.m_progress),
                                  FontAlignment::ALIGN_LEFT,
                                  LAYER_GUI + 1,
                                  sy_col);
        left += theme.font->get_text_width(word);
      }
    }
  }
}

void
ScriptEditor::update(float dt_sec)
{
  if (!m_enabled)
    return;

  ControlTextarea::update(dt_sec);

  if (m_validate_timer <= VALIDATE_SCRIPT_DELAY)
  {
    m_validate_timer += dt_sec;
    if (m_validate_timer > VALIDATE_SCRIPT_DELAY)
    {
      refresh_script_validation();
    }
  }
}

bool
ScriptEditor::event(const SDL_Event& ev)
{
  if (!m_enabled)
    return false;

  int add_tabs = 0;
  if (ev.type == SDL_KEYDOWN && (ev.key.keysym.sym == SDLK_RETURN || ev.key.keysym.sym == SDLK_RETURN2))
  {
    std::string l = get_line(get_line_num(std::min(m_caret_pos, m_secondary_caret_pos)));

    while (add_tabs < l.size() && l.at(add_tabs) == ' ')
      add_tabs++;

    if (l.size() > 0 && l.at(l.size() - 1) == '{')
      add_tabs += TAB_SPACES;
  }
  else if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_TAB)
  {
    if (ev.key.keysym.mod & KMOD_SHIFT || get_line_num(m_caret_pos) != get_line_num(m_secondary_caret_pos))
    {
      int l1 = get_line_num(m_caret_pos);
      int l2 = get_line_num(m_secondary_caret_pos);
      int o1 = get_xpos(m_caret_pos);
      int o2 = get_xpos(m_secondary_caret_pos);

      if (l1 > l2)
      {
        std::swap(l1, l2);
        std::swap(o1, o2);
      }
      
      for (int l = l1; l <= l2; l++)
      {
        m_caret_pos = m_secondary_caret_pos = get_offset_from_pos(l, 0);
        if (ev.key.keysym.mod & KMOD_SHIFT)
        {
          int tabs = TAB_SPACES;
          while (tabs-- > 0 && get_contents().at(get_offset_from_pos(l, 0)) == ' ')
          {
            m_caret_pos++;
            assert(erase_selected_text());
            if (l == l1)
              o1 = std::max(o1 - 1, 0);
            if (l == l2)
              o2 = std::max(o2 - 1, 0);
          }
        }
        else
        {
          put_text(std::string(TAB_SPACES, ' '));
          if (l == l1)
            o1 += TAB_SPACES;
          if (l == l2)
            o2 += TAB_SPACES;
        }
      }

      m_caret_pos = get_offset_from_pos(l1, o1);
      m_secondary_caret_pos = get_offset_from_pos(l2, o2);
    }
    else
    {
      add_tabs += TAB_SPACES;
    }
  }

  bool r = ControlTextarea::event(ev);

  if (r)
    m_validate_timer = 0.f;

  // The If is necessary, else it's instantly delete any selection
  if (add_tabs > 0)
    put_text(std::string(add_tabs, ' '));

  return r;
}

void
ScriptEditor::refresh_script_validation()
{
  auto vm = SquirrelVirtualMachine::current()->get_vm().get_vm();

  squirrel_callback_editor = this;
  sq_setcompilererrorhandler(vm, [](HSQUIRRELVM v, const SQChar* desc, const SQChar* source,
                                    SQInteger line, SQInteger column){
    squirrel_callback_editor->set_error(line, column, std::string(desc));
  });

  m_err_msg = "";
  m_err_line = 0;
  m_err_col = 0;

  try {
    std::string code = get_contents();
    if (SQ_FAILED(sq_compilebuffer(vm, code.c_str(), code.length(), "", SQTrue)))
      throw SquirrelError(vm, "Couldn't compile command");

    sq_pop(vm, 1);
  } catch(SquirrelError& e) {
    // Expected; handled in the handler above
  } catch(std::exception& e) {
    log_warning << "Unexpected exception while compiling script in editor: " << e.what() << std::endl;
  } catch(...) {
    log_warning << "Unexpected error while compiling script in editor" << std::endl;
  }

  sq_setcompilererrorhandler(vm, nullptr);
  squirrel_callback_editor = nullptr;
}

/* EOF */
