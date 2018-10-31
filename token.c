//
//  token.c
//  cinc
//
//  Created by Peter Terpstra on 9/7/18.
//  Copyright © 2018 Peter Terpstra. All rights reserved.
//

#include "token.h"
#include <stdlib.h>
#include <stdio.h>

Token* new_token(unsigned char type,TokenVal* val, Token* prev) {
    Token* token=malloc(sizeof(Token));
    token->type=type;
    token->val=val;
    token->next=NULL;
    if (prev) {
        prev->next=token;
    }
    return token;
}

TokenVal* val_from_int(int val) {
    TokenVal* tval=malloc(sizeof(TokenVal));
    tval->type=intval;
    tval->intval=val;
    tval->constflag=true;
    return tval;
}

TokenVal* val_from_const_str(const char* val) {
    TokenVal* tval=malloc(sizeof(TokenVal));
    tval->type=strval;
    tval->strval=(char*)val;
    tval->constflag=true;
    return tval;
}

TokenVal* val_from_str(char* val) {
    TokenVal* tval=malloc(sizeof(TokenVal));
    tval->type=strval;
    tval->strval=val;
    tval->constflag=false;
    return tval;
}

void print_tok(Token* token) {
    Token tok=*token;
    if (tok.type>127) {
        printf("Token of type ");
        switch (tok.type) {
            case TYPE_EOF:
                printf("EOF");
                break;
            case TYPE_IDENT:
                printf("IDENT");
                break;
            case TYPE_NUM:
                printf("NUM");
                break;
            case TYPE_RETURN:
                printf("RETURN");
                break;
            case TYPE_TYPE:
                printf("TYPE");
                break;
            default:
                break;
        }
        if (tok.val) {
            printf(", and value ");
            TokenVal val=*(tok.val);
            switch (val.type) {
                case intval:
                    printf("%d\n",val.intval);
                    break;
                case strval:
                    printf("%s\n",val.strval);
                    break;
                default:
                    break;
            }
        } else {
            printf("\n");
        }
    } else {
        printf("Token %c\n",tok.type);
    }

}
