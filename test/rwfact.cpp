#include <iostream>

extern "C" {
    double n();
}

extern "C" {
    double printval(double,double);
}

double n() {
  double x;
  std::cout << "Inserisci il valore di n: ";
  std::cin >> x;
  return x;
}

double printval(double n, double fn) {
  std::cout << "factorial(" << n << ") = " << fn << std::endl;
  return 0;
}
