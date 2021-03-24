#include <iostream>
#include <iomanip>
#include <cmath>

#include "List333.h"

/* Print a List333.  Notice how awkward this is.  We'll make it better
   later with friend functions. */
void PrintList(List333 list, std::string label, std::ostream & out)
{
  out << label << std::endl;
  out << std::setprecision(3) << std::fixed << "\t";
  for (List333 const * next=&list; next; next=next->next())
    out << " " << std::setw(6) << next->value();
  out << std::endl;
}

/* Main()
   Just exercises the List333. */
int main(int argc, char *argv[])
{
  List333 One;
  PrintList(One, "One:", std::cout);

  // Create a copy of One that has only non-negative values
  List333 NonNegativeList(One);
  for ( List333 const * next = &NonNegativeList; next; next = next->next() )
    {
      if ( next->value() < 0 )
        next->value() = -next->value();
    }
  PrintList(NonNegativeList, "Non-Negative List:", std::cout);

  // Assignment
  List333 const * candidate = One.next();
  if ( candidate ) candidate = candidate->next();
  if ( candidate )
    {
      One = *candidate;
      PrintList(One, "Updated One:", std::cout);
    }

  return 0;
}
