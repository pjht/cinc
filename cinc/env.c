//
//  env.c
//  cinc
//
//  Created by Peter Terpstra on 9/11/18.
//  Copyright © 2018 Peter Terpstra. All rights reserved.
//

#include "env.h"
#include <stdlib.h>
#include <string.h>
Env* new_env(Env* prev) {
    Env* env=malloc(sizeof(Env));
    env->next=NULL;
    env->prev=prev;
    env->max_els=100;
    env->num_els=0;
    env->offset=8;
    env->offsets=malloc(sizeof(int)*100);
    env->varnames=malloc(sizeof(const char*)*100);
    env->types=malloc(sizeof(const char*)*100);
    env->contlabel=NULL;
    env->breaklabel=NULL;
    if (prev) {
        env->offset=prev->offset;
        env->contlabel=prev->contlabel;
        env->breaklabel=prev->breaklabel;
        prev->next=env;
    }
    return env;
}

void add_entry(Env* env,const char* type,const char* name) {
    if (env->num_els==env->max_els) {
        env->offsets=realloc(env->offsets,sizeof(int)*(100+env->num_els));
        env->varnames=realloc(env->varnames,sizeof(const char*)*(100+env->num_els));
        env->types=realloc(env->types,sizeof(const char*)*(100+env->num_els));
    }
    env->varnames[env->num_els]=name;
    env->types[env->num_els]=name;
    env->offsets[env->num_els]=env->offset;
    env->num_els++;
    env->offset+=8;
}

int get_offset(Env* env,const char* name) {
    for(int i=0;i<(env->num_els);i++) {
        if(strcmp(env->varnames[i], name)==0) {
            return env->offsets[i];
        }
    }
    if (env->prev) {
        return get_offset(env->prev, name);
    }
    return -1;
}

const char* get_type(Env* env,const char* name) {
    for(int i=0;i<(env->num_els);i++) {
        if(strcmp(env->varnames[i], name)==0) {
            return env->types[i];
        }
    }
    if (env->prev) {
        return get_type(env->prev, name);
    }
    return NULL;
}

void free_env(Env* env) {
    if (env->next) {
        free_env(env->next);
    }
    free(env->offsets);
    free(env->varnames);
    free(env);
}
