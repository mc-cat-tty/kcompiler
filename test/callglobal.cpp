#include <iostream>

extern "C" {
  double f();
  double GLOBAL;
}

int main() {
  GLOBAL = 20;
  std::cout << "f(): " << f() << std::endl;
  std::cout << "GLOBAL: " << GLOBAL << std::endl;
}