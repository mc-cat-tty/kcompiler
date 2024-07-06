#include <iostream>

extern "C" {
  double f();
  double GLOBAL;
}

int main() {
  GLOBAL = 20;
  std::cout << f() << std::endl;
}