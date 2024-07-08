#include <iostream>

extern "C" double f();

int main() {
  std::cout << f() << std::endl;
}