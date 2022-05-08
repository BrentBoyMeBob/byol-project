// Include core libraries.
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Include mpc, an external parser.
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

// History function decoy for Windows.
void add_history(char* unused) {}


/* Unix Specifc */
#else
#include <editline/readline.h>
//#include <editline/history.h>
#endif


/* Extra Functions */
// Add an integer that finds the number of nodes in all of the children.
/*int number_of_nodes(mpc_ast_t* t) {
  if (t->children_num == 0) { return 1; }
  if (t->children_num >= 1) {
    int total = 1;
    for (int i = 0; i < t->children_num; i++) {
      total = total + number_of_nodes(t->children[i]);
    }
    return total;
  }
  return 0;
}*/ // Add a function for evaluating operators.
long eval_op(long x, char* op, long y) {
  if (strcmp(op, "+") == 0) { return x + y; }     // Addition
  if (strcmp(op, "-") == 0) { return x - y; }     // Subtraction
  if (strcmp(op, "*") == 0) { return x * y; }     // Multiplication
  if (strcmp(op, "/") == 0) { return x / y; }     // Division
  if (strcmp(op, "^") == 0) { return pow(x, y); } // Exponents
  if (strcmp(op, "%") == 0) { return x % y; }     // Moduluses
  return 0;
} // Add a function to evaluate the tree.
long eval(mpc_ast_t* t) {
  // If tagged as a number, return it directly back.
  if (strstr(t->tag, "number")) {
    return atoi(t->contents);
  }

  // Make the operator the second child.
  char* op = t->children[1]->contents;

  // Store the third child in the x variable.
  long x = eval(t->children[2]);

  // Iterate the remaining children, and combine them.
  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }
  return x;
}


/* Main REPL */
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
              operator : '+' | '-' | '*' | '/' | '^' | '%' ; \
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
      //mpc_ast_print(r.output);
      long result = eval(r.output);
      printf("%li\n", result);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    //printf("No you're a %s\n", input);

    // Free the input in the buffer.
    free(input);
  }

  // Clean the parsers and end the program.
  mpc_cleanup(4, Number, Operator, Expr, Lisp);
  return 0;
}
