%{
#include <stdio.h>

#include "sweet_vinumc.h"

int yylex();
%}

%token CONTENT

%%

program: {};

%%
