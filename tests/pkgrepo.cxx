#include "../src/libcards.h"

int main (int arg, char** argv) {

    cards::pkgrepo pkgrepo(argv[1]);

    for (auto i: pkgrepo.getListOfPackagesFromGroup(argv[2]))
        std::cout << i << std::endl;

    return 0;
}
