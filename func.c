//
//  func.c
//  cinc
//
//  Created by Peter Terpstra on 9/13/18.
//  Copyright © 2018 Peter Terpstra. All rights reserved.
//

#include "func.h"
#include <stdbool.h>
#include <stdlib.h>

Arg* make_arg(const char* type,const char* name) {
    Arg* arg=malloc(sizeof(Arg));
    arg->type=type;
    arg->name=name;
    return arg;
}

void free_arg(Arg* arg) {
    free(arg);
}

Func* make_func(const char* name,bool defined,int nargs,Arg* args) {
    Func* func=malloc(sizeof(Func));
    func->name=name;
    func->defined=defined;
    func->nargs=nargs;
    func->args=args;
    return func;
}

void free_func(Func* func) {
    free(func->args);
    free(func);
}
