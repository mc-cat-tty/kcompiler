#include <iostream>

extern "C" double f(double x);

int main() {
  std::cout << f(10) << std::endl;
  std::cout << f(-10) << std::endl;
  std::cout << f(0) << std::endl;
  return 0;
}