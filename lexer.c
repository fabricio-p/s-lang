#include <stdio.h>
#include <string.h>
#include "lexer.h"

static void skip_space(char const **ofs_p, char const *const end,
                       Pos *pos)
{
  char const *ofs = *ofs_p;
  for (char _c; ofs < end; ++ofs, ++pos->offset)
  {
    _c = *ofs;
    if (_c == '\n')
    {
      ++pos->line;
      pos->col = 1;
    } else if (_c != '\t' && _c != '\r' && _c != ' ')
    {
      break;
    }
  }
  *ofs_p = ofs;
}

// TODO: Check shit for invalid number literals and stuff
static TokenKind lex_number(char const **ofs_p, char const *const end)
{
  TokenKind kind = TK_INT;
  char const *ofs = *ofs_p;
  for (char _c; ofs < end; ++ofs)
  {
    _c = *ofs;
    if ((_c < '0' || _c > '9') && (_c != '_') &&
        !((_c >= 'a' && _c <= 'z') || (_c >= 'A' && _c <= 'Z')))
    {
      if (_c == '.' && kind != TK_FLOAT)
      {
        kind = TK_FLOAT;
      } else
      {
        break;
      }
    }
  }
  *ofs_p = ofs;
  return kind;
}

static void lex_ident(char const **ofs_p, char const *const end)
{
  char const *ofs = *ofs_p;
  for (char _c; ofs < end; ++ofs)
  {
    _c = *ofs;
    if (!(_c == '_' || (_c >= '0' && _c <= '9') ||
          (_c >= 'A' && _c <= 'Z') || (_c >= 'a' && _c <= 'z')))
    {
      break;
    }
  }
  *ofs_p = ofs;
}

static Status lex_string(char const **ofs_p, char const *const end,
                         Pos *endpos)
{
  Status status = S_UNTERMINATED_STRING;
  char const *ofs = *ofs_p;
  for (char _c; ofs < end; ++ofs, ++endpos->offset, ++endpos->col)
  {
    _c = *ofs;
    if (_c == '\\')
    {
      ++ofs;
    } else if (_c == '"')
    {
      status = S_OK;
      break;
    } else if (_c == '\n')
    {
      ++endpos->line;
      endpos->col = 0;
    }
  }
  *ofs_p = ++ofs;
  endpos->offset += 2;
  endpos->col += 2;
  return status;
}

static TokenKind ident_or_keyword(char const *const start,
                                  char const *const end)
{
  int len = end - start;
  TokenKind kind = TK_IDENT;

  if (len == 2)
  {
    if (start[0] == 'f' && start[1] == 'c')
    {
      kind = TK_FC;
    } else if (start[0] == 'i' && start[1] == 'f')
    {
      kind = TK_IF;
    }
  } else if (len == 3)
  {
    if (memcmp(start, "loc", 3) == 0)
    {
      kind = TK_LOC;
    } else if (memcmp(start, "for", 3) == 0)
    {
      kind = TK_FOR;
    } else if (memcmp(start, "whl", 3) == 0)
    {
      kind = TK_WHL;
    } else if (memcmp(start, "end", 3) == 0)
    {
      kind = TK_END;
    } else if (memcmp(start, "box", 3) == 0)
    {
      kind = TK_BOX;
    } else if (memcmp(start, "ret", 3) == 0)
    {
      kind = TK_RET;
    }
  }
  return kind;
}

void Lexer_init(Lexer *lexer, SizedStr src)
{
  lexer->src = src;
  lexer->ofs = src.buf;
  lexer->_next_status = S_OK;
  lexer->next.span.s = (Pos) {0, 1, 1};
  lexer->next.span.e = (Pos) {0, 1, 1};
  lexer->pos = (Pos) {0, 1, 1};
  lexer->next.kind = 0;
  Lexer_next(lexer, NULL);
}

Status Lexer_next(Lexer *lexer, Token *token)
{
  Status status = lexer->_next_status;
  char const *ofs = lexer->ofs;
  char const *const end = lexer->src.buf + lexer->src.len;
  if (token != NULL)
  {
    *token = lexer->next;
  }
  if (status == S_OK)
  {
    lexer->pos = lexer->next.span.e;
    skip_space(&ofs, end, &lexer->next.span.e);
    lexer->next.span.s = lexer->next.span.e;
#ifdef TEST
    lexer->next.start = ofs;
#endif /* TEST */
    if (ofs == end)
    {
      if (lexer->next.kind != TK_EOF)
      {
        lexer->next.kind = TK_EOF;
      }
    } else
    {
      Token *next = &lexer->next;
      char _c = *ofs++;
      switch (_c)
      {
        case '<':
        case '=':
        case '>': {
          char const char2opkind[] = {
#define CMP(c, k) [c - '<'] = TK_OP_##k
            CMP('<', LT), CMP('=', ASSIGN), CMP('>', GT)
#undef CMP
          };
          next->kind = char2opkind[_c - '<'];
          if (*ofs == '=')
          {
            ++ofs;
            next->kind += TK_OP_GE - TK_OP_GT /* (3) */;
          }
          break;
        }
        case '!':
          if (*ofs == '=')
          {
            ++ofs;
            next->kind = TK_OP_NEQ;
          } else {
            next->kind = TK_OP_NOT;
          }
          break;
        case '"':
          next->kind = TK_STR;
          lexer->_next_status = lex_string(&ofs, end, &next->span.e);
          break;
        case '\'':
          next->kind = TK_CHAR;
          for (; ofs < end; ++ofs)
          {
            _c = *ofs;
            if (_c == '\'')
            {
              ++ofs;
              break;
            } else if (_c == '\\')
            {
              ++ofs;
            } else if (_c == '\n')
            {
              ++lexer->pos.line;
              lexer->pos.col = 1;
            }
          }
          break;
        case '0'...'9':
          next->kind = lex_number(&ofs, end);
          break;
        case 'a'...'z':
        case 'A'...'Z':
        case '_': {
          char const *start = ofs - 1;
          lex_ident(&ofs, end);
          next->kind = ident_or_keyword(start, ofs);
          break;
        }
#define CHAR(c, k) case c: next->kind = TK_##k; break
        CHAR('+', OP_PLUS);
        CHAR('-', OP_MINUS);
        CHAR('*', OP_ASTERISK);
        CHAR('/', OP_SLASH);
        CHAR('?', QMARK);
        CHAR(':', COLON);
        CHAR(',', COMMA);
        CHAR(';', SEMICOLON);
        CHAR('(', LPAREN);
        CHAR(')', RPAREN);
        CHAR('[', LBRACKET);
        CHAR(']', RBRACKET);
        CHAR('{', LBRACE);
        CHAR('}', RBRACE);
#undef CHAR
        default:
          lexer->_next_status = S_INVALID_CHAR;
          next->kind = 0;
      }
    }
    lexer->ofs = ofs;
  }
  if (lexer->next.kind != TK_STR && lexer->next.kind != TK_EOF)
  {
    int prev_offset = lexer->next.span.e.offset;
    lexer->next.span.e.offset = ofs - lexer->src.buf;
    lexer->next.span.e.col += lexer->next.span.e.offset - prev_offset;
  }
  return status;
}
