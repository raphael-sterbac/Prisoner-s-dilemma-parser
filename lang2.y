%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
    int weight;
    int num_agents;
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
strats* make_strat(stmt *s, char *name, strats *next, var *defs, int weight) {
    strats *st = malloc(sizeof(strats));
    st->s = s;
    st->name = name;
    st->next = next;
    st->defs = defs;
    st->weight = weight;
    st->num_agents = 0;
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
%type <e> arithmetic 
%type <s> stmt 
%type <st> strats

// Defines tokens (lexèmes) for the grammar 
%token ASSIGN OR AND NOT EQ IF ELSE STRAT CONST DEF CHEAT HONEST RET INF INF-EQ SUP-EQ UNDEF LAST  ADD MINUS
%token <i> IDENT
%token <i> IDSTRAT
%token <i> NUM 
%token <i> RAND 

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
%right INF-EQ
%right SUP-EQ
%right ADD
%right MINUS
%right IF
%right ELSE

// Defines the grammar to parse our language
%%
prog: strats { program_strats = $1; }
    | strats CONST declist { program_strats = $1; }

declist	:  { $$ = NULL; program_vars = NULL; }
        | declist IDENT ASSIGN NUM	{ ($$ = make_ident($2,$4))->next = $1; }

strats: STRAT IDSTRAT defs stmt { $$ = make_strat($4,$2,NULL,$3,1); }
    | strats STRAT IDSTRAT defs stmt { $$ = make_strat($5,$3,$1,$4,1); }
    | STRAT IDSTRAT '(' NUM ')' defs stmt { $$ = make_strat($7,$2,NULL,$6,atoi($4)); }
    | strats STRAT IDSTRAT '(' NUM ')' defs stmt { $$ = make_strat($8,$3,$1,$7,atoi($5)); }

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


expr	: expr OR expr	{ $$ = make_expr(OR,NULL,$1,$3, NULL); }
	| expr AND expr	{ $$ = make_expr(AND,NULL,$1,$3, NULL); }
	| expr EQ expr	{ $$ = make_expr(EQ,NULL,$1,$3, NULL); }
	| expr INF expr	{ $$ = make_expr(INF,NULL,$1,$3, NULL); }
	| expr SUP expr	{ $$ = make_expr(SUP,NULL,$1,$3, NULL); }
	| expr INF-EQ expr	{ $$ = make_expr(INF-EQ,NULL,$1,$3, NULL); }
	| expr SUP-EQ expr	{ $$ = make_expr(SUP-EQ,NULL,$1,$3, NULL); }
	| NOT expr	    { $$ = make_expr(NOT,NULL,$2,NULL, NULL); }
    | RAND          { $$ = make_expr(RAND, NULL, NULL, NULL, $1); }
    | term          { $$ = $1; }
	| '(' expr ')'	{ $$ = $2; }

term: CHEAT     { $$ = make_expr(CHEAT, NULL, NULL, NULL, "Cheat"); }
    | HONEST    { $$ = make_expr(HONEST, NULL, NULL, NULL, "Honest"); }
    | UNDEF     { $$ = make_expr(UNDEF, NULL, NULL, NULL, "Undef"); }
    | LAST      { $$ = make_expr(LAST, NULL, NULL, NULL, "last"); }
    | arithmetic {$$ = $1;}

arithmetic:  NUM       { $$ = make_expr(NUM, NULL, NULL, NULL, $1); }
    | IDENT		{ $$ = make_expr(0,find_ident($1),NULL,NULL, NULL); }
    | arithmetic ADD arithmetic { $$ = make_expr(ADD, NULL, $1, $3, NULL); }
    | arithmetic MINUS arithmetic       { $$ = make_expr(MINUS, NULL, $1, $3, NULL); }

ret :  IDENT    { $$ = make_expr(0,find_ident($1),NULL,NULL, NULL); }
    | CHEAT     { $$ = make_expr(CHEAT, NULL, NULL, NULL, NULL); }
    | HONEST    { $$ = make_expr(HONEST, NULL, NULL, NULL, NULL); }
    | LAST      { $$ = make_expr(LAST, NULL, NULL, NULL, "last"); }

%%

#include "langlex.c"

/****************************************************************************/
/* programme interpreter      :                                             */

int random_int(int min, int max) {
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

int string_to_int(char* s) {
    int i;

    sscanf(s, "%*[^0123456789]%d", &i);

    return i;
}

int last = 'u';

int total_pts;
int pop_totale;
int pop;
int nb_libres;
int agt_morts;
char snum[100];
agt* agents;

/* Evaluates expression e as a term and returns the value as a string */
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
        case RAND: 
            sprintf(snum,"%d",random_int(0,string_to_int(e->val)));
            return snum;
        case NUM:  return e->val;
        case ADD: 
            sprintf(snum,"%d",atoi(eval_term(e->left)) + atoi(eval_term(e->right)));
            return snum;
        case MINUS: 
            sprintf(snum,"%d",atoi(eval_term(e->left)) - atoi(eval_term(e->right)));
            return snum;
		case 0: return e->var->value; 
        break;
	}
}

// Eval expression e as a condition, and returns the value as an int
int eval_cond (expr *e) {
    switch(e->type) {
		case CHEAT: return 'c';
        case HONEST: return 'h';
        case UNDEF: return 'u';
        case LAST:  return last;
        case RAND: return random_int(0,string_to_int(e->val));
        case NUM:  return atoi(e->val);
		case 0: 
            if(!strcmp(e->var->value,"Undef")) return 'u';
            else if(!strcmp(e->var->value, "Honest")) return 'h';
            else if(!strcmp(e->var->value, "Cheat")) return 'c';
            else if(!strcmp(e->var->value, "last")) return last;
            else return atoi(e->var->value); 
        case ADD: return atoi(eval_term(e->left)) + atoi(eval_term(e->right));
        case MINUS: return atoi(eval_term(e->left)) - atoi(eval_term(e->right));
		case OR: return eval_cond(e->left) || eval_cond(e->right);
		case AND: return eval_cond(e->left) && eval_cond(e->right);
		case EQ: return eval_cond(e->left) == eval_cond(e->right);
		case INF: return eval_cond(e->left) < eval_cond(e->right);
		case SUP: return eval_cond(e->left) > eval_cond(e->right);
		case INF-EQ: return eval_cond(e->left) <= eval_cond(e->right);
		case SUP-EQ: return eval_cond(e->left) >= eval_cond(e->right);
		case NOT: return !eval_cond(e->left);
        break;
    }
}

// Execute statement s
int execute (stmt *s)
{
	switch(s->type)
	{
		case ASSIGN:
			s->var->value = eval_term(s->expr);
            return 'u';
		case ';':
			execute(s->left);
			return execute(s->right);
		case IF:
			if (eval_cond(s->expr)) return execute(s->left);
            return 'u';
		case ELSE:
			if (eval_cond(s->expr)) return execute(s->left);
            else return execute(s->right);
        case RET: 
            return eval_cond(s->expr);
	}
}

// Find a random strategy from the list program_strats, taking weights into account
strats *random_strat(void) {
    strats *weighted_strats;
    strats *w;
	strats *v = program_strats;
    int num_strats = 0;
    int r;

    weighted_strats = w = make_strat(v->s,v->name,NULL,v->defs,v->weight); 

    for(int i = 1; i < v->weight; i++) {
            w->next = make_strat(v->s,v->name,NULL,v->defs,v->weight); 
            w = w->next;
            num_strats++;
    }

    v = v->next;

	while (v) {
        for(int i = 0; i < v->weight; i++) {
            w->next = make_strat(v->s,v->name,NULL,v->defs,v->weight); 
            w = w->next;
            num_strats++;
        }

        v = v->next;
    }
    
    r = random_int(0,num_strats);
    w = weighted_strats;

    while(r > 0) {
        r--;
        w = w->next;
    }

    return find_strat(w->name);
}

// Find a new strategy from dad's strategy
strats *new_strat(strats* old_st) {
    int r = random_int(0,100);
    if(r >= 0 && r < mutation) {
        return random_strat();
    } else {
        return old_st;
    }
}

// Updates points for agents a1 and a2
void a_jour_pts(agt *a1, agt *a2,int i, int j){
	a1->spw +=i;
	a1->lf -=1;
	a2->spw +=j;
	a2->lf -=1;
	total_pts+=i+j;
}

//Copy a2 in a1
void copy_agt(agt *a1,agt *a2){
	a1->st=a2->st;
	a1->lf=a2->lf;
	a1->spw=a2->spw;
	a1->lst=a2->lst;
}

//Adds the spawned agent at the end of agents
void ajt_agt(agt *a){ 
	agents[pop].st=new_strat(a->st);
    agents[pop].st->num_agents++;
	agents[pop].lf=life;
	agents[pop].spw=0;
	agents[pop].lst='u';
}

// Prints strategies distribution
void distrib_strat(void) {
	strats *v = program_strats;
	while (v ){
		printf("%f pourcents de %s \n", ((float)(v->num_agents))/((float)pop), v->name);
		v = v->next;
	}
}

// Meeting between agents a1 and a2
void step (agt *a1, agt *a2)
{
    int temp;

    last = a1->lst;
    int res1 = execute((a1->st)->s);
    last = a2->lst;
    int res2 = execute((a2->st)->s);
    a1->lst = res2;
    a2->lst = res1;
	switch (res1)
	{
		case 'h':
			switch(res2)
			{
				case 'h':
					a_jour_pts(a1,a2,rewardhh,rewardhh);
                break;
				
				case 'c':
					a_jour_pts(a1,a2,rewardhc,rewardch);
                break;
			}
        break;
		case 'c':
			switch(execute((a2->st)->s))
				{
					case 'h':
						a_jour_pts(a1,a2,rewardch,rewardhc);
                    break;
					
					case 'c':
						a_jour_pts(a1,a2,rewardcc,rewardcc);
                    break;
				}
        break;
	}
}

// Manages the creation of agents
void refresh(int r1){
	agt *a1=&agents[r1]; // agent number r1
	if (a1->spw >= spawn){ //spawn of an agent
		if (nb_libres>pop){ //case where agents is large enough to welcome a new agent
			ajt_agt(a1);
			pop_totale+=1;
			pop+=1;
		}
		else{ //when agents is too small, we duplicate its size
			agents=realloc(agents,2*pop*sizeof(agt));
			a1=&agents[r1];
			ajt_agt(a1);
			nb_libres = 2*nb_libres;
			pop_totale+=1;
			pop+=1;
		}
		a1->spw-=spawn; //a1 uses his spawn points
	}
}

void dead(int r1, int r2){
	agt *a1=&agents[r1]; // agent number r1
	agt *a2=&agents[r2]; // agent number r2
	if (a1->lf <= 0){ //death of an agent, we permutate it with the last agent of agents and decrease pop
        a1->st->num_agents -= 1; //decrease of the numbre of agent following the strategy
		copy_agt(a1,agents+(pop-1));
		pop-=1;
		agt_morts+=1;
		if (r2==pop){//if a2 was the last agent
			a2=&agents[r1];
		}
	}
	if (a2->lf <= 0){ 
        a2->st->num_agents -= 1; 
		copy_agt(a2,agents+(pop-1));
		pop-=1;
		agt_morts+=1;
	}
}

void tlf(){
	int i=0;
	int t=0;
	for (i=0;i<pop;i++){
		t+=agents[i].lf;
	}
	printf("Total life points : %d",t);
}


// Simulates meetings*initials random meetings
void meet(){
	int i;
	int j;
	for (i=0;i<meetings;i++){
		if(pop<=0 || pop==1){ //when the agents are estinguished
			break;
		}
		printf("\nIl reste %d agents \n",pop);
		printf("La moyenne de points generes par agents est %f \n", ((float)total_pts/(float)pop_totale));
		printf("La distribution des strategies est :\n");
        distrib_strat(); //prints the current distribution of strategies
		tlf();
		for (j=0;j<intervals;j++){
			if(pop<=0 || pop==1){ //when the agents are estinguished
				break;
			}
			int r1 = random_int(0,pop-1); //choose a first random agent
			int r2=random_int(0,pop-2); //choose a second, different agent
			if (r2>=r1){
				r2+=1;
			}
            step(&agents[r1],&agents[r2]); //simulates one encounter
			refresh(r1); //update spawn
			refresh(r2);
			dead(r1,r2); //update life points
		}
	}
	if (pop<=0){
		printf("\nIl ne reste plus d'agents\n");
	}
	if (pop==1){
		printf("\nIl reste un seul agent\n");
	}
}

// Create agent i
void create_agent(int i) {
    agents[i].st = random_strat();
    agents[i].st->num_agents += 1;
    agents[i].lf = life;
    agents[i].spw = 0;
    agents[i].lst = 'u';
}

// Initialize agents
void init() {
    int i;
    for(i = 0; i < nb_libres; i++) { //creation of initial agents
        create_agent(i);
    }
    distrib_strat();
    meet(); //launch the meetings
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
		case ADD: 
            print_expr(e->left); printf(" + "); print_expr(e->right);
            break;
		case MINUS: 
            print_expr(e->left); printf(" - "); print_expr(e->right);
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
            printf("ASSIGN var %s to expr: ", s->var->name);
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
    printf("\nSTRAT: %s, weight %d", st->name, st->weight);
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
    srand(time(NULL));

	if (argc <= 1) { yyerror("no file specified"); exit(1); }
	yyin = fopen(argv[1],"r");
	if (!yyparse()) print_strats(program_strats);

	total_pts=0; //save the total number of points earned by agents
	pop_totale=initial; //save the total number of agents that once existed
	pop=pop_totale; //the actual number of agents
	nb_libres=pop; //actual size of agents
	agt_morts=0; //number of agents that died
	agents=malloc(pop*sizeof(agt)); //array of living agents

    init(); //starts the simulation
	printf("\nFIN DE LA SIMULATION\n\n");
    free(agents);
}

