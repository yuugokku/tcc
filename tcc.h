#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>


char *user_input;

void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);

struct Token;
typedef struct Token Token;

enum TokenKind;
typedef enum TokenKind TokenKind;

Token *token;

Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize(char *p);

bool consume(char *op);
void expect(char *op);
int expect_number();
bool at_eof();

struct Node;
typedef struct Node Node;
enum NodeKind;
typedef enum NodeKind NodeKind;

void gen(Node *node);
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();


