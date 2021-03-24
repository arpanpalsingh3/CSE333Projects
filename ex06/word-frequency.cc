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
  std::map<std::string,int> word_map;

  if ( argc != 1 ) {
    usage(argv[0]);
    return EXIT_FAILURE;
  }

  int total_words = 0;
  /* Whatever this produces is our definition of "a word." */
  while ( std::cin >> next_word )
    {
      /* If there is no entry in the map with key next_word, one is created
         and its value initialized (in this case to zero). */
      word_map[next_word]++;
      total_words++;
    }

  /* Iterate over all key-value pairs in the map.
     "auto" means "I don't know the type, you figure it out".
     The '&' means "a reference" (not a copy).
     We'll talk about these things in more detail later. */
  for (auto const & [key,value]: word_map)
    {
      std::cout << std::setw(10) << key << std::setw(10)
                << std::fixed << std::setprecision(3)
                << 100.0*(float)value/(float)total_words  << "%\n";
    }

  return EXIT_SUCCESS;
}
