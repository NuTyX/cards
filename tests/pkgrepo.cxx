#include "../src/libcards.h"

int main () {
	pkgrepo Repository("cards.conf");
	std::string Port = "gcc";
 	std::cout << "PortDir of "
		<< Port << ": "
		<< Repository.getPortDir(Port)
		<< std::endl;
  std::cout << "BasePortName of "
		<< Port << ": "
		<< Repository.getBasePortName(Port)
		<< std::endl;
  std::cout << "PortVersion of "
		<< Port << ": "
		<< Repository.getPortRelease(Port)
		<< std::endl;
/*	for (auto i:Repository.getListOfPortsDirectory())
		std::cout << i.dir
			<< " : "
			<< i.url
			<< std::endl;
*/
	std::cout << Repository.getPortInfo(Port)
			<< std::endl;
  return 0;
}
// vim:set ts=2 :
