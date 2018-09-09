//
//  token.h
//  cinc
//
//  Created by Peter Terpstra on 9/7/18.
//  Copyright © 2018 Peter Terpstra. All rights reserved.
//

#ifndef token_h
#define token_h

#include <stdbool.h>

typedef enum {
    intval,
    strval
} TokenValType;

typedef struct {
    TokenValType type;
    bool constflag;
    union {
        int intval;
        char* strval;
    };
} TokenVal;


#define TYPE_IDENT 128
#define TYPE_NUM 129
#define TYPE_EOF 130
#define TYPE_RETURN 131
#define TYPE_TYPE 132

struct _token {
    unsigned char type;
    TokenVal* val;
    struct _token* next;
};

typedef struct _token Token;

Token* new_token(unsigned char type,TokenVal* val, Token* prev);
void print_tok(Token* tok);
TokenVal* val_from_int(int val);
TokenVal* val_from_const_str(const char* val);
TokenVal* val_from_str(char* val);
#endif /* token_h */
