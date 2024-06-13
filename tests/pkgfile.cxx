#include "../src/libcards.h"

int main () {
    cards::pkgfile Repository("cards.conf");
    for (auto i : Repository.getListOfPackages())
        std::cout << i.first
            << " "
            << i.second.version()
            << "-"
            << i.second.release()
            << " "
            << i.second.description()
            << " "
            << i.second.collection()
            << std::endl;

    if (Repository.checkPortExist("gcc")){
        
        std::cout << "gcc exist: "
            << Repository.getPortVersion("gcc")
            << "-"
            << Repository.getPortRelease("gcc")
            << std::endl;
    }
    return 0;
}
// vim:set ts=2 :
