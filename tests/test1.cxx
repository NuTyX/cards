#include "../src/repodwl.h"
#include "../src/pkgsync.h"
#include "../src/conf.h"
int main () {
	cards::Conf* config = new cards::Conf;

//	Repodwl* repodwl = new Repodwl("/etc/cards.conf");
//	Pkgsync* pkgsync = new Pkgsync;
//	pkgsync->run();
//	std::cout << "Run parseCollectionDirectory" << std::endl;
//	repodwl.parseConfig("/etc/cards.conf");
//	repodwl->parseCollectionDirectory();
//	std::cout << "Run checkBinaryExist" << std::endl;
//	if (!repodwl->checkBinaryExist("konsole"))
/*	repodwl->downloadPackageFileName("konsole");
	if (repodwl->checkBinaryExist("konsole"))
			std::cout << "konsole exist" << std::endl;
	std::cout << repodwl->getPackageFileName("konsole") << std::endl;
	std::cout << repodwl->getPackageFileNameSignature("konsole") << std::endl;
	delete repodwl;*/
	delete config;
	return 0;
}
