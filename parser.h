//
//  parser.h
//  cinc
//
//  Created by Peter Terpstra on 9/9/18.
//  Copyright © 2018 Peter Terpstra. All rights reserved.
//

#ifndef parser_h
#define parser_h

#include "token.h"
#include "ast.h"

AstNode* parse(Token* prg);

#endif /* parser_h */
