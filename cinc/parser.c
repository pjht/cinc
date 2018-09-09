//
//  parser.c
//  cinc
//
//  Created by Peter Terpstra on 9/9/18.
//  Copyright © 2018 Peter Terpstra. All rights reserved.
//

#include "parser.h"
#include "token.h"
#include "ast.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static Token lahead;

static void advance() {
    lahead=*(lahead.next);
}

static void match(unsigned char type) {
    if (lahead.type!=type) {
        exit(1);
    }
    advance();
}

static const char* getid() {
    if (lahead.type!=TYPE_IDENT) {
        exit(1);
    }
    const char* id=lahead.val->strval;
    advance();
    return id;
}

static const char* get_num() {
    if (lahead.type!=TYPE_NUM) {
        exit(1);
    }
    const char* num=lahead.val->strval;
    advance();
    return num;
}

static const char* gettype() {
    if (lahead.type!=TYPE_TYPE) {
        exit(1);
    }
    const char* id=lahead.val->strval;
    advance();
    return id;
}

static AstNode* expr() {
    AstNode* expr_root=make_node("num");
    add_child(expr_root, make_node(get_num()));
    return expr_root;
}

static AstNode* statement() {
    switch (lahead.type) {
        case TYPE_RETURN: {
            match(TYPE_RETURN);
            AstNode* return_root=make_node("return");
            add_child(return_root, expr());
            match(';');
            return return_root;
        }
        default:
            printf("Error: Expected statement");
            exit(1);
    }
}
    

static AstNode* block() {
    match('{');
    AstNode* block_root=make_node("block");
    while (lahead.type!='}') {
        add_child(block_root, statement());
    }
    match('}');
    return block_root;
}

static AstNode* func() {
    const char* type=gettype();
    const char* name=getid();
    match('(');
    match(')');
    AstNode* func_block=block();
    AstNode* func_root=make_node("func");
    add_child(func_root, make_node(type));
    add_child(func_root, make_node(name));
    add_child(func_root, func_block);
    return func_root;
}

AstNode* parse(Token* prg) {
    lahead=*(prg);
    return func();
}

