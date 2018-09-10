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

#define PARSER_DEBUG 0

int main(int argc, const char * argv[]) {
    char* prgstr="int main() {\n  return 173;\n}\n";
    if (PARSER_DEBUG) {
        printf("Program:\n");
        printf("%s",prgstr);
    }
    Token* tokens=tokenize(prgstr);
    if (PARSER_DEBUG) {
        printf("Tokens:\n");
        Token* tok=tokens;
        while (tok) {
            print_tok(tok);
            tok=tok->next;
        }
    }
    AstNode* ast=parse(tokens);
    free_toklist(tokens);
    if (PARSER_DEBUG) {
        printf("AST:\n");
        print_tree(ast, 0);
    }
    char* prg=generate_prg(ast);
    free_tree(ast);
    if (PARSER_DEBUG) {
        printf("Output assembly:\n");
        printf("%s",prg);
    }
    FILE* outfile=fopen("/Users/peterterpstra/Desktop/projects/xcode/cinc/cinc/out.mys","w");
    fputs(prg, outfile);
    fclose(outfile);
    free(prg);
}
