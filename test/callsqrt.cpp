#include <iostream>

extern "C" {
    double sqrt(double);
}

int main() {
    double x;
    std::cout << "Inserisci il valore di x: ";
    std::cin >> x;
    std::cout << "sqrt(" << x << ") = " << sqrt(x) << std::endl;
}
