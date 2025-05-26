%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex();

void yyerror(char *s)
{
	fflush(stdout);
	fprintf(stderr, "%s\n", s);
}

/***************************************************************************/
/* Data structures for storing a program.                                */


typedef struct var	// a variable
{
	char *name;
	char *value;
	struct var *next;
} var;


typedef struct expr	// an expression
{
	int type;	// type of expression 
	var *var;   // variable linked to the expression (if there is one)
	struct expr *left, *right; // Left and right subexpression (if there is one)
    char *val; // Value of the expression (if there is one)
} expr;

typedef struct stmt	// command
{
	int type;	// type of statement  
	var *var;   // variable linked to the statement
	expr *expr; // expression linked to the statement
	struct stmt *left, *right; // Left and right sub-statements
    char *val; // Value of the expression
} stmt;

typedef struct strats
{
    char *name; // Name of strategy
    stmt *s; // Statement of strategy
    struct strats *next; // Next strategy
    var *defs; // Definitions of strategy
} strats;

typedef struct agt
{
	int id; //id of the agent
	strats *st; //strategie
	int lf; //life points remaining
	int spw; //spawn points remaining
	int lst;
} agt;

/****************************************************************************/
/* All data pertaining to the programme are accessible from these two vars. */

var *program_vars; // List of program constants (defined at the end)
strats *program_strats; // List of program strats
var *current_defs; // List of current definitions (at parsing)

int duration;
int rewardhh;
int rewardcc;
int rewardhc;
int rewardch;
int initial;
int meetings;
int spawn;
int intervals;
int life;
int mutation;

/****************************************************************************/
/* Functions for setting up data structures at parse time.                 */

// Makes an identifier for a constant of name s and value val
var* make_ident (char *s, char *val)
{
	var *v = malloc(sizeof(var));
	v->name = s;
	v->value = val;	
	v->next = NULL;
    program_vars = v;


    if(!strcmp(s,"duration")) duration = atoi(val);
    if(!strcmp(s,"rewardHH")) rewardhh = atoi(val);
    if(!strcmp(s,"rewardCC")) rewardcc = atoi(val);
    if(!strcmp(s,"rewardHC")) rewardhc = atoi(val);
    if(!strcmp(s,"rewardCH")) rewardch = atoi(val);
    if(!strcmp(s,"initial")) initial = atoi(val);
    if(!strcmp(s,"meetings")) meetings = atoi(val);
    if(!strcmp(s,"spawn")) spawn = atoi(val);
    if(!strcmp(s,"intervals")) intervals = atoi(val);
    if(!strcmp(s,"life")) life = atoi(val);
    if (!strcmp(s,"mutation")) mutation = atoi(val);

	return v;
}

// Similar function for definitions
var* make_def (char *s, expr *e)
{
	var *v = malloc(sizeof(var));
	v->name = s;
	v->value = e->val;	
	v->next = NULL;
    current_defs = v;
	return v;
}

// Finds the definition or constant corresponding to the identifier s
var* find_ident (char *s)
{
	var *v = current_defs;
	while (v && strcmp(v->name,s)) v = v->next;
	if (!v) { yyerror("undeclared variable"); exit(1); }
	return v;
}

// Makes an expression with the given parameters
expr* make_expr (int type, var *var, expr *left, expr *right, char *val)
{
	expr *e = malloc(sizeof(expr));
	e->type = type;
	e->var = var;
    e->val = val;
	e->left = left;
	e->right = right;
	return e;
}

// Makes a statement with the given parameters
stmt* make_stmt (int type, var *var, expr *expr,
			stmt *left, stmt *right)
{
	stmt *s = malloc(sizeof(stmt));
	s->type = type;
	s->var = var;
	s->expr = expr;
	s->left = left;
	s->right = right;
	return s;
}

// Makes a strategy with the given parameters 
strats* make_strat(stmt *s, char *name, strats *next, var *defs) {
    strats *st = malloc(sizeof(strats));
    st->s = s;
    st->name = name;
    st->next = next;
    st->defs = defs;
    return st;
}

strats* find_strat (char *s)
{
	strats *v = program_strats;
	while (v && strcmp(v->name,s)) v = v->next;
	if (!v) { yyerror("unexistent strategie"); exit(1); }
	return v;
}

agt* make_agt (int id, strats *strat, int life, int spawn, int last)
{
	agt *a = malloc(sizeof(agt));
	a->id = id;
	a->st = strat;
    a->lf = life;
	a->spw = spawn;
	a->lst = last;
	return a;
}

%}

/****************************************************************************/

/* types used by terminals and non-terminals */

%union {
	char *i;
	var *v;
	expr *e;
	stmt *s;
    strats *st;
}

%type <v> declist
%type <v> defs
%type <e> expr
%type <e> ret 
%type <e> term 
%type <s> stmt 
%type <st> strats

// Defines tokens (lexèmes) for the grammar 
%token ASSIGN OR AND NOT EQ IF ELSE STRAT CONST DEF CHEAT HONEST  RET INF SUP UNDEF LAST 
%token <i> IDENT
%token <i> IDSTRAT
%token <i> RAND 
%token <i> NUM 

// Defines priorities for our operators 
// TODO : Vérifier les priorités
%left ';'
%right DEF
%left OR 
%left AND
%right NOT 
%right EQ
%right INF
%right SUP
%right IF
%right ELSE

// Defines the grammar to parse our language
%%
prog: strats { program_strats = $1; }
    | strats CONST declist { program_strats = $1; }

/* TODO: Fix les constantes (définies après le parsing des stratégies) */
declist	:  { $$ = NULL; program_vars = NULL; }
        | declist IDENT ASSIGN NUM	{ ($$ = make_ident($2,$4))->next = $1; }

strats: STRAT IDSTRAT defs stmt { $$ = make_strat($4,$2,NULL,$3); }
    | strats STRAT IDSTRAT defs stmt { $$ = make_strat($5,$3,$1,$4); }

defs: { $$ = NULL; current_defs = NULL; }
    | defs DEF IDENT ASSIGN term { ($$ = make_def($3,$5))->next = $1; }

stmt : stmt ';' stmt { $$ = make_stmt(';',NULL,NULL,$1,$3); }
    | IF expr stmt
        { $$ = make_stmt(IF,NULL,$2,$3,NULL); }
    | IF expr stmt ELSE stmt
        { $$ = make_stmt(ELSE,NULL,$2,$3,$5); }
    | RET ret 
        { $$ = make_stmt(RET,NULL,$2,NULL,NULL); } 
    | IDENT ASSIGN term 
        { $$ = make_stmt(ASSIGN,find_ident($1),$3,NULL,NULL); }


expr	: IDENT		{ $$ = make_expr(0,find_ident($1),NULL,NULL, NULL); }
	| expr OR expr	{ $$ = make_expr(OR,NULL,$1,$3, NULL); }
	| expr AND expr	{ $$ = make_expr(AND,NULL,$1,$3, NULL); }
	| expr EQ expr	{ $$ = make_expr(EQ,NULL,$1,$3, NULL); }
	| expr INF expr	{ $$ = make_expr(INF,NULL,$1,$3, NULL); }
	| expr SUP expr	{ $$ = make_expr(SUP,NULL,$1,$3, NULL); }
	| NOT expr	    { $$ = make_expr(NOT,NULL,$2,NULL, NULL); }
    | RAND          { $$ = make_expr(RAND, NULL, NULL, NULL, $1); }
    | term          { $$ = $1; }
	| '(' expr ')'	{ $$ = $2; }

term: CHEAT     { $$ = make_expr(CHEAT, NULL, NULL, NULL, "Cheat"); }
    | HONEST    { $$ = make_expr(HONEST, NULL, NULL, NULL, "Honest"); }
    | UNDEF     { $$ = make_expr(UNDEF, NULL, NULL, NULL, "Undef"); }
    | LAST      { $$ = make_expr(LAST, NULL, NULL, NULL, "last"); }
    | NUM       { $$ = make_expr(NUM, NULL, NULL, NULL, $1); }

ret :  IDENT    { $$ = make_expr(0,find_ident($1),NULL,NULL, NULL); }
    | CHEAT     { $$ = make_expr(CHEAT, NULL, NULL, NULL, NULL); }
    | HONEST    { $$ = make_expr(HONEST, NULL, NULL, NULL, NULL); }
    | LAST      { $$ = make_expr(LAST, NULL, NULL, NULL, "last"); }

%%

#include "langlex.c"

/****************************************************************************/
/* programme interpreter      :                                             */

int last = 'u';

/* Evaluates expression e and returns the value */
char *eval_term (expr *e)
{
	switch (e->type)
	{
		case CHEAT: return "Cheat";
        case HONEST: return "Honest";
        case UNDEF: return "Undef";
        case LAST:
            if(last == 'u') return "Undef";
            else if(last == 'h') return "Honest";
            else if(last == 'c') return "Cheat";
            break;
        case RAND: rand();
        case NUM:  return e->val;
		case 0: return e->var->value; 
        break;
	}
}

int eval_cond (expr *e) {
    switch(e->type) {
		case CHEAT: return 'c';
        case HONEST: return 'h';
        case UNDEF: return 'u';
        case LAST: return last;
        case RAND: rand();
        case NUM:  return atoi(e->val);
		case 0: return atoi(e->var->value); 
		case OR: return eval_cond(e->left) || eval_cond(e->right);
		case AND: return eval_cond(e->left) && eval_cond(e->right);
		case EQ: return eval_cond(e->left) == eval_cond(e->right);
		case INF: return eval_cond(e->left) < eval_cond(e->right);
		case SUP: return eval_cond(e->left) > eval_cond(e->right);
		case NOT: return !eval_cond(e->left);
        break;
    }
}

int execute (stmt *s)
{
	switch(s->type)
	{
		case ASSIGN:
            printf("assign %s : %s\n", s->var->name, eval_term(s->expr));
			s->var->value = eval_term(s->expr);
            return 0;
		case ';':
			execute(s->left);
			return execute(s->right);
		case IF:
			if (eval_cond(s->expr)) return execute(s->left);
            return 0;
		case ELSE:
			if (eval_cond(s->expr)) return execute(s->left);
            else return execute(s->right);
        case RET: 
            last = eval_cond(s->expr);
            return eval_cond(s->expr);
	}
}

void step (agt *a1, agt *a2)
{
    int temp;

    last = a1->lst;
    int res1 = execute((a1->st)->s);
    temp = last;
    last = a2->lst;
    int res2 = execute((a2->st)->s);
    a1->lst = last;
    a2->lst = temp;

    printf("Agent 1 : %c\n", res1);
    printf("Agent 2 : %c\n", res2);

	switch (res1)
	{
		case 'h':
			switch(res2)
			{
				case 'h':
                    printf("%d %d\n", a1->spw, a2->spw);
					a1->spw +=rewardhh;
					a1->lf -=1;
					a2->spw +=rewardhh;
					a2->lf -=1;
                break;
				
				case 'c':
                    printf("%d %d\n", a1->spw, a2->spw);
					a1->spw +=rewardhc;
					a1->lf -=1;
					a2->spw +=rewardch;
					a2->lf -=1;
                break;
			}
        break;
		case 'c':
			switch(execute((a2->st)->s))
				{
					case 'h':
                        printf("%d %d\n", a1->spw, a2->spw);
						a1->spw +=rewardch;
						a1->lf -=1;
						a2->spw +=rewardhc;
						a2->lf -=1;
                    break;
					
					case 'c':
                        printf("%d %d\n", a1->spw, a2->spw);
						a1->spw +=rewardcc;
						a1->lf -=1;
						a2->spw +=rewardcc;
						a2->lf -=1;
                    break;
				}
        break;
	}
}

/* Prints expression e */
void print_expr (expr *e)
{
	switch (e->type)
	{
		case OR: 
            print_expr(e->left); printf(" || "); print_expr(e->right);
            break;
		case AND: 
            print_expr(e->left); printf(" && "); print_expr(e->right);
            break;
		case EQ: 
            print_expr(e->left); printf(" == "); print_expr(e->right);
            break;
		case INF: 
            print_expr(e->left); printf(" < "); print_expr(e->right);
            break;
		case SUP: 
            print_expr(e->left); printf(" > "); print_expr(e->right);
            break;
		case NOT: print_expr(e->left); break;
		case CHEAT: printf("Cheat"); break;
        case HONEST: printf("Honest"); break;
        case UNDEF: printf("Undef"); break;
        case LAST: printf("last"); break;
        case RAND: printf("%s", e->val); break;
        case NUM: printf("%s", e->val); break;
		case 0: printf("%s", e->var->name); break;
	}
}


/* Prints statement stmt in form of an AST */
void print_stmt(stmt *s, int tabs)
{
    printf("\n");
    for(int i=0;i<tabs;i++) {
        printf("\t");
    }
    printf("| ");
	switch(s->type)
	{
		case ASSIGN:
            printf("ASSIGN var %s to expr :", s->var);
			print_expr(s->expr);
			break;
		case ';':
			print_stmt(s->left, tabs);
			print_stmt(s->right, tabs);
			break;
		case IF:
            printf("IF: ");
			print_expr(s->expr);
            print_stmt(s->left, tabs+1);
			break;
		case ELSE:
            printf("IF-ELSE: ");
			print_expr(s->expr);
            print_stmt(s->left, tabs+1);
            print_stmt(s->right, tabs+1);
			break;
        case RET: 
            printf("RET: ");
            print_expr(s->expr);
            break;
	}
}

/* Prints strats list st in form of an AST */
void print_strats(strats *st) {
    if(st->next) {
        print_strats(st->next);
    }
    printf("\nSTRAT: %s", st->name);
    print_stmt(st->s,0);
    printf("\n");
}

/* Translate statement to original code */
void translate_stmt(stmt *s)
{
	switch(s->type)
	{
		case ASSIGN:
            printf("%s = ", s->var->name);
			print_expr(s->expr);
			break;
		case ';':
			translate_stmt(s->left);
            printf(";\n\t");
			translate_stmt(s->right);
			break;
		case IF:
            printf("if ");
			print_expr(s->expr);
            printf(" ");
            translate_stmt(s->left);
			break;
		case ELSE:
            printf("if ");
			print_expr(s->expr);
            printf(" ");
            translate_stmt(s->left);
            printf(" else ");
            translate_stmt(s->right);
			break;
        case RET: 
            printf("return ");
            print_expr(s->expr);
            break;
	}
}

/* Translate defs to original code */
void translate_defs(var *v) {
    if(v->next) {
        translate_defs(v->next);
    }

    printf("\tdef %s = %s\n", v->name, v->value);
}

/* Translates a list of strategies to original code */
void translate_strats(strats *st) {
    if(st->next) {
        translate_strats(st->next);
    }
    printf("Strategy %s\n", st->name);
    if(st->defs) translate_defs(st->defs);
    printf("\t");
    translate_stmt(st->s);
    printf("\n\n");
}

/* Translates constant lists to original code */
void print_constants(var *v) {
    if(v->next) {
        print_constants(v->next);
    }

    printf("\t %s = %s\n", v->name, v->value);
}

/* Translates to original code, given a strategies list and a constants list */
void translate_prog(strats *st, var *v) {
    translate_strats(st);
    printf("Constants\n");
    print_constants(v);
}

/****************************************************************************/

int main (int argc, char **argv)
{
	if (argc <= 1) { yyerror("no file specified"); exit(1); }
	yyin = fopen(argv[1],"r");
	if (!yyparse()) print_strats(program_strats);
    printf("\n");

	int nb_step = 0;
	printf("Choisissez un nombre d'interactions : \n");
	scanf("%d", &nb_step);
	char S1[100];
	char S2[100];
	printf("Choisissez la strategie du premier agent : \n");
	scanf("%s",&S1);
	printf("Choisissez la strategie du deuxieme agent : \n");
	scanf("%s",&S2);
	strats* s1= find_strat(S1);
	strats* s2= find_strat(S2);	
	agt *a1=make_agt(1,s1,nb_step,spawn,'u');
	agt *a2=make_agt(2,s2,nb_step,spawn,'u');
	int i=0;
	for(i=0;i<nb_step;i++){
        printf("Step %d:\n", i);
		step(a1,a2);
	}
	printf("Suite a leur recontre, les agents on respectivement %d et %d points de spawn",a1->spw, a2->spw);
}

