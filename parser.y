/* following code will put in hpp */
%code requires {
#include <stdio.h>
#include <iostream>
#include <memory>
#include <string>
#include "ast.hpp"
}

%{
#include <iostream>
#include <memory>
#include <string>
#include "ast.hpp"

// implement by flex generated code, called by bison generated code
extern int yylex(void); 

static void
yyerror(std::unique_ptr<BaseAST> &global_ast, const char *msg) {
    global_ast->dump();
    std::cout << "Error: " << msg << std::endl;
    exit(1);
}

static int f_index_type = -1;
static int f_index_type_is_ok (int type) {
    if (f_index_type != -1 && f_index_type != type) {
        return -1;
    } else {
        f_index_type = type;
        return 0;
    }
}

%}

%define parse.error verbose
%parse-param { std::unique_ptr<BaseAST> &global_ast }

%union {
    int num_i;
    double num_float;
    double num_float_e;

    BaseAST *ast_val;
}

%token T_D
%token T_COMMENT
%token T_NEWLINE
%token SS

%token T_ELE_V
%token T_ELE_VT
%token T_ELE_VN
%token T_ELE_F

%token <num_i>          T_NUM_INT
%token <num_float>      T_NUM_FLOAT
%token <num_float_e>    T_NUM_FLOAT_E

%type <num_float> line_v_vertex
%type <ast_val> line_v

%%

file :
    | file line
    ;

line :
    line_comment
    |
    line_v {
        CompUnitAST* ptr = dynamic_cast<CompUnitAST*>(global_ast.get());
        if (ptr) {
            ptr->line_list.push_back(std::unique_ptr<BaseAST>($1));
        } else {
            std::cout << "Error: invalid ptr\n";
        }
    }
    |
    line_vt
    |
    line_f
    ;

line_comment :
    T_COMMENT
    ;

line_v :
    T_ELE_V SS line_v_vertex SS line_v_vertex SS line_v_vertex line_end {
        auto ast = new LineVAST();
        ast->type = "v";
        ast->v0 = $3;
        ast->v1 = $5;
        ast->v2 = $7;

        $$ = ast;
    }
    ;

line_vt :
    T_ELE_VT SS line_v_vertex SS line_v_vertex SS line_v_vertex line_end
    ;

line_v_vertex :
    T_NUM_INT {printf("T_NUM_INT: %d\n", $1);}
    |
    T_NUM_FLOAT {printf("T_NUM_FLOAT: %.14lf\n", $1);}
    |
    T_NUM_FLOAT_E {printf("T_NUM_FLOAT_E: %.14lf\n", $1);}
    ;

/*
   f v
   f v//vn
   f v/vt/vn 
*/
line_f :
    T_ELE_F f_index_list line_end
    ;

f_index_list:
    |
    f_index_list SS T_NUM_INT { if (-1 == f_index_type_is_ok(1)) YYERROR; printf ("-%d-", $3); }
    |
    f_index_list SS T_NUM_INT T_D T_D T_NUM_INT { if (-1 == f_index_type_is_ok(2)) YYERROR; }
    |
    f_index_list SS T_NUM_INT T_D T_NUM_INT T_D T_NUM_INT { if (-1 == f_index_type_is_ok(3)) YYERROR; }
    ;


line_end :
    |
    SS
    ;
%%

