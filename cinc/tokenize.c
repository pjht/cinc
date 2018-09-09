//
//  tokenize.c
//  cinc
//
//  Created by Peter Terpstra on 9/7/18.
//  Copyright © 2018 Peter Terpstra. All rights reserved.
//

#include "tokenize.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define ID_MAX_SIZE 31

Token* next_token(int* strpos, char* prg, Token* prev) {
    char current=prg[*strpos];
    if (isalpha(current) || current=='_') {
        char* id=malloc(sizeof(char)*ID_MAX_SIZE+1);
        int length=1;
        id[0]=current;
        (*strpos)++;
        while (true) {
            current=prg[*strpos];
            if (isalnum(current) || current=='_') {
                id[length]=current;
                length++;
                (*strpos)++;
            } else {
                break;
            }
            if (length==ID_MAX_SIZE) {
                break;
            }
        }
        id[length]=0;
        if (strcmp("return",id)==0) {
            return new_token(TYPE_RETURN, NULL, prev);
        } else if (strcmp("int",id)==0) {
           return new_token(TYPE_TYPE, val_from_const_str("int"), prev);
        }
        return new_token(TYPE_IDENT, val_from_str(id), prev);
    } else if (isdigit(current)) {
        char* id=malloc(sizeof(char)*ID_MAX_SIZE+1);
        int length=1;
        id[0]=current;
        (*strpos)++;
        while (true) {
            current=prg[*strpos];
            if (isdigit(current)) {
                id[length]=current;
                length++;
                (*strpos)++;
            } else {
                break;
            }
            if (length==ID_MAX_SIZE) {
                break;
            }
        }
        id[length]=0;
        return new_token(TYPE_NUM, val_from_int(atoi(id)), prev);
    } else if (isblank(current) || current=='\n') {
        (*strpos)++;
        current=prg[*strpos];
        while (isblank(current)) {
            (*strpos)++;
            current=prg[*strpos];
        }
        return next_token(strpos, prg, prev);
    } else if (current=='{' || current=='}' || current=='(' || current==')' || current==';' ) {
        (*strpos)++;
        return new_token(current, NULL, prev);
    }
    return new_token(TYPE_EOF, NULL, prev);
}

Token* tokenize(char* prg) {
    int strpos=0;
    Token* first=NULL;
    Token* tok=NULL;
    while (true) {
        tok=next_token(&strpos, prg, tok);
        if (!first) {
            first=tok;
        }
        if (tok->type==TYPE_EOF) {
            break;
        }
    }
    return first;
}

void free_toklist(Token* tokens) {
    while (tokens) {
        unsigned char type=tokens->type;
        if (tokens->val) {
            TokenVal val=*(tokens->val);
            if (!(val.constflag)) {
                if (val.type==strval) {
                    free(val.strval);
                }
            }
        }
        free(tokens->val);
        Token* next=tokens->next;
        free(tokens);
        tokens=next;
        if (type==TYPE_EOF) {
            break;
        }
    }
}
