#include <iostream>

constexpr size_t size = 10;

extern "C" double f(double i), val[size] = {1, 2, 3, 4};

int main() {
  for (int i = 0; i < size-1; i++)
    std::cout << f(i) << std::endl;
  
  std::cout << std::endl;

  for (int i = 0; i < size; i++)
    std::cout << f(i) << std::endl;
  return 0;
}