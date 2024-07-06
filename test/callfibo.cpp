#include <iostream>

extern "C" {
    double fibo(double);
}

int main() {
    double n;
    std::cout << "Inserisci il valore di n: ";
    std::cin >> n;
    std::cout << "fibonacci(" << n << ") = " << fibo(n) << std::endl;
}
