#include "../src/repodwl.h"
int main () {
	Repodwl* repodwl = new Repodwl("/etc/cards.conf");
//	std::cout << "Run parseCollectionDirectory" << std::endl;
//	repodwl.parseConfig("/etc/cards.conf");
//	repodwl->parseCollectionDirectory();
	std::cout << "Run checkBinaryExist" << std::endl;
	if (repodwl->checkBinaryExist("cards"))
			std::cout << "cards exist" << std::endl;
	std::cout << repodwl->getPackageFileName("cards") << std::endl;
	std::cout << repodwl->getPackageFileNameSignature("cards") << std::endl;
	delete repodwl;
	return 0;
}
