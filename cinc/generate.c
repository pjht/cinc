//
//  generate.c
//  cinc
//
//  Created by Peter Terpstra on 9/9/18.
//  Copyright © 2018 Peter Terpstra. All rights reserved.
//

#include "generate.h"
#include "ast.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char* prg_asm;

static void generate_expr(AstNode* ast) {
    const char* type=ast->data;
    if(strcmp("num", type)==0) {
        strncat(prg_asm, "mov rax,", 8);
        strncat(prg_asm,ast->children[0]->data,strlen(ast->children[0]->data));
        strncat(prg_asm, "\n", 1);
    }
}


static void generate_statement(AstNode* ast) {
    const char* type=ast->data;
    if(strcmp("return", type)==0) {
        generate_expr(ast->children[0]);
        strncat(prg_asm, "ret\n", 4);
    }
}

static void generate_block(AstNode* ast) {
    for (int i=0;i<(ast->num_children);i++) {
        generate_statement(ast->children[i]);
    }
}


static void generate_func(AstNode* ast) {
    const char* name=ast->children[1]->data;
    strncat(prg_asm, ".globl _", 8);
    strncat(prg_asm, name, strlen(name));
    strncat(prg_asm, "\n_", 2);
    strncat(prg_asm, name, strlen(name));
    strncat(prg_asm, ":\n", 2);
    generate_block(ast->children[2]);
}


char* generate_prg(AstNode* ast) {
    prg_asm=malloc(sizeof(char));
    prg_asm[0]=0;
    generate_func(ast);
    return prg_asm;
}
