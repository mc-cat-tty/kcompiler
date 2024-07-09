#include <iostream>
#include <iomanip>

extern "C" double f(double x, double y), not_test, and_test, or_test;

void printResults(double x, double y) {
  f(x, y);
  
  std::cout
    << "not_test: " << not_test << "\n"
    << "and_test: " << and_test << "\n"
    << "or_test: " << or_test << "\n";
}
/*
true
false
false

false
false
true

false
true
true
*/


int main() {
  printResults(0, 0);
  printResults(5, 0);
  printResults(5, 5);

  return 0;
}