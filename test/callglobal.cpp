#include <iostream>

extern "C" double f(), g(), GLOBAL;

int main() {
  GLOBAL = 20;
  std::cout << "f(): " << f() << std::endl;
  std::cout << "g(): " << g() << std::endl;
  std::cout << "GLOBAL: " << GLOBAL << std::endl;
}