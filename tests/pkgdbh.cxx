#include "../src/libcards.h"

int main(int argc, char** argv)
{
	pkgdbh ListOfPackages;

	for (auto i:ListOfPackages.getSetOfPackagesNames()) {
		if (ListOfPackages.getCategories(i).empty()) continue;
		std::cout << i << ":";
		for (auto categories:ListOfPackages.getCategories(i)) {
				std::cout << categories ;
		}
		std::cout << std::endl;
	}

	return(EXIT_SUCCESS);
}
// vim:set ts=2 :
