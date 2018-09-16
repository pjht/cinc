//
//  main.c
//  cinc
//
//  Created by Peter Terpstra on 9/7/18.
//  Copyright © 2018 Peter Terpstra. All rights reserved.
//

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "tokenize.h"
#include "ast.h"
#include "parser.h"
#include "generate.h"

#define COMPILER_DEBUG 1

int main(int argc, const char * argv[]) {
    char* prgstr="int main(){int j=0;for(int i=0;i<7;i++){if(i==5){break;}j+=2;}return j;}";
    #if COMPILER_DEBUG
        printf("Program:\n");
        printf("%s",prgstr);
    #endif
    Token* tokens=tokenize(prgstr);
    #if COMPILER_DEBUG
        printf("Tokens:\n");
        Token* tok=tokens;
        while (tok) {
            print_tok(tok);
            tok=tok->next;
        }
    #endif
    AstNode* ast=parse(tokens);
    #if COMPILER_DEBUG
        printf("AST:\n");
        print_tree(ast, 0);
    #endif
    char* prg=generate_prg(ast);
    #if COMPILER_DEBUG
        printf("Output assembly:\n");
        printf("%s",prg);
    #endif
    FILE* outfile=fopen("/Users/peterterpstra/Desktop/projects/xcode/cinc/cinc/out.s","w");
    //compile with gcc -masm=intel out.s -o out
    fputs(prg, outfile);
    fclose(outfile);
    free(prg);
    free_tree(ast);
    free_toklist(tokens);
}
