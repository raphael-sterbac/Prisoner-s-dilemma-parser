%option noyywrap

DIG	[0-9]

%%

"if"	{ return IF; }
"else"	{ return ELSE; }
"Strategy" { return STRAT; }
"Constants" { return CONST; }
"def" { return DEF; }
"Cheat" { return CHEAT; }
"Undef" { return UNDEF; }
"Honest" { return HONEST; }
"return" { return RET; }
"last" { return LAST; }
"Random("[0-9]*")" { yylval.i = strdup(yytext); return RAND; }

"="	{ return ASSIGN; }
"||" { return OR; }
"&&" { return AND; }
"!"	{ return NOT; }
"==" { return EQ; }
"<" { return INF; }
">" { return SUP; }
"<=" { return INF-EQ; }
">=" { return SUP-EQ; }
"+" { return ADD; }
"-" { return MINUS; }

[0-9]* { yylval.i = strdup(yytext); return NUM; }

[a-z_][a-zA-Z0-9_]* { yylval.i = strdup(yytext); return IDENT; }
[A-Z_][a-zA-Z0-9_]* { yylval.i = strdup(yytext); return IDSTRAT; }


[	 \n]   { /* ignore */ }

.	{ return *yytext; }

%%

