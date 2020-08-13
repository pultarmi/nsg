#include "own.h"
#include <efanna2e/index_nsg.h>
#include <efanna2e/util.h>

int main(int argc, char **argv) {
    efanna2e::IndexNSG index(0, 0, 0, nullptr);
    float* aux = new float[1000000000];
    for(int i=0; i < 1000000000; i++){
        aux[i] = 1;
    }
    usleep(10000000);

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
