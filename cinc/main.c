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

int main(int argc, const char * argv[]) {
    char* prgstr="int main() {\nreturn 0;\n}";
    Token* tokens=tokenize(prgstr);
    AstNode* ast=parse(tokens);
    free_toklist(tokens);
}
