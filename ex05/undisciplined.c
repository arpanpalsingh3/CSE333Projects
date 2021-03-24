#include <stdio.h>
#include <ctype.h>

#define MAX_WORD_SIZE 100
char word[MAX_WORD_SIZE];
int word_index = 0;

void addChar(char c)
{
  word[word_index] = c;
  word_index = (word_index+1) % MAX_WORD_SIZE;
}

int main(int argc, char *argv[])
{
  char next_char = getchar();
  while ( next_char != EOF )
    {
      word_index = 0;
      while ( isspace(next_char) )
        next_char = getchar();
      while ( !isspace(next_char) && next_char != 'a' && next_char != EOF )
        {
          addChar(next_char);
          next_char = getchar();
        }
      while ( !isspace(next_char) && next_char != 'e' && next_char != EOF ) 
          {
            addChar(next_char);
            next_char = getchar();
          }
      while ( !isspace(next_char) && next_char != 'i' && next_char != EOF ) 
          {
            addChar(next_char);
            next_char = getchar();
          }
      while ( !isspace(next_char) && next_char != 'o' && next_char != EOF ) 
          {
            addChar(next_char);
            next_char = getchar();
          }
      while ( !isspace(next_char) && next_char != 'u' && next_char != EOF ) 
          {
            addChar(next_char);
            next_char = getchar();
          }
      if ( next_char == 'u' )
        {
          while ( !isspace(next_char) && next_char != EOF )
            {
              addChar(next_char);
              next_char = getchar();
            }
          // print_word
          addChar('\0');
          printf("%s\n", word);
        }
      
    }
}
