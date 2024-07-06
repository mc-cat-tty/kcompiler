#include <iostream>
#include <ctime>

extern "C" {
    double randk();
}

extern "C" {
    double randinit(double);
}

int main() {
    double x;
    std::time_t t = std::time(0);
    randinit(t);
    for (int i=0; i<10; i++) {
       std::cout << randk() << std::endl;
    }; 
    return 0;
}
