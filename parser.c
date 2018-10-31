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
#define PARSER_DEBUG 1

static Token* lahead;

static void advance() {
#if PARSER_DEBUG
    printf("Consumed:\n");
    print_tok(lahead);
#endif
    lahead=lahead->next;
}

static void match(unsigned char type) {
    if (lahead->type!=type) {
        if (type<128) {
            printf("Expected %c, got ",type);
        } else {
            printf("Expected %d, got ",type);
        }
        print_tok(lahead);
        exit(1);
    }
    advance();
}

static const char* getid() {
    if (lahead->type!=TYPE_IDENT) {
        printf("Expected IDENT, got ");
        print_tok(lahead);
        exit(1);
    }
    const char* id=lahead->val->strval;
    advance();
    return id;
}

static const char* get_num() {
    if (lahead->type!=TYPE_NUM) {
        printf("Expected NUM, got ");
        print_tok(lahead);
        exit(1);
    }
    const char* num=lahead->val->strval;
    advance();
    return num;
}

static const char* gettype() {
    if (lahead->type!=TYPE_TYPE) {
        printf("Expected TYPE, got ");
        print_tok(lahead);
        exit(1);
    }
    const char* id=lahead->val->strval;
    advance();
    return id;
}

static AstNode* expr(void);

static AstNode* factor_lvl1() {
    AstNode* factor_root;
    switch (lahead->type) {
        case '(':
            match('(');
            factor_root=expr();
            match(')');
            break;
        case TYPE_NUM:
            factor_root=make_node("num");
            add_child(factor_root, make_node(get_num()));
            break;
        case TYPE_IDENT:
            factor_root=make_node("var");
            add_child(factor_root, make_node(getid()));
            break;
        default:
            printf("Error: Expected factor\n");
            exit(1);
    }
    if (lahead->type==TYPE_INC || lahead->type==TYPE_DEC) {
        unsigned char type=lahead->type;
        match(lahead->type);
        AstNode* old_root=factor_root;
        switch (type) {
            case TYPE_INC:
                factor_root=make_node("postinc");
                add_child(factor_root, old_root);
                break;
            case TYPE_DEC:
                factor_root=make_node("postdec");
                add_child(factor_root, old_root);
                break;
        }
        
    }
    return factor_root;
}

static AstNode* factor() {
    AstNode* factor_root;
    switch (lahead->type) {
        case '-':
            match('-');
            factor_root=make_node("neg");
            add_child(factor_root, factor_lvl1());
            break;
        case '~':
            match('~');
            factor_root=make_node("comp");
            add_child(factor_root, factor_lvl1());
            break;
        case '!':
            match('!');
            factor_root=make_node("not");
            add_child(factor_root, factor_lvl1());
            break;
        case TYPE_INC:
            match(TYPE_INC);
            factor_root=make_node("preinc");
            add_child(factor_root, factor_lvl1());
            break;
        case TYPE_DEC:
            match(TYPE_DEC);
            factor_root=make_node("predec");
            add_child(factor_root, factor_lvl1());
            break;
        default:
            return factor_lvl1();
    }
    return factor_root;
}


static AstNode* term() {
    AstNode* term_root=factor();
    while (lahead->type=='*' || lahead->type=='/' || lahead->type=='%') {
        unsigned char type=lahead->type;
        match(lahead->type);
        AstNode* term1=factor();
        switch (type) {
            case '*': {
                AstNode* old_root=term_root;
                term_root=make_node("mul");
                add_child(term_root, old_root);
                add_child(term_root, term1);
                break;
            }
            case '/': {
                AstNode* old_root=term_root;
                term_root=make_node("div");
                add_child(term_root, old_root);
                add_child(term_root, term1);
                break;
            }
            case '%': {
                AstNode* old_root=term_root;
                term_root=make_node("mod");
                add_child(term_root, old_root);
                add_child(term_root, term1);
                break;
            }
        }
    }
    return term_root;
}

static AstNode* arithexpr() {
    AstNode* expr_root=term();
    while (lahead->type=='+' || lahead->type=='-') {
        unsigned char type=lahead->type;
        match(lahead->type);
        AstNode* term1=term();
        switch (type) {
            case '+': {
                AstNode* old_root=expr_root;
                expr_root=make_node("add");
                add_child(expr_root, old_root);
                add_child(expr_root, term1);
                break;
            }
            case '-': {
                AstNode* old_root=expr_root;
                expr_root=make_node("sub");
                add_child(expr_root, old_root);
                add_child(expr_root, term1);
                break;
            }
        }
    }
    return expr_root;
}


static AstNode* shiftexpr() {
    AstNode* expr_root=arithexpr();
    while (lahead->type==TYPE_SL || lahead->type==TYPE_SR) {
        unsigned char type=lahead->type;
        match(lahead->type);
        AstNode* expr1=arithexpr();
        switch (type) {
            case TYPE_SL: {
                AstNode* old_root=expr_root;
                expr_root=make_node("sal");
                add_child(expr_root, old_root);
                add_child(expr_root, expr1);
                break;
            }
            case TYPE_SR: {
                AstNode* old_root=expr_root;
                expr_root=make_node("sar");
                add_child(expr_root, old_root);
                add_child(expr_root, expr1);
                break;
            }
        }
    }
    return expr_root;
}

static AstNode* relexpr() {
    AstNode* expr_root=shiftexpr();
    while (lahead->type=='<' || lahead->type=='>'|| lahead->type==TYPE_LE|| lahead->type==TYPE_GE) {
        unsigned char type=lahead->type;
        match(lahead->type);
        AstNode* expr1=shiftexpr();
        switch (type) {
            case '<': {
                AstNode* old_root=expr_root;
                expr_root=make_node("lt");
                add_child(expr_root, old_root);
                add_child(expr_root, expr1);
                break;
            }
            case '>': {
                AstNode* old_root=expr_root;
                expr_root=make_node("gt");
                add_child(expr_root, old_root);
                add_child(expr_root, expr1);
                break;
            }
            case TYPE_LE: {
                AstNode* old_root=expr_root;
                expr_root=make_node("le");
                add_child(expr_root, old_root);
                add_child(expr_root, expr1);
                break;
            }
            case TYPE_GE: {
                AstNode* old_root=expr_root;
                expr_root=make_node("ge");
                add_child(expr_root, old_root);
                add_child(expr_root, expr1);
                break;
            }
        }
    }
    return expr_root;
}

static AstNode* eqexpr() {
    AstNode* expr_root=relexpr();
    while (lahead->type==TYPE_NE || lahead->type==TYPE_EQ) {
        unsigned char type=lahead->type;
        match(lahead->type);
        AstNode* expr1=relexpr();
        switch (type) {
            case TYPE_NE: {
                AstNode* old_root=expr_root;
                expr_root=make_node("ne");
                add_child(expr_root, old_root);
                add_child(expr_root, expr1);
                break;
            }
            case TYPE_EQ: {
                AstNode* old_root=expr_root;
                expr_root=make_node("eq");
                add_child(expr_root, old_root);
                add_child(expr_root, expr1);
                break;
            }
        }
    }
    return expr_root;
}

static AstNode* andexpr() {
    AstNode* expr_root=eqexpr();
    while (lahead->type=='&') {
        match(lahead->type);
        AstNode* expr1=eqexpr();
        AstNode* old_root=expr_root;
        expr_root=make_node("and");
        add_child(expr_root, old_root);
        add_child(expr_root, expr1);
    }
    return expr_root;
}

static AstNode* xorexpr() {
    AstNode* expr_root=andexpr();
    while (lahead->type=='^') {
        match(lahead->type);
        AstNode* expr1=andexpr();
        AstNode* old_root=expr_root;
        expr_root=make_node("xor");
        add_child(expr_root, old_root);
        add_child(expr_root, expr1);
    }
    return expr_root;
}

static AstNode* orexpr() {
    AstNode* expr_root=xorexpr();
    while (lahead->type=='|') {
        match(lahead->type);
        AstNode* expr1=xorexpr();
        AstNode* old_root=expr_root;
        expr_root=make_node("or");
        add_child(expr_root, old_root);
        add_child(expr_root, expr1);
    }
    return expr_root;
}

static AstNode* landexpr() {
    AstNode* expr_root=orexpr();
    while (lahead->type==TYPE_LAND) {
        match(lahead->type);
        AstNode* expr1=orexpr();
        AstNode* old_root=expr_root;
        expr_root=make_node("land");
        add_child(expr_root, old_root);
        add_child(expr_root, expr1);
    }
    return expr_root;
}

static AstNode* lorexpr() {
    AstNode* expr_root=landexpr();
    while (lahead->type==TYPE_LOR) {
        match(lahead->type);
        AstNode* expr1=landexpr();
        AstNode* old_root=expr_root;
        expr_root=make_node("lor");
        add_child(expr_root, old_root);
        add_child(expr_root, expr1);
    }
    return expr_root;
}

static AstNode* condexpr() {
    AstNode* expr_root=make_node("cond");
    AstNode* lorexp=lorexpr();
    if (lahead->type=='?') {
        add_child(expr_root, lorexp);
    } else {
        return lorexp;
    }
    match('?');
    add_child(expr_root, expr());
    match(':');
    add_child(expr_root, condexpr());
    return expr_root;
}

static AstNode* expr() {
    if (lahead->type==TYPE_IDENT) {
        if (lahead->next->type=='=') {
            const char* id=getid();
            match('=');
            AstNode* val=expr();
            AstNode* expr_root=make_node("assign");
            add_child(expr_root, make_node(id));
            add_child(expr_root, val);
            return expr_root;
        } else if (lahead->next->type==TYPE_COMPSET) {
            const char* id=getid();
            char operator=*(lahead->val->strval);
            match(lahead->type);
            //Construct a sequence of tokens effecting <id>=<id><operator><link right here>
            Token* tok=new_token(TYPE_IDENT, val_from_const_str(id), NULL);
            Token* first=tok;
            tok=new_token('=', NULL, tok);
            tok=new_token(TYPE_IDENT, val_from_const_str(id), tok);
            tok=new_token(operator, NULL, tok);
            //Link in the generated token stream
            print_tok(lahead);
            tok->next=lahead;
            print_tok(tok->next);
            lahead=first;
            print_tok(tok->next);
            return expr();
        } else {
            return condexpr();
        }
    } else {
        return condexpr();
    }
}

static AstNode* exp_option() {
    if (lahead->type==';' || lahead->type==')') {
        return NULL;
    } else {
        return expr();
    }
}

static AstNode* block(void);
static AstNode* declaration(void);

static AstNode* statement() {
    switch (lahead->type) {
        case TYPE_RETURN: {
            match(TYPE_RETURN);
            AstNode* return_root=make_node("return");
            add_child(return_root, expr());
            match(';');
            return return_root;
        }
        case TYPE_IF: {
            match(TYPE_IF);
            AstNode* if_root=make_node("if"); // We dont know whether it's an if only or a if-else, so we assume if only.
            match('(');
            add_child(if_root, expr());
            match(')');
            add_child(if_root, statement());
            if (lahead->type==TYPE_ELSE) {
                if_root->data="ifelse";
                match(TYPE_ELSE);
                add_child(if_root, statement());
            }
            return if_root;
        }
        case TYPE_FOR: {
            match(TYPE_FOR);
            match('(');
            AstNode* for_root=make_node("for");
            if (lahead->type==TYPE_TYPE) {
                add_child(for_root, make_node("decl"));
                add_child(for_root, declaration());
            } else {
                add_child(for_root, make_node("exp"));
                add_child(for_root, exp_option());
                match(';');
            }
            add_child(for_root, exp_option());
            match(';');
            add_child(for_root, exp_option());
            match(')');
            add_child(for_root, statement());
            return for_root;
        }
        case TYPE_WHILE: {
            match(TYPE_WHILE);
            match('(');
            AstNode* while_root=make_node("while");
            add_child(while_root, expr());
            match(')');
            add_child(while_root, statement());
            return while_root;
        }
        case TYPE_DO: {
            match(TYPE_DO);
            AstNode* dowhile_root=make_node("dowhile");
            AstNode* statm=statement();
            match(TYPE_WHILE);
            add_child(dowhile_root, expr());
            add_child(dowhile_root, statm);
            match(';');
            return dowhile_root;
        }
        case TYPE_BREAK:
            match(TYPE_BREAK);
            match(';');
            return make_node("break");
        case TYPE_CONTINUE:
            match(TYPE_CONTINUE);
            match(';');
            return make_node("continue");
        case '{':
            return block();
        default: {
            AstNode* statement_root=exp_option();
            match(';');
            return statement_root;
//            printf("Error: Expected statement\n");
//            exit(1);
        }
    }
}

static AstNode* declaration() {
    const char* type=gettype();
    const char* id=getid();
    AstNode* decl_root;
    if (lahead->type=='=') {
        match('=');
        AstNode* decl_val=expr();
        match(';');
        decl_root=make_node("vardecinitial");
        add_child(decl_root, make_node(type));
        add_child(decl_root, make_node(id));
        add_child(decl_root, decl_val);
    } else {
        match(';');
        decl_root=make_node("vardec");
        add_child(decl_root, make_node(type));
        add_child(decl_root, make_node(id));;
    }
    return decl_root;
}

static AstNode* block_item() {
    if (lahead->type==TYPE_TYPE) {
        return declaration();
    } else {
        return statement();
    }
}

static AstNode* block() {
    match('{');
    AstNode* block_root=make_node("block");
    while (lahead->type!='}') {
        AstNode* item=block_item();
        if (item) {
            add_child(block_root, item);
        }
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
    lahead=prg;
    return func();
}

