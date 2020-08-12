#include "own.h"

int main() {
    float* aux = new float[1000000000];
    for(int i=0; i < 1000000000; i++){
        aux[i] = 1;
    }
    usleep(10000000);

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
