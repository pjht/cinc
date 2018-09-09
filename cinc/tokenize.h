//
//  tokenize.h
//  cinc
//
//  Created by Peter Terpstra on 9/7/18.
//  Copyright © 2018 Peter Terpstra. All rights reserved.
//

#ifndef tokenize_h
#define tokenize_h

#include "token.h"

Token* tokenize(char* prg);

void free_toklist(Token* tokens);
#endif /* tokenize_h */
