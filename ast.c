//
//  ast.c
//  cinc
//
//  Created by Peter Terpstra on 9/9/18.
//  Copyright © 2018 Peter Terpstra. All rights reserved.
//

#include "ast.h"
#include <stdlib.h>
#include <stdio.h>
#define AST_CHILDREN_START 10

AstNode* make_node(const char* data) {
    AstNode* node=malloc(sizeof(AstNode));
    node->data=data;
    node->num_children=0;
    node->max_children=AST_CHILDREN_START;
    node->children=malloc(sizeof(AstNode*)*AST_CHILDREN_START);
    return node;
}

void add_child(AstNode* parent,AstNode* child) {
    if (parent->num_children==parent->max_children) {
        parent->children=realloc(parent->children, sizeof(AstNode*)*(parent->max_children+10));
        (parent->max_children)+=10;
    }
    parent->children[parent->num_children]=child;
    parent->num_children++;
}

void print_tree(AstNode* root,int tabLevel) {
    if (tabLevel==0) {
        printf("Root: %s\n",root->data);
    }
    if ((root->num_children)>0) {
        for (int i=0;i<(root->num_children);i++) {
            for (int i=0;i<tabLevel;i++) {
                printf("  ");
            }
            printf("Child %d: ",i);
            if (root->children[i]) {
                printf("%s\n",root->children[i]->data);
                print_tree(root->children[i],tabLevel+1);
            } else {
                printf("NULL\n");
            }
        }
    }
}

void free_tree(AstNode* root) {
    if (root) {
        if (root->children) {
            for (int i=0;i<(root->num_children);i++) {
                free_tree(root->children[i]);
            }
        }
        free(root);
    }
}
