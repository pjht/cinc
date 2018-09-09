//
//  parser.c
//  cinc
//
//  Created by Peter Terpstra on 9/9/18.
//  Copyright © 2018 Peter Terpstra. All rights reserved.
//

#include "parser.h"
#include "token.h"
#include <stdlib.h>

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

static int get_num() {
    if (lahead.type!=TYPE_NUM) {
        exit(1);
    }
    int num=lahead.val->intval;
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

static char** func() {
    const char* type=gettype();
    const char* name=getid();
    match('(');
    match(')');
    match('{');
    //block code here
    match('}');
    //return ast here
}

char** parse(Token* prg) {
    lahead=*(prg);
    return func();
}

