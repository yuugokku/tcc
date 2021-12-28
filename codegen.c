/*
 * codegen.c: コードジェネレータ
 * */

#include "tcc.h"

// 抽象構文木のノードの種類
enum NodeKind{
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_NUM, // 整数
    ND_EQ,  // ==
    ND_NEQ, // !=
    ND_LE,  // <=
    ND_LT,  // <
};

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
    NodeKind kind;      // ノードの型
    Node *lhs;          // 左辺
    Node *rhs;          // 右辺
    int val;            // kindがND_NUMの場合のみ使う
};

void gen(Node *node) {
    if (node->kind == ND_NUM) {
        printf("  push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind) {
        case ND_ADD:
            printf("  add rax, rdi\n");
            break;
        case ND_SUB:
            printf("  sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("  imul rax, rdi\n");
            break;
        case ND_DIV:
            // x86-64の面倒くさい仕様による
            printf("  cqo\n");  // RAXレジスタの64ビットの値を128ビットに伸ばして、RDXとRAXにセットする

            // RAXレジスタとRDXをあわせた128ビット整数を、引数のレジスタの64ビットの値で割り、
            // 商をRAXに、余りをRDXにセットする
            printf("  idiv rdi\n");  
            break;
        case ND_EQ:
            printf("  cmp rax, rdi\n");
            printf("  sete al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_NEQ:
            printf("  cmp rax, rdi\n");
            printf("  setne al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_LT:
            printf("  cmp rax, rdi\n");
            printf("  setl al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_LE:
            printf("  cmp rax, rdi\n");
            printf("  setle al\n");
            printf("  movzb rax, al\n");
            break;
    }

    printf("  push rax\n");
}

// 二項演算子のノードを作成
Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

// 数値のノードを作成
Node *new_node_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

// expr := equality
Node *expr() {
    return equality();
}

// equality := relational ('==' relational | '!=' relational)*
Node *equality() {
    Node *node = relational();

    for (;;) {
        if (consume("==")){
            node = new_node(ND_EQ, node, relational());
        } else if(consume("!="))
            node = new_node(ND_NEQ, node, relational());
        else
            return node;
    }
}

// relational := add ('<' add | '<=' add | '>' add | '>=' add )*
Node *relational() {
    Node *node = add();

    for (;;) {
        if(consume("<"))
            node = new_node(ND_LT, node, add());
        else if(consume("<="))
            node - new_node(ND_LE, node, add());
        else if(consume(">"))
            node = new_node(ND_LT, add(), node);
        else if(consume(">="))
            node = new_node(ND_LE, add(), node);
        else
            return node;
    }
}

// expr := mul ('+' mul | '-' mul)*
Node *add() {
    Node *node = mul();

    for (;;) {
        if(consume("+"))
            node = new_node(ND_ADD, node, mul());
        else if (consume("-"))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

// primary := num | '(' expr ')'
Node *primary() {
    // 次のトークンが"("なら、"(" expr ")"のはず
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    // そうでなければ数値のはず
    return new_node_num(expect_number());
}

// mul := unary ('*' unary | '/' unary)*
Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume("*"))
            node = new_node(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
    }
}

// unary := ('-' | '+')? primary
Node *unary() {
    if (consume("+"))
        return primary();
    if (consume("-"))
        return new_node(ND_SUB, new_node_num(0), primary());
    return primary();
}


