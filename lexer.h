#ifndef SLANG_LEXER_H
#define SLANG_LEXER_H
#include "common.h"

enum tokenkind_et {
  TK_EOF = 1,
  TK_OP_PLUS,
  TK_OP_MINUS,
  TK_OP_ASTERISK,
  TK_OP_SLASH,
  TK_OP_GT,
  TK_OP_LT,
  TK_OP_ASSIGN,
  TK_OP_GE,
  TK_OP_LE,
  TK_OP_EQ,
  TK_OP_NOT,
  TK_OP_NEQ,
  TK_OP_AND,
  TK_OP_OR,
  TK_QMARK,
  TK_COLON,
  TK_COMMA,
  TK_SEMICOLON,
  TK_IDENT,
  TK_INT,
  TK_CHAR,
  TK_FLOAT,
  TK_STR,
  TK_FC,
  TK_LOC,
  TK_BOX,
  TK_IF,
  TK_ELIF,
  TK_ELSE,
  TK_FOR,
  TK_WHL,
  TK_END,
  TK_RET,
  TK_LPAREN,
  TK_RPAREN,
  TK_LBRACKET,
  TK_RBRACKET,
  TK_LBRACE,
  TK_RBRACE
};

struct token_st {
#ifdef TEST
  char const *start;
#endif /* TEST */
  Span      span;
  TokenKind kind;
};

struct lexer_st {
  SizedStr   src;
  char const *ofs;
  Token      next;
  Pos        pos;
  Status     _next_status;
};

void   Lexer_init(Lexer *, SizedStr);
Status Lexer_next(Lexer *, Token *);

#endif /* SLANG_LEXER_H */
