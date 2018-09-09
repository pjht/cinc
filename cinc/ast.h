//
//  ast.h
//  cinc
//
//  Created by Peter Terpstra on 9/9/18.
//  Copyright © 2018 Peter Terpstra. All rights reserved.
//

#ifndef ast_h
#define ast_h

struct _astnode {
    const char* data;
    int num_children;
    int max_children;
    struct _astnode** children;
};

typedef struct _astnode AstNode;

AstNode* make_node(const char* data);
void add_child(AstNode* parent,AstNode* child);
void free_tree(AstNode* root);

#endif /* ast_h */
