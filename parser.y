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
yyerror(ModelData* model_data, const char *msg) {
    (void) model_data;
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
%parse-param { ModelData* model_data }

%union {
    int num_i;
    double num_float;
    double num_float_e;
}

%token T_NEWLINE
%token T_S
%token T_D

%token T_Type_V
%token T_Type_VT
%token T_Type_VN

%token T_Type_P
%token T_Type_L
%token T_Type_F

%token <num_i>          T_NUM_INT
%token <num_float>      T_NUM_FLOAT
%token <num_float_e>    T_NUM_FLOAT_E

%token T_COMMENT

%type <num_float> T_Type_Number


%%

T_File :
    | T_File T_Line
    ;

T_Line :
    T_COMMENT
    |
    T_VertexDataV
    |
    T_VertexDataVT
    |
    T_VertexDataVN
    |
    T_ElementPoint
    |
    T_ElementLine
    |
    T_ElementFace
    ;

T_VertexDataV :
    T_Type_V T_S T_Type_Number T_S T_Type_Number T_S T_Type_Number T_LineEnd {
        model_data->v_list.push_back(VertexDataV {$3, $5, $7});
    }
    ;

T_VertexDataVT :
    T_Type_VT T_S T_Type_Number T_S T_Type_Number T_LineEnd {
        model_data->vt_list.push_back(VertexDataVT {$3, $5});
    }
    ;

T_VertexDataVN :
    T_Type_VN T_S T_Type_Number T_S T_Type_Number T_S T_Type_Number T_LineEnd {
        model_data->vn_list.push_back(VertexDataVN {$3, $5, $7});
    }
    ;

T_Type_Number :
    T_NUM_INT {
        printf("T_NUM_INT: %d\n", $1);
        $$ = $1;
    }
    |
    T_NUM_FLOAT {
        printf("T_NUM_FLOAT: %.14lf\n", $1);
        $$ = $1;
    }
    |
    T_NUM_FLOAT_E {
        printf("T_NUM_FLOAT_E: %.14lf\n", $1);
        $$ = $1;
    }
    ;

/*
  p v
*/
T_ElementPoint :
    T_Type_P T_S T_NUM_INT T_LineEnd {
        ElementPoint tmp;
        tmp.index = $3;

        model_data->point_list.push_back(tmp);
    }
    ;

/*
  l v0...vn
*/
T_ElementLine :
    T_Type_L T_LineVertexIndexList T_LineEnd
    ;

T_LineVertexIndexList :
    {
        model_data->line_list.push_back(ElementLine());
    }
    |
    T_LineVertexIndexList T_S T_NUM_INT {
        auto& line = model_data->line_list.back();

        line.index_list.push_back($3);
    }
    ;

/*
   f v
   f v/vt
   f v//vn
   f v/vt/vn
*/
T_ElementFace :
    T_Type_F T_FaceVertexIndexList T_LineEnd
    ;

T_FaceVertexIndexList :
    {
        model_data->face_list.push_back(ElementFace());
    }
    |
    T_FaceVertexIndexList T_S T_NUM_INT {
        printf ("f type 1\n");
        if (-1 == f_index_type_is_ok(1))
            YYERROR;
        auto& face = model_data->face_list.back();
        face.index_list.push_back($3);
    }
    |
    T_FaceVertexIndexList T_S T_NUM_INT T_D T_NUM_INT {
        printf ("f type 2\n");
        if (-1 == f_index_type_is_ok(2))
            YYERROR;

        auto& face = model_data->face_list.back();
        face.index_list.push_back($3);
        face.index_texture_list.push_back($5);
    }
    |
    T_FaceVertexIndexList T_S T_NUM_INT T_D T_D T_NUM_INT {
        printf ("f type 3\n");
        if (-1 == f_index_type_is_ok(3))
            YYERROR;
        auto& face = model_data->face_list.back();
        face.index_list.push_back($3);
        face.index_normal_list.push_back($6);
    }
    |
    T_FaceVertexIndexList T_S T_NUM_INT T_D T_NUM_INT T_D T_NUM_INT {
        printf ("f type 4\n");
        if (-1 == f_index_type_is_ok(4))
            YYERROR;
        auto& face = model_data->face_list.back();
        face.index_list.push_back($3);
        face.index_texture_list.push_back($5);
        face.index_normal_list.push_back($7);
    }
    ;

T_LineEnd :
    |
    T_S
    ;
%%

