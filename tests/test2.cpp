#include <libcards.h>

int main(int argc, char** argv)
{
	Pkgdbh ListOfPackages;
	std::string argument = argv[1];

	for (auto i:ListOfPackages.getListOfPackagesNames()) {
		for (auto c:ListOfPackages.getCategories(i)) {
			if (c == argument) {
				std::cout << c << ": " << i << std::endl;
				break;
			}
		}
	}

	return EXIT_SUCCESS;
}
// vim:set ts=2 :
