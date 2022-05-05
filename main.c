// Include core libraries.
#include <stdio.h>
#include <stdlib.h>

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


/* REPL Function */
int main(int argc, char** argv) {
  // Print information about the lang and REPL.
  puts("Brent's BYOL Project");
  puts("Press Ctrl-C to Exit \n");

  // Insert a loop for the input.
  while (1) {
    // Define the prompt.
    char* input = readline("bbmb-byol> ");

    // Put the line in history.
    add_history(input);

    // Read the line and input it in the buffer.
    printf("No you're a %s\n", input);

    // Free the input in the buffer.
    free(input);
  }

  // End the program.
  return 0;
}
