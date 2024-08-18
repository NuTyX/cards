#include "../src/libcards.h"

int main () {
	cards::pkgrepo Repository("cards.conf");

	for (auto p : Repository.getBinaryPackageList())
		std::cout << p
			<< std::endl;

	std::string  package = Repository.getBinaryPackageInfo("gcc");

	if (package.size() == 0)
		std::cout << "Package not found"
		<< std::endl;

	std::cout << package;

	return 0;
}
// vim:set ts=2 :
