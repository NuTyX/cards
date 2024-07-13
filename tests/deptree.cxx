#include "../src/libcards.h"

int main(int arg, char** argv)
{
    cards::deptree Tree;
    std::cout << "   dependencies ( '-->' = listed already)"
            << std::endl;

    Tree.printDependencies(argv[1], "", 0);

    return 0;
}
