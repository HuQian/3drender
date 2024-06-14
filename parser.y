%{
#include <stdio.h>
extern void yyerror(const char* msg); // implement in .l, called by bison generated code
extern int yylex(void); // implement by flex generated code, called by bison generated code

static int f_index_type = -1;
static int f_index_type_is_ok (int type) {
    if (f_index_type != -1 && f_index_type != type) {
        yyerror("un consistency index type\n");
        return -1;
    } else {
        f_index_type = type;
        return 0;
    }
}

%}

%define parse.error verbose

%union {
    int num_i;
    double num_float;
    double num_float_e;
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


%%

file :
    | file line
    ;

line :
    line_comment
    |
    line_v
    |
    line_vt
    |
    line_f
    ;

line_comment :
    T_COMMENT
    ;

line_v :
    T_ELE_V SS line_v_vertex SS line_v_vertex SS line_v_vertex line_end
    ;

line_vt :
    T_ELE_V SS line_v_vertex SS line_v_vertex SS line_v_vertex line_end
    ;

line_v_vertex :
    T_NUM_INT {printf("T_NUM_INT: %ld\n", $1);}
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

int main() {

    return yyparse();
}
