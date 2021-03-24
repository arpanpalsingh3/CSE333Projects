#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/* Here's one of the annoying things about C... No dynamically
   resized strings. */
#define MAX_WORD_SIZE 10

typedef enum state_enum
  {
   state_none,
   state_garbage,
   state_a,
   state_e,
   state_i,
   state_o,
   state_u,
   NUM_STATES,
   state_done
  } State;

typedef enum char_class_enum
  {
   char_ws,
   char_a,
   char_e,
   char_i,
   char_o,
   char_u,
   char_non_ws,
   char_EOF,
   NUM_CHAR_CLASS
  } CharClass;

/* The "object" representing the state machine,
   except for the transition table */
typedef struct system_t
{
  State     current_state;
  char      next_char;
  CharClass next_char_class;
  char*     word;
  int       arrLen;
  int       word_index;
} System;


/* Utility method: converts raw character to class */
CharClass classifyChar(char c)
{
  switch (tolower(c))
    {
    case EOF:  return char_EOF;
    case 'a':  return char_a;
    case 'e':  return char_e;
    case 'i':  return char_i;
    case 'o':  return char_o;
    case 'u':  return char_u;
    default:   if ( isspace(c) ) return char_ws;
    }
  return char_non_ws;
}

/* Utility method: sets currently accumulated word string to the empty string */
void System_resetWord(System *pSystem)
{
  pSystem->word_index = 0;
}

/* Utility method: adds the last read character to the current word string */
void System_addchar(System *pSystem)
{ // checks to see if we need to resize
  if (pSystem->word_index < pSystem->arrLen) {
    // add new letter and move the index up
    pSystem->word[pSystem->word_index] = pSystem->next_char;
    pSystem->word_index++;
  } else {
    // allocate a temp array to store old array vals
    char* temp = malloc(sizeof(char)*pSystem->arrLen);
    for(int i = 0; i < pSystem->arrLen; i ++) {
      temp[i] = pSystem->word[i];
    }
    // temp has the copied characters now
    // so we can free the old allocation of pSystem->word
    free(pSystem->word);
    // now allocate a new one that is 2x the size
    pSystem->word = malloc(sizeof(char)*pSystem->arrLen*2);
    // copy elements back
    for(int i = 0; i < pSystem->arrLen; i ++) {
      pSystem->word[i] = temp[i];
    }
    // free temp
    free(temp);
    // update the arrLen variable
    pSystem->arrLen *= 2;
    // add new letter
    pSystem->word[pSystem->word_index] = pSystem->next_char;
    pSystem->word_index++;
  }

}

/* Utility method: prints the current word */
void System_printWord(System *pSystem)
{
  pSystem->word[pSystem->word_index] = '\0';
  printf("%s\n", pSystem->word);
  pSystem->word_index = 0;
}

/* "Constructor" for a System structure */
void System_initialize(System *pSystem)
{
  pSystem->current_state = state_none;
  pSystem->arrLen = 2;
  pSystem->word = malloc(sizeof(char)*pSystem->arrLen);
  System_resetWord(pSystem);
}
  
/* This declares a function pointer type for functions that
   take a pointer to a System structure and return nothing. */
typedef void (*ActionFunction)(System*);

/* This declares a structure for the elements of the 
   transition table.  (This is a bit primitive because it
   allows only one action per transition.)  */
typedef struct transition_t {
  State next_state;
  ActionFunction action;
} Transition;

/* The transition table for the vowels state machine */
Transition transition_matrix[NUM_STATES][NUM_CHAR_CLASS] =
  {
   { // state_none
    {state_none, NULL},            // char_ws
    {state_a,    System_addchar},  // char_a
    {state_garbage, System_addchar},  // char_e
    {state_garbage, System_addchar},  // char_i
    {state_garbage, System_addchar},  // char_o
    {state_garbage, System_addchar},  // char_u
    {state_none, System_addchar},  // char_non_ws
    {state_done, NULL },           // char_eof
   },
   { // state_garbage
    {state_none, System_resetWord},   // char_ws
    {state_garbage, System_addchar},  // char_a
    {state_garbage, System_addchar},  // char_e
    {state_garbage, System_addchar},  // char_i
    {state_garbage, System_addchar},  // char_o
    {state_garbage, System_addchar},  // char_u
    {state_garbage, System_addchar},  // char_non_ws
    {state_done, NULL },              // char_eof
   },
   { // state_a
    {state_none, System_resetWord}, // ws
    {state_garbage,    System_addchar},   // a
    {state_e,    System_addchar},   // e
    {state_garbage,    System_addchar},   // i
    {state_garbage,    System_addchar},   // o
    {state_garbage,    System_addchar},   // u
    {state_a,    System_addchar},   // all others
    {state_done, NULL}              // eof
   },
   { // state_e
    {state_none, System_resetWord},
    {state_garbage,    System_addchar},
    {state_garbage,    System_addchar},
    {state_i,    System_addchar},
    {state_garbage,    System_addchar},
    {state_garbage,    System_addchar},
    {state_e,    System_addchar},
    {state_done, NULL}
   },
   { // state_i
    {state_none, System_resetWord},
    {state_garbage,    System_addchar},
    {state_garbage,    System_addchar},
    {state_garbage,    System_addchar},
    {state_o,    System_addchar},
    {state_garbage,    System_addchar},
    {state_i,    System_addchar},
    {state_done, NULL}
   },
   { // state_o
    {state_none, System_resetWord},
    {state_garbage,    System_addchar},
    {state_garbage,    System_addchar},
    {state_garbage,    System_addchar},
    {state_garbage,    System_addchar},
    {state_u,    System_addchar},
    {state_o,    System_addchar},
    {state_done, NULL}
   },
   {  // state_u
    {state_none, System_printWord},
    {state_garbage,    System_addchar},
    {state_garbage,    System_addchar},
    {state_garbage,    System_addchar},
    {state_garbage,    System_addchar},
    {state_garbage,    System_addchar},
    {state_u,    System_addchar},
    {state_done, System_printWord}
   }
  };

/* The mainline: has the logic to read the input tokens and make
   state transitions.  This code is state machine independent --
   there's nothing in it that has to do with the vowels problem.  */

int main(int argc, char *argv[]) {
  System system;
  ActionFunction action;

  System_initialize(&system);

  while (system.current_state != state_done )
    {
      system.next_char = getchar();
      system.next_char_class = classifyChar(system.next_char);

      action = transition_matrix[system.current_state][system.next_char_class].action;
      if ( action != NULL )
        action(&system);

      system.current_state = transition_matrix[system.current_state][system.next_char_class].next_state;
    }

  return EXIT_SUCCESS;
}
