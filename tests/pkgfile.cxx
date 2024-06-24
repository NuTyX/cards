#include "../src/libcards.h"

int main () {
    cards::pkgfile Repository("cards.conf");
    for (auto i : Repository.getListOfPackages()) {
        std::cout << i.first
            << " "
            << i.second.version()
            << "-"
            << i.second.release()
            << " "
            << i.second.description()
            << " "
            << i.second.collection()
            << ": ";
            for (auto j : Repository.getDependencies(i.first) )
               std::cout << j
                   << ",";
            std::cout << std::endl;
    }
    if (Repository.checkPortExist("libreoffice")){
        
        std::cout << "libreoffice exist: "
            << Repository.getPortVersion("libreoffice")
            << "-"
            << Repository.getPortRelease("libreoffice")
            << std::endl;
    }
    return 0;
}
