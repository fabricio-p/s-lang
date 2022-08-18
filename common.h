#ifndef SLANG_COMMON_H
#define SLANG_COMMON_H
#include <stdlib.h>

/*
 * Keywords: fc, var, box, if, elif, else, for, whl, end
 * Operators:
 *   Unary: !, -
 *   Binary: +, -, *, /
 *   Comparison: >, =, <, >=, ==, <=
 *   Logical: and, or
 */

typedef enum   status_et   Status;
typedef struct sizedstr_st SizedStr;
typedef struct pos_st      Pos;
typedef struct span_st     Span;

typedef enum   tokenkind_et TokenKind;
typedef struct token_st     Token;
typedef struct lexer_st     Lexer;

typedef struct valtype_st      ValType;
typedef enum   litkind_et      LitKind;
typedef struct lit_st          Lit;
typedef enum   opkind_et       OpKind;
typedef struct op_st           Op;
typedef enum   exprkind_et     ExprKind;
typedef struct expr_st         Expr;
typedef struct field_st        Field;
typedef struct field_st        *Params;
typedef struct field_st        *Fields;
typedef struct fcdecl_st       FcDecl;
typedef struct fcdecl_st       FcExpr;
typedef struct vardecl_st      VarDecl;
typedef struct boxdecl_st      BoxDecl;
typedef struct condit_st       Condit;
typedef struct ifstmt_st       IfStmt;
typedef struct forloop_st      ForLoop;
typedef struct whlloop_st      WhlLoop;
typedef enum   stmtkind_et     StmtKind;
typedef struct stmt_st         Stmt;
typedef struct stmt_st         *Block;
typedef enum   toplevelkind_et TopLevelKind;
typedef struct toplevel_st     TopLevel;
typedef struct toplevel_st     *Program;
typedef struct errrep_st       ErrRep;
typedef struct ast_st          AST;

enum status_et {
  S_OK = 0,
  S_OOM,
  S_INVALID_CHAR,
  S_UNTERMINATED_STRING
};

struct sizedstr_st {
  char const *buf;
  ssize_t    len;
};

struct pos_st {
  int offset;
  int line;
  int col;
};

struct span_st {
  Pos s;
  Pos e;
};

#define _restr __restrict


#endif /* SLANG_COMMON_H */
