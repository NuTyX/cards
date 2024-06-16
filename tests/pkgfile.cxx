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

    if (Repository.checkPortExist("libreoffice")){
        
        std::cout << "libreoffice exist: "
            << Repository.getPortVersion("libreoffice")
            << "-"
            << Repository.getPortRelease("libreoffice")
            << std::endl;
        for (auto i : Repository.getDependencies("libreoffice") )
            std::cout << i
                << " ";
        std::cout << std::endl;
    }
    return 0;
}
// vim:set ts=2 :
