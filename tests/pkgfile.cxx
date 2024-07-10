#include "../src/libcards.h"

int main (int argc, char** argv) {
    cards::pkgfile Repository("cards.conf");
    /*
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
    } */
    cards::ports_t ports = Repository.getListOfPackages();
    if (Repository.checkPortExist(argv[1]))
        for (auto j : Repository.getDependencies(argv[1]))
            std::cout <<"["<< j << "]"<< std::endl;
    else
        std::cout << argv[1] << " not found !!!" << std::endl;
    return 0;
}
