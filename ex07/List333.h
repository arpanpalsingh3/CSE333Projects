#include <iostream>
#include <chrono>
#include <random>

class List333
{
public:
  /* A default argument value is given.  If the client code
     doesn't supply a value, then true will be used (assuming
     there is otherwise a match with this constructor).  */
  List333(bool autogen=true);
  List333(const List333 &other);
  ~List333();

  List333 const * next() const { return next_; }
  double & value() const { return *pValue_; }

  /* Overloaded assignment operator. */
  List333 & operator=(const List333 & other);


private:
  List333 * next_;
  double  * pValue_;
};

