// Include core libraries.
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Include mpc, an external parser toolbox.
#include "mpc.h"

// Declare a buffer for user input with a size of 2048.
static char buffer[2048];


/* Windows Specific */
#ifdef _WIN32
#include <string.h>

// Fake Readline for Windows.
char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy[strlen(cpy)-1] = '\0';
  return cpy;
}

// Cheap add_history fakeout for Windows.
void add_history(char* unused) {}


/* Unix Specifc */
#else
#include <editline/readline.h>
//#include <editline/history.h>
#endif


/* Extra Functions */
// Create an evaluation function for operators.
long evaluate_operator(long x, char* operator, long y) {
  if (strcmp(operator, "+") == 0) { return x + y; }     // Addition
  if (strcmp(operator, "-") == 0) { return x - y; }     // Subtraction
  if (strcmp(operator, "*") == 0) { return x * y; }     // Multiplication
  if (strcmp(operator, "/") == 0) { return x / y; }     // Division
  //if (strcmp(operator, "^") == 0) { return pow(x, y); } // Exponents
  //if (strcmp(operator, "%") == 0) { return x % y; }     // Moduluses
} // Create an evaluation function for expressions.
long evaluate(mpc_ast_t* t) {
  // If tagged as a number, return it directly.
  // FIXME: PROBLEMATIC ERROR
  if (strstr(t->tag, "number")) {
    return atoi(t->contents);
  }

  // The operator is a second child.
  printf("stage2");
  char* operator = t->children[1]->contents;

  // Make a variable for the third child.
  long x = evaluate(t->children[2]);

  // Iterate the remaining children and combining.
  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = evaluate_operator(x, operator, evaluate(t->children[i]));
    x++;
  }
  
  return 1;
}


/* Main Code */
int main(int argc, char** argv) {
  // Set up parsers with mpc.
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* Lisp     = mpc_new("lisp");

  // Define the language with the parsers.
  mpca_lang(MPCA_LANG_DEFAULT,
	    " \
              number   : /-?[0-9]+/ ; \
              operator : '+' | '-' | '*' | '/' ; \
              expr     : <number> | '(' <operator> <expr>+ ')' ; \
              lisp     : /^/ <operator> <expr>+ /$/ ; \
	    ",
	    Number, Operator, Expr, Lisp);
  
  // Print information about the lang and REPL.
  puts("Brent's BYOL Project");
  puts("Press Ctrl-C to Exit \n");

  // Insert a loop for the input.
  while (1) {
    // Define the prompt.
    char* input = readline("bbmb-byol> ");

    // Put the line in history.
    add_history(input);

    // Parse the input.
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lisp, &r)) {
      mpc_ast_print(r.output);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    // Calculate and print the output
    long result = evaluate(r.output);
    printf("%li\n", result);
    
    // Clean the input and output in the buffer.
    //mpc_ast_delete(r.output);
    free(input);
  }

  // Clean the parsers and end the program.
  mpc_cleanup(4, Number, Operator, Expr, Lisp);
  return 0;
}
