#include "../src/libcards.h"

int main () {
	cards::pkgrepo Repository("cards.conf");

	std::cout << Repository.getBinaryPackageList().size()
			<< std::endl;

	std::cout << Repository.getPackageFileName("gcc")
			<< std::endl;

	return 0;
}
// vim:set ts=2 :
