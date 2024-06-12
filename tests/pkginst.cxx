#include "../src/libcards.h"

using namespace cards;

class Pkg_inst : public pkgdbh
{
	public:
		Pkg_inst();
		std::set<std::string> getList();
};

Pkg_inst::Pkg_inst()
	: pkgdbh("")
{
	m_root="/";
	getListOfPackagesNames("");
	buildCompleteDatabase(true);
}
std::set<std::string> Pkg_inst::getList()
{
	std::set<std::string> m_List;
	for (auto i : m_listOfPackages)
		m_List.insert(i.first + " " + i.second.version() + " " + i.second.description());
	return m_List;
}
int main(int argc, char** argv)
{
	Pkg_inst ListOfPackages;

	std::set<std::string> List = ListOfPackages.getList();
	for (auto i:List)
		std::cout << i << std::endl;


	return 0;
}
// vim:set ts=2 :
