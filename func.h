//
//  func.h
//  cinc
//
//  Created by Peter Terpstra on 9/13/18.
//  Copyright © 2018 Peter Terpstra. All rights reserved.
//

#ifndef func_h
#define func_h

#include <stdbool.h>

typedef struct Arg {
    const char* type;
    const char* name;
} Arg;

typedef struct Func {
    const char* name;
    bool defined;
    int nargs;
    Arg* args;
} Func;

#endif /* func_h */
