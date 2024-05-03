#include "../src/libcards.h"

int main () {
	cards::Conf config("cards.conf");
	for (auto i : config.dirUrl())
			std::cout << i.url << " " << i.collection << " " << i.dir << std::endl;
	return 0;
}
