#include <iostream>

extern "C" double f(double num);

int main() {
  double num = 10;
  std::cout << f(num) << std::endl;
  return 0;
}