#include <libcards.h>

int main(int argc, char** argv)
{
	Pkgdbh ListOfPackages;

	for (auto i:ListOfPackages.getListOfPackagesNames()) {
		for (auto categories:ListOfPackages.getCategories(i)) {
				std::cout << categories << ": " << i << std::endl;
		}
	}

	return(EXIT_SUCCESS);
}
// vim:set ts=2 :
