#include <iostream>

extern "C" double f(), g();

int main() {
  std::cout << "Sum first 5 ones: " <<f() << std::endl;
  std::cout << "Little Gauss on first 5 integers: " << g() << std::endl;
}