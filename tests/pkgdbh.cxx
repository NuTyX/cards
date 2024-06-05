#include "../src/libcards.h"

int main(int argc, char** argv)
{
	pkgdbh ListOfPackages;

	for (auto i:ListOfPackages.getListOfPackagesNames()) {
		for (auto categories:ListOfPackages.getCategories(i)) {
				std::cout << categories << ": " << i << std::endl;
		}
	}

	return(EXIT_SUCCESS);
}
// vim:set ts=2 :
