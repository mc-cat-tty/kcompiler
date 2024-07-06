#include <iostream>

extern "C" {
    double floor(double);
}

int main() {
    double x;
    std::cout << "Inserisci il valore di x: ";
    std::cin >> x;
    for (int i=0; i<1; i++) {
       std::cout << floor(x) << std::endl;
    }; 
    return 0;
}
