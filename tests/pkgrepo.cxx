#include "../src/libcards.h"

int main () {
    cards::pkgrepo pkgrepo("cards.conf");

    std::cout << pkgrepo.fileName("gcc")
        << std::endl;

    std::cout << pkgrepo.hash("gcc")
        << std::endl;

    std::cout << pkgrepo.sign(pkgrepo.hash("gcc"))
        << std::endl;

    std::cout << pkgrepo.dirName("gcc")
        << std::endl;

    return 0;
}
