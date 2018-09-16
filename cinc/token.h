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
#define TYPE_LAND 133
#define TYPE_LOR 134
#define TYPE_EQ 135
#define TYPE_NE 136
#define TYPE_LE 137
#define TYPE_GE 138
#define TYPE_SL 139
#define TYPE_SR 140
#define TYPE_COMPSET 141
#define TYPE_INC 142
#define TYPE_DEC 143
#define TYPE_IF 144
#define TYPE_ELSE 145
#define TYPE_FOR 146
#define TYPE_WHILE 147
#define TYPE_DO 148
#define TYPE_BREAK 149
#define TYPE_CONTINUE 150
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
