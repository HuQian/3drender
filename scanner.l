%option noyywrap
%option nounput
%option noinput
%option debug
%option yylineno

%{
#include <string>
#include <iostream>

#include "parser.tab.hpp"

double get_float ();
double get_float_e ();

%}


%%


^v {
    return T_Type_V;
}

^vn {
    return T_Type_VN;
}

^vt {
    return T_Type_VT;
}

^p {
    return T_Type_P;
}

^l {
    return T_Type_L;
}

^f {
    return T_Type_F;
}

[0-9]+ {
    yylval.num_i = atoi(yytext); /* we save value in yylval when flex, and get it by bison */
    return T_NUM_INT;
}

[+-]*[0-9]+\.[0-9]+ {
    yylval.num_float = get_float();

    return T_NUM_FLOAT;
}

[+-]*[0-9]+\.[0-9]+e[+-]*[0-9]+ {
    yylval.num_float_e = get_float_e();

    return T_NUM_FLOAT_E;
}

[ \t]+ {
    return T_S;
}

\/ {
    return T_D;
}

[\n\r]+ {
    ; // return T_NEWLINE;
}

#.*[\n\r]* {
    return T_COMMENT;
}

.               { printf("[other]\n"); return 0; /* end when meet everything else */ }

%%


double get_float() {
    double value = 0;
    int flag = 1;
    int i = 0;

    if (yytext[i] == '-' || yytext[i] == '+') {
        flag = yytext[i] == '-' ? -1 : 1;
        i++;
    }

    for (; yytext[i] != '.'; i++) {
        value *= 10;
        value += yytext[i] - '0';
    }

    /* skip . */
    i++;

    for (double p = 10; i < yyleng; i++, p *= 10) {
        value += (yytext[i] - '0') / p;
    }

    value *= flag;

    return value;
}

double get_float_e() {
    double value = 0;
    int flag = 1;
    int i = 0;

    if (yytext[i] == '-' || yytext[i] == '+') {
        flag = yytext[i] == '-' ? -1 : 1;
        i++;
    }

    for (; yytext[i] != '.'; i++) {
        value *= 10;
        value += yytext[i] - '0';
    }

    /* skip . */
    i++;

    for (double p = 10; yytext[i] != 'e'; i++, p *= 10) {
        value += (yytext[i] - '0') / p;
    }

    /* skip e */
    i++;

    int ee_flag = 1;
    if (yytext[i] == '-' || yytext[i] == '+') {
        ee_flag = yytext[i] == '-' ? -1 : 1;
        i++;
    }

    int ee = 0;
    for (; i < yyleng; i++) {
        ee *= 10;
        ee += yytext[i] - '0';
    }

    for (; ee > 0; ee--) {
        if (ee_flag == -1) {
            value /= 10;
        } else {
            value *= 10;
        }
    }

    value *= flag;

    return value;
}

