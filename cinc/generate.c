//
//  generate.c
//  cinc
//
//  Created by Peter Terpstra on 9/9/18.
//  Copyright © 2018 Peter Terpstra. All rights reserved.
//

#include "generate.h"
#include "ast.h"
#include "env.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char* prg_asm;
static int prgsize=0;
static int nextlabel=0;
Env* env=NULL;

void prg_add(const char* str) {
    unsigned long len=strlen(str);
    prg_asm=realloc(prg_asm, prgsize+len+1);
    prgsize+=len;
    strncat(prg_asm,str,len);
    
}

static char* next_label() {
    char* buf=malloc(sizeof(char)*4097);
    buf[0]='l';
    int written=snprintf(buf+1, 4096, "%d",nextlabel);
    if(written<0 || written>=4096) {
        printf("Error: was not able to convert next label number to a string.\n");
        exit(1);
    }
    nextlabel++;
    return buf;
}

static void generate_expr(AstNode* ast) {
    const char* type=ast->data;
    if (strcmp("num", type)==0) {
        prg_add("mov rax,");
        prg_add(ast->children[0]->data);
        prg_add("\n");
        return;
    } else if (strcmp("var", type)==0) {
        prg_add("mov rax,[rbp-");
        char buf[4096];
        int offset=get_offset(env, ast->children[0]->data);
        if (offset==-1) {
            printf("Error: no such variable %s\n",ast->children[0]->data);
            exit(1);
        }
        int written=snprintf(buf, 4096, "%d",offset);
        if(written<0 || written>=4096) {
            printf("Error: was not able to convert offset for variable %s to a string.\n",ast->children[0]->data);
            exit(1);
        }
        prg_add(buf);
        prg_add("]\n");
        return;
    } else if (strcmp("neg", type)==0) {
        generate_expr(ast->children[0]);
        prg_add("neg rax\n");
        return;
    } else if (strcmp("not", type)==0) {
        generate_expr(ast->children[0]);
        prg_add("cmp rax,0\nmove rax,0\nsete al\n");
        return;
    } else if (strcmp("comp", type)==0) {
        generate_expr(ast->children[0]);
        prg_add("not rax\n");
        return;
    } else if (strcmp("preinc", type)==0 || strcmp("postinc", type)==0 ) {
        prg_add("mov rax,[rbp-");
        char buf[4096];
        int offset=get_offset(env, ast->children[0]->children[0]->data);
        if (offset==-1) {
            printf("Error: no such variable %s\n",ast->children[0]->children[0]->data);
            exit(1);
        }
        int written=snprintf(buf, 4096, "%d",offset);
        if (written<0 || written>=4096) {
            printf("Error: was not able to convert offset for variable %s to a string.\n",ast->children[0]->data);
            exit(1);
        }
        prg_add(buf);
        prg_add("]\ninc rax\nmov [rbp-");
        prg_add(buf);
        prg_add("],rax\n");
        if (strcmp("postinc", type)==0) {
            prg_add("dec rax\n");
        }
        return;
    } else if (strcmp("predec", type)==0 || strcmp("postdec", type)==0) {
        prg_add("mov rax,[rbp-");
        char buf[4096];
        int offset=get_offset(env, ast->children[0]->children[0]->data);
        if (offset==-1) {
            printf("Error: no such variable %s\n",ast->children[0]->children[0]->data);
            exit(1);
        }
        int written=snprintf(buf, 4096, "%d",offset);
        if (written<0 || written>=4096) {
            printf("Error: was not able to convert offset for variable %s to a string.\n",ast->children[0]->data);
            exit(1);
        }
        prg_add("]\ndec rax\nmov [rbp-");
        prg_add(buf);
        prg_add("],rax\n");
        if (strcmp("postdec", type)==0) {
            prg_add("inc rax\n");
        }
        return;
    } else if (strcmp("cond", type)==0) {
        char* e3=next_label();
        char* post=next_label();
        generate_expr(ast->children[0]);
        prg_add("cmp rax,0\n");
        prg_add("je ");
        prg_add(e3);
        prg_add("\n");
        generate_expr(ast->children[1]);
        prg_add("jmp ");
        prg_add(post);
        prg_add("\n");
        prg_add(e3);
        prg_add(":\n");
        generate_expr(ast->children[2]);
        prg_add(post);
        prg_add(":\n");
        free(e3);
        free(post);
        return;
    } else if (strcmp("assign", type)==0) {
        generate_expr(ast->children[1]);
        prg_add("mov [rbp-");
        char buf[4096];
        int offset=get_offset(env, ast->children[0]->data);
        if (offset==-1) {
            printf("Error: no such variable %s\n",ast->children[0]->data);
            exit(1);
        }
        int written=snprintf(buf, 4096, "%d",offset);
        if(written<0 || written>=4096) {
            printf("Error: was not able to convert offset for variable %s to a string.\n",ast->children[0]->data);
            exit(1);
        }
        prg_add(buf);
        prg_add("],rax\n");
        return;
    }
    generate_expr(ast->children[0]);
    prg_add("push rax\n");
    generate_expr(ast->children[1]);
    prg_add("pop rcx\n");
    if (strcmp(type,"add")==0) {
        prg_add("add rax,rcx\n");
    } else if (strcmp(type,"sub")==0) {
        prg_add("sub rcx,rax\n");
        prg_add("mov rax,rcx\n");
    } else if (strcmp(type,"mul")==0) {
        prg_add("imul rax,rcx\n");
    } else if (strcmp(type,"div")==0 || strcmp(type,"mod")==0) {
        prg_add("mov rdx,0\npush rcx\npush rax\npop rcx\npop rax\nidiv rcx\n");
        if (strcmp(type,"mod")==0) {
            prg_add("mov rax,rdx\n");
        }
    } else if (strcmp(type, "and")==0) {
        prg_add("and rcx,rax\n");
    } else if (strcmp(type, "or")==0) {
        prg_add("or rax,rcx\n");
    } else if (strcmp(type, "xor")==0) {
        prg_add("xor rax,rcx\n");
    } else if (strcmp(type, "land")==0) {
        prg_add("cmp rcx,0\nsetne cl\ncmp rax,0\nmov rax,0\nsetne al\nand al,cl\n");
    } else if (strcmp(type, "lor")==0) {
        prg_add("or rax,rcx\nmov rax,0\nsetne al\n");
    } else if (strcmp(type, "sal")==0) {
        prg_add("sal rax,rcx\n");
    } else if (strcmp(type, "sar")==0) {
        prg_add("sar rax,rcx\n");
    } else if (strcmp(type, "eq")==0) {
        prg_add("cmp rcx,rax\nmov rax,0\nsete al\n");
    } else if (strcmp(type, "ne")==0) {
        prg_add("cmp rcx,rax\nmov rax,0\nsetne al\n");
    } else if (strcmp(type, "lt")==0) {
        prg_add("cmp rcx,rax\nmov rax,0\nsetl al\n");
    } else if (strcmp(type, "le")==0) {
        prg_add("cmp rcx,rax\nmov rax,0\nsetle al\n");
    } else if (strcmp(type, "gt")==0) {
        prg_add("cmp rcx,rax\nmov rax,0\nsetg al\n");
    } else if (strcmp(type, "ge")==0) {
        prg_add("cmp rcx,rax\nmov rax,0\nsetge al\n");
    } else {
        printf("Unknown expr type %s\n",type);
        exit(1);
    }
}

static void generate_block(AstNode* ast);

static void generate_statement(AstNode* ast) {
    const char* type=ast->data;
    if (strcmp("return", type)==0) {
        generate_expr(ast->children[0]);
        prg_add("mov rsp,rbp\npop rbp\nret\n");
    } else if (strcmp("vardec", type)==0) {
        if (get_offset(env, ast->children[1]->data)!=-1) {
            printf("Error: Redeclaration of variable %s\n",ast->children[1]->data);
            exit(1);
        }
        add_entry(env,ast->children[0]->data,ast->children[1]->data);
        prg_add("push 0\n");
    } else if (strcmp("vardecinitial", type)==0) {
        if (get_offset(env, ast->children[1]->data)!=-1) {
            printf("Error: Redeclaration of variable %s\n",ast->children[1]->data);
            exit(1);
        }
        add_entry(env,ast->children[0]->data,ast->children[1]->data);
        generate_expr(ast->children[2]);
        prg_add("push rax\n");
    } else if (strcmp("if", type)==0) {
        char* post=next_label();
        generate_expr(ast->children[0]);
        prg_add("cmp rax,0\n");
        prg_add("je ");
        prg_add(post);
        prg_add("\n");
        generate_statement(ast->children[1]);
        prg_add(post);
        prg_add(":\n");
        free(post);
        return;
    } else if (strcmp("ifelse", type)==0) {
        char* els=next_label();
        char* post=next_label();
        generate_expr(ast->children[0]);
        prg_add("cmp rax,0\n");
        prg_add("je ");
        prg_add(els);
        prg_add("\n");
        generate_statement(ast->children[1]);
        prg_add("jmp ");
        prg_add(post);
        prg_add("\n");
        prg_add(els);
        prg_add(":\n");
        generate_statement(ast->children[2]);
        prg_add(post);
        prg_add(":\n");
        free(els);
        free(post);
        return;
    } else if (strcmp("while", type)==0) {
        char* begin=next_label();
        char* end=next_label();
        env->breaklabel=end;
        env->contlabel=begin;
        prg_add(begin);
        prg_add(":\n");
        generate_expr(ast->children[0]);
        prg_add("je ");
        prg_add(end);
        prg_add("\n");
        generate_statement(ast->children[1]);
        prg_add("jmp ");
        prg_add(begin);
        prg_add("\n");
        prg_add(end);
        prg_add(":\n");
        env->contlabel=NULL;
        env->breaklabel=NULL;
        free(begin);
        free(end);
    } else if (strcmp("dowhile", type)==0) {
        char* begin=next_label();
        char* end=next_label();
        env->breaklabel=end;
        env->contlabel=begin;
        prg_add(begin);
        prg_add(":\n");
        generate_statement(ast->children[1]);
        generate_expr(ast->children[0]);
        prg_add("jne ");
        prg_add(begin);
        prg_add("\n");
        prg_add(end);
        prg_add(":\n");
        env->contlabel=NULL;
        env->breaklabel=NULL;
        free(end);
        free(begin);
    } else if (strcmp("for", type)==0) {
        char* cond=next_label();
        char* end=next_label();
        char* cont=next_label();
        env->breaklabel=end;
        env->contlabel=cont;
        env=new_env(env);
        if (ast->children[1]) {
            if (strcmp(ast->children[0]->data, "decl")==0) {
                generate_statement(ast->children[1]);
            } else {
                generate_expr(ast->children[1]);
            }
        }
        prg_add(cond);
        prg_add(":\n");
        if (ast->children[2]) {
            generate_expr(ast->children[2]);
        } else {
            prg_add("mov rax,1\n");
        }
        prg_add("je ");
        prg_add(end);
        prg_add("\n");
        generate_statement(ast->children[4]);
        prg_add(cont);
        prg_add(":\n");
        if (ast->children[3]) {
            generate_expr(ast->children[3]);
        }
        prg_add("jmp ");
        prg_add(cond);
        prg_add("\n");
        prg_add(end);
        prg_add(":\n");
        int bytes=(env->num_els)*8;
        prg_add("add rsp,");
        char buf[4096];
        int written=snprintf(buf, 4096, "%d",bytes);
        if(written<0 || written>=4096) {
            printf("Error: was not able to convert the bytes to deallocate into a string\n");
            exit(1);
        }
        prg_add(buf);
        prg_add("\n");
        env->contlabel=NULL;
        env->breaklabel=NULL;
        Env* prev=env->prev;
        if (prev) {
            prev->next=NULL;
        }
        free_env(env);
        env=prev;
        free(cond);
        free(end);
        free(cont);
    } else if (strcmp("break", type)==0) {
        if (env->breaklabel) {
            prg_add("jmp ");
            prg_add(env->breaklabel);
            prg_add("\n");
        } else {
            printf("Error: break outside of loop\n");
        }
    } else if (strcmp("continue", type)==0) {
        if (env->breaklabel) {
            prg_add("jmp ");
            prg_add(env->contlabel);
            prg_add("\n");
        } else {
            printf("Error: continue outside of loop\n");
        }
    } else if (strcmp("block", type)==0) {
        generate_block(ast);
    } else {
        generate_expr(ast);
    }
}

static void generate_block(AstNode* ast) {
    env=new_env(env);
    for (int i=0;i<(ast->num_children);i++) {
        generate_statement(ast->children[i]);
    }
    int bytes=(env->num_els)*8;
    prg_add("add rsp,");
    char buf[4096];
    int written=snprintf(buf, 4096, "%d",bytes);
    if(written<0 || written>=4096) {
        printf("Error: was not able to convert the bytes to deallocate into a string\n");
        exit(1);
    }
    prg_add(buf);
    prg_add("\n");
    Env* prev=env->prev;
    if (prev) {
        prev->next=NULL;
    }
    free_env(env);
    env=prev;
}


static void generate_func(AstNode* ast) {
    const char* name=ast->children[1]->data;
    prg_add(".globl _");
    prg_add(name);
    prg_add("\n_");
    prg_add(name);
    prg_add(":\n");
    prg_add("push rbp\nmov rbp,rsp\n");
    generate_block(ast->children[2]);
    //Generate the implicit return 0:
    AstNode* ret=make_node("return");
    AstNode* num=make_node("num");
    add_child(num, make_node("0"));
    add_child(ret,num);
    generate_statement(ret);
}


char* generate_prg(AstNode* ast) {
    prg_asm=malloc(sizeof(char));
    prg_asm[0]=0;
    generate_func(ast);
    return prg_asm;
}
