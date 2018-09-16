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
        } else if (strcmp("if",id)==0) {
            return new_token(TYPE_IF, NULL, prev);
        } else if (strcmp("else",id)==0) {
            return new_token(TYPE_ELSE, NULL, prev);
        } else if (strcmp("for",id)==0) {
            return new_token(TYPE_FOR, NULL, prev);
        } else if (strcmp("while",id)==0) {
            return new_token(TYPE_WHILE, NULL, prev);
        } else if (strcmp("do",id)==0) {
            return new_token(TYPE_DO, NULL, prev);
        } else if (strcmp("break",id)==0) {
            return new_token(TYPE_BREAK, NULL, prev);
        } else if (strcmp("ccontinue",id)==0) {
            return new_token(TYPE_CONTINUE, NULL, prev);
        }
        return new_token(TYPE_IDENT, val_from_str(id), prev);
    } else if (isdigit(current)) {
        char* num=malloc(sizeof(char)*NUM_MAX_SIZE+1);
        int length=1;
        num[0]=current;
        (*strpos)++;
        while (true) {
            current=prg[*strpos];
            if (isdigit(current)) {
                num[length]=current;
                length++;
                (*strpos)++;
            } else {
                break;
            }
            if (length==NUM_MAX_SIZE) {
                break;
            }
        }
        num[length]=0;
        return new_token(TYPE_NUM, val_from_str(num), prev);
    } else if (isblank(current) || current=='\n') {
        (*strpos)++;
        current=prg[*strpos];
        while (isblank(current)) {
            (*strpos)++;
            current=prg[*strpos];
        }
        return next_token(strpos, prg, prev);
    } else if (current=='&') {
        (*strpos)++;
        current=prg[*strpos];
        if (current=='&') {
            (*strpos)++;
            return new_token(TYPE_LAND, NULL, prev);
        }
        return new_token('&', NULL, prev);
    } else if (current=='|') {
        (*strpos)++;
        current=prg[*strpos];
        if (current=='|') {
            (*strpos)++;
            return new_token(TYPE_LOR, NULL, prev);
        }
        return new_token('|', NULL, prev);
    } else if (current=='=') {
        (*strpos)++;
        current=prg[*strpos];
        if (current=='=') {
            (*strpos)++;
            return new_token(TYPE_EQ, NULL, prev);
        }
        return new_token('=', NULL, prev);
    } else if (current=='!') {
        (*strpos)++;
        current=prg[*strpos];
        if (current=='=') {
            (*strpos)++;
            return new_token(TYPE_NE, NULL, prev);
        }
        return new_token('!', NULL, prev);
    } else if (current=='<') {
        (*strpos)++;
        current=prg[*strpos];
        if (current=='=') {
            (*strpos)++;
            return new_token(TYPE_LE, NULL, prev);
        } else if (current=='<') {
            (*strpos)++;
            return new_token(TYPE_SL, NULL, prev);
        }
        return new_token('<', NULL, prev);
    } else if (current=='>') {
        (*strpos)++;
        current=prg[*strpos];
        if (current=='=') {
            (*strpos)++;
            return new_token(TYPE_GE, NULL, prev);
        } else if (current=='>') {
            (*strpos)++;
            return new_token(TYPE_SR, NULL, prev);
        }
        return new_token('>', NULL, prev);
    } else if (current==0) {
        return new_token(TYPE_EOF, NULL, prev);
    } else {
        if (current=='+' || current=='-' || current=='/' || current=='*' || current=='%' ||  current=='&' || current=='|' || current=='^') {
           (*strpos)++;
            char* str=malloc(sizeof(char));
            *str=current;
            current=prg[*strpos];
            if (current=='=') {
                (*strpos)++;
                return new_token(TYPE_COMPSET, val_from_str(str), prev);
            } else {
                if (*str=='+' && current=='+') {
                    (*strpos)++;
                    return new_token(TYPE_INC, NULL, prev);
                }
                if (*str=='-' && current=='-') {
                    (*strpos)++;
                    return new_token(TYPE_DEC, NULL, prev);
                }
                return new_token(*str, NULL, prev);
            }
        }
        (*strpos)++;
        return new_token(current, NULL, prev);
    }
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
