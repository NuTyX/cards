#include "../src/libcards.h"

int main (int arg, char** argv) {

    std::set<std::string> listOfFiles;
    findRecursiveFile(listOfFiles,argv[2],WS_DEFAULT);

    struct stat buf;
    cards::pkgrepo pkgrepo(argv[1]);
    std::string::size_type pos;

    for (auto i :listOfFiles) {
        pos = i.rfind( '/' );
        if (pos != std::string::npos)
            std::cout << '@' << i.substr( pos + 1 ) << std::endl;
        archive a(i);

        pkgrepo.generateHash(i);
        pkgrepo.generateSign();
        a.printMeta();
        std::cout << HASHSUM << pkgrepo.hash() << std::endl;
        std::cout << SIGNATURE << pkgrepo.sign() << std::endl;
        if (stat(i.c_str(), &buf) != 0)
            std::cerr << "Cannot find size of " << i << std::endl;
        else
            std::cout << SIZE <<buf.st_size << std::endl;

        std::cout << std::endl;
    }
    return 0;
}
