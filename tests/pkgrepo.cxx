#include "../src/libcards.h"

int main (int arg, char** argv) {
    cards::pkgrepo pkgrepo(argv[1]);

    std::string packageFileName = pkgrepo.fileName(argv[2]);

    std::cout << packageFileName << std::endl;
    std::cout << pkgrepo.dirName(argv[2]) << std::endl;

    std::string fullpathfileName = pkgrepo.dirName(argv[2])  + "/" + packageFileName;

    pkgrepo.generateHash(fullpathfileName);

    std::cout << "hash         : " << pkgrepo.hash()
        << std::endl;

    pkgrepo.generateSign();

    std::cout << "signature    : " << pkgrepo.sign()
        << std::endl;


    pkgrepo.sign(argv[2]);
    pkgrepo.hash(argv[2]);

    if (pkgrepo.checkSign(argv[2]))
        std::cout << "Signature is ok";
    else
        std::cout << "Bad Signature";

    std::cout << std::endl;

    return 0;
}
