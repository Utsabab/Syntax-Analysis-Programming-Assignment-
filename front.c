/* front.c - a lexical analyzer system for simple
 arithmetic expressions */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/* Global declarations */
/* Variables */
int charClass;
char lexeme [100];
char nextChar;
int lexLen;
int token;
int nextToken;
FILE *in_fp, *fopen();

char* each_line = NULL;
size_t read = 0;
size_t len = 0;
int num_col = 1;
int num_row = 0;
int break_at = 0;
int char_index;
char last_read_char;	
char read_till[1000];
char last_read_lex[1000];

/* Function declarations */
void addChar();
void getChar();
void getNonBlank();
void term();
void expr();
void factor();
void error();
int lex();

/* Character classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

/* Token codes */
#define INT_LIT 10
#define IDENT 11
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26

/******************************************************/

/* main driver */
void main(int argc, char* argv[]) {

	if (argc != 2) {
		printf("Number of argument doesn't match");
		exit(0);
	}

/* Open the input data file and process its contents */
	char* file = argv[1];

	if ((in_fp = fopen(file, "r")) == NULL)
 	printf("ERROR - cannot open %s\n", file);
 	else {
 		while ((read = getline(&each_line, &len, in_fp)) != -1) {
 			strcpy(read_till, "");
 			strcpy(last_read_lex, "");
 		 	num_row++;
 		 	num_col = 1;
 		 	char_index = 0;
 		 	break_at = 0;
 		 	getChar();
 		 	if (each_line != NULL) {
 		 		do {
 					lex();
 					expr();
 					if (break_at == 1) {
 						break;
 					}
 				} while (nextToken != EOF);

 		 	}
		}
 	}
}

//while (fgets()) {


/*****************************************************/
/* lookup - a function to lookup operators and parentheses
 			and return the token */

int lookup(char ch) {
	switch (ch) {
	 	case '(':
	 	addChar();
	 	nextToken = LEFT_PAREN;
	 	break;

	 	case ')':
	 	addChar();
	 	nextToken = RIGHT_PAREN;
	 	break;

	 	case '+':
	 	addChar();
	 	nextToken = ADD_OP;
	 	break;

		case '-':
		addChar();
		nextToken = SUB_OP;
		break;

		case '*':
		addChar();
		nextToken = MULT_OP;
		break;

		case '/':
		addChar();
		nextToken = DIV_OP;
		break;

		default:
		addChar();
		nextToken = EOF;
		break;
	}
	strcat(read_till, lexeme);
	strcat(read_till, " ");
	return nextToken;
}

/*****************************************************/
/* addChar - a function to add nextChar to lexeme */

void addChar() {
 	if (lexLen <= 98) {
 		lexeme[lexLen++] = nextChar;
 		lexeme[lexLen] = 0;
 	}
 	else
 	printf("Error - lexeme is too long \n");
}

/*****************************************************/
/* getChar - a function to get the next character of
 			input and determine its character class */

void getChar() {
 	if (each_line[char_index] != '\n' && each_line[char_index] != '\0') {
 		num_col++;
 		nextChar = each_line[char_index++];
 		if (isalpha(nextChar))
 			charClass = LETTER;
 		else if (isdigit(nextChar))
 			charClass = DIGIT;
 		else charClass = UNKNOWN;
 	}
 	else
 		charClass = EOF;
}

/*****************************************************/
/* getNonBlank - a function to call getChar until it
 				returns a non-whitespace character */

void getNonBlank() {
	while (isspace(nextChar))
 		getChar();
}


/*****************************************************/
/* lex - a simple lexical analyzer for arithmetic
 	expressions */

int lex() {
	lexLen = 0;
 	getNonBlank();
 	last_read_char = nextChar;
 	strcpy(last_read_lex, lexeme);
 	switch (charClass) {

 	/* Parse identifiers */
 		case LETTER:
 		addChar();
 		getChar();
 		while (charClass == LETTER || charClass == DIGIT) {
 			addChar();
 			getChar();
 		}
 		strcat(read_till, lexeme);
 		strcat(read_till, " ");
 		nextToken = IDENT;
 		break;

	/* Parse integer literals */
 		case DIGIT:
 		addChar();
 		getChar();
 		while (charClass == DIGIT) {
 			addChar();
 			getChar();
 		}
 		strcat(read_till, lexeme);
 		strcat(read_till, " ");
 		nextToken = INT_LIT;
 		break;

	/* Parentheses and operators */
 		case UNKNOWN:
 		lookup(nextChar);
 		getChar();
 		break;

	/* EOF */
 		case EOF:
 		nextToken = EOF;
 		lexeme[0] = 'E';
 		lexeme[1] = 'O';
 		lexeme[2] = 'F';
 		lexeme[3] = 0;
 		break;

 	} /* End of switch */

 	printf("Next token is: %d, Next lexeme is %s\n",
 	nextToken, lexeme);
 	return nextToken;
} /* End of function lex */

/* expr
Parses strings in the language generated by the rule:
<expr> -> <term> {(+ | -) <term>}
*/
void expr() {
	printf("Enter <expr>\n");
/* Parse the first term */
	term();
	if (break_at == 1) {
		return;
	}
/* As long as the next token is + or -, get
the next token and parse the next term */
	while (nextToken == ADD_OP || nextToken == SUB_OP) {
		lex();
		term();
		if (break_at == 1) {
			return;
		}
	}
	printf("Exit <expr>\n");
} /* End of function expr */

/* term
Parses strings in the language generated by the rule:
<term> -> <factor> {(* | /) <factor>)
*/
void term() {
	printf("Enter <term>\n");
/* Parse the first factor */
	factor();
	if (break_at == 1) {
		return;
	}
/* As long as the next token is * or /, get the
next token and parse the next factor */
	while (nextToken == MULT_OP || nextToken == DIV_OP) {
		lex();
		factor();
		if (break_at == 1) {
			return;
		}
	}
	printf("Exit <term>\n");
} /* End of function term */

/* factor
Parses strings in the language generated by the rule:
<factor> -> id | int_constant | ( <expr )
*/
void factor() {
	printf("Enter <factor>\n");
/* Determine which RHS */
	if (nextToken == IDENT || nextToken == INT_LIT)
/* Get the next token */
		lex();
/* If the RHS is ( <expr>), call lex to pass over the
left parenthesis, call expr, and check for the right
parenthesis */
	else {
		if (nextToken == LEFT_PAREN) {
			lex();
			expr();
			if (nextToken == RIGHT_PAREN)
				lex();
			else
				error();
		} /* End of if (nextToken == ... */

/* It was not an id, an integer literal, or a left
parenthesis */
		else
			error();
	} /* End of else */
	printf("Exit <factor>\n");;
}
/* End of function factor */