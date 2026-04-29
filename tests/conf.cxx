#include "../src/libcards.h"

int main () {
	cards::conf config("cards.conf");
	for (auto i : config.dirUrl())
		std::cout << "url: "
			<< i.url
			<< "\ncollection: "
			<< i.collection
			<< "\ndepot: "
			<< i.depot
			<< std::endl;
	return 0;
}
// vim:set ts=2 :
