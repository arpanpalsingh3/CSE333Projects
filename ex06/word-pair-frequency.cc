#include <iostream>
#include <cstdlib>
#include <string>
#include <map>
#include <iomanip>

/* Usage() */
void usage(const char *exe_name)
{
  std::cout << "Usage: " << exe_name << std::endl;
  std::cout << "\tReads from stdin.  Prints list of words seen and number" << std::endl;
  std::cout << "\tof occurrences of each." << std::endl;
}

/* Reads stdin.  Writes to stdout each words seen in the input
   and the number of times each word was seen. */
int main(int argc, char *argv[])
{
  std::string next_word;
  // The keys are now pairs in this map
  std::map<std::pair <std::string,std::string>,int> word_map;

  if ( argc != 1 ) {
    usage(argv[0]);
    return EXIT_FAILURE;
  }

  int total_words = 0;
  std::string prevWord; 
  /* Whatever this produces is our definition of "a word." */
  while ( std::cin >> next_word )
    {
      /* Make sure I'm on the second word so I know for sure prevWord is 
        initialized. Then create the pair, add it to map. If it's already there
        then increment the counter. If not, create new key-value pair. */
      if ( total_words > 0 ) { 
        std::pair <std::string,std::string> twoGram (prevWord, next_word);
        word_map[twoGram]++;
      }
      total_words++;
      prevWord = next_word;
    }

  /* Iterate over all key-value pairs in the map.
     "auto" means "I don't know the type, you figure it out".
     The '&' means "a reference" (not a copy).
     We'll talk about these things in more detail later. */
  for (auto const & [key,value]: word_map)
    {
      // Print 2 words, most everything else stays the same 
      std::cout << std::setw(10) << key.first << std::setw(10) << key.second << std::setw(10)
                << std::fixed << std::setprecision(3)
                << 100.0*(float)value/(float)total_words  << "%\n";
    }

  return EXIT_SUCCESS;
}
