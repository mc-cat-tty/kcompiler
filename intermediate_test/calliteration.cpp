#include <iostream>

extern "C" double f(double num);

int main() {
  double num = 10;
  std::cout << "Little Gauss formula of 10: " << f(num) << std::endl;
  return 0;
}