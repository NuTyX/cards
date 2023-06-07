#include <libcards.h>

int main()
{
	Pkgdbh ListOfPackages;

	for (auto i:ListOfPackages.getListOfPackagesName())
		std::cout << i << "\t" << ListOfPackages.getVersion(i) << "-" \
		<< ListOfPackages.getRelease(i) << "\t" << ListOfPackages.getDescription(i) << " " << std::endl;

	return EXIT_SUCCESS;
}
// vim:set ts=2 :
