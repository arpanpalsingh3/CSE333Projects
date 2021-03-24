#include <iostream>
#include <chrono>
#include <random>
#include <iomanip>

#include "List333.h"

/* Values, and the list itself, are generated at random each run */
static std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
static std::normal_distribution<double> distribution(0.0,1.0);

/* Constructor.  If autogen == true, randomly decides whether
   or not to create a successor node. That is completely artificial,
   and is implemented just for the purposes of the exercise. */
List333::List333(bool autogen)
{
  pValue_ = new double();
  /* If new fails we throw, but there's no one there to catch so 
     execution will be terminated. */
  if ( !pValue_ )
    throw "new failure in List333::List333!";
  /* Generate and record a random value */
  *pValue_ = distribution(generator);

  /* Deal with possible successor generation */
  if ( autogen && value() < 1.0)
    next_ = new List333(autogen);
  else
    next_ = nullptr;
}
List333::~List333() { 
  delete pValue_;
  delete next_;
}

/* Copy constructor */
List333::List333(const List333 &other)
{
  next_ = nullptr;
  pValue_ = new double();
  if ( !pValue_ )
    throw "new failure in List333::List333(other)!";
  
  /* Use assignment to complete initialization */
  *this = other;
}

List333 & List333::operator=(const List333 & other)
{
  /* Have to worry about assignment from a List333 that is a successor
     to this list */
  List333 *oldNext = next_;
  
  value() = other.value();
  List333 *me = this;
  for (List333 const *other_next=other.next(); other_next; other_next=other_next->next())
    {
      List333 *newNode = new List333(false);
      newNode->value() = other_next->value();
      me->next_ = newNode;
      me = newNode;
    }
  me->next_ = nullptr;

  if ( oldNext )
    delete oldNext;
  
  return *this;
}

 
