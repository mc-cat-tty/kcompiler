#include <iostream>
#include <ctime>

extern "C" {
    double timek();
}

extern "C" {
    double printval(double, double);
}

double timek() {
    std::time_t t = std::time(0);
    return t;
}

double printval(double x, double controlchar) {
  if (controlchar==0) std::cout << x << std::endl;
  else std::cout << "--------------------\n---Array ordinato---\n--------------------\n";
  return 0;
}
