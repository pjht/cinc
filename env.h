//
//  env.h
//  cinc
//
//  Created by Peter Terpstra on 9/11/18.
//  Copyright © 2018 Peter Terpstra. All rights reserved.
//

#ifndef env_h
#define env_h

struct _env {
    const char** varnames;
    const char** types;
    int* offsets;
    int num_els;
    int max_els;
    int offset;
    struct _env* next;
    struct _env* prev;
    char* contlabel;
    char* breaklabel;
};

typedef struct _env Env;

Env* new_env(Env* prev);
void add_entry(Env* env,const char* type,const char* name);
int get_offset(Env* env,const char* name);
void free_env(Env* env);

#endif /* env_h */
