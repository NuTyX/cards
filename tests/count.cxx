#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#include  "../src/libcards.h"

int main () {

	cards::conf config("/etc/cards.conf");

	struct package {
		std::string name;
		std::string desc;
		std::string ver;
		std::string rel;
		std::string set;
	};

	struct stat st;
    size_t TotalSize = 0; // Total size for all the depots files
	size_t size = 0; // Size of the file we parse

	for (auto i : config.dirUrl()) {
		std::string s = i.depot + "/" + i.collection + "/.REPO";
    	if (stat(s.c_str(),&st) == 0 )	{
			TotalSize += st.st_size;
		}
	}
	std::vector<package> listOfPackages;

	for (auto i : config.dirUrl()) {
		std::string s = i.depot + "/" + i.collection + "/.REPO";

		int fd = open(s.c_str(),O_RDONLY);

		if (fstat(fd, &st) < 0) {
			perror("fstat");
			close(fd);
			return 1;
		}
		size = st.st_size;
		size_t step = size / 50;

		char *data = static_cast<char*>
			(mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0));

			if (data == MAP_FAILED) {
			perror("mmap");
			close(fd);
			return 1;
		}

		const char *p = data;
		const char *end = data + size;
		package pkg;
		pkg.set = " ";
		while (p < end) {
			const char *nl = (const char*)memchr(p, '\n', size);
			if (!nl)
				nl = end;

			if (p[0] == '\n') {
				listOfPackages.push_back(pkg);
			} else {
				size_t len = nl - p;
				std::string found(p + 1, len - 1);
				switch (p[0]) {
					case 'N':
						pkg.name = found;
						break;
					case 'D':
						pkg.desc = found;
						break;
					case 'V':
						pkg.ver = found;
						break;
					case 'r':
						pkg.rel = found;
						break;
					case 's':
						pkg.set = found;
						break;
					case 'c':
						pkg.set = found;
						break;
				}
			}
			p = nl + 1;
		}
		munmap(data,size);
		close(fd);
	}
	for (auto s: listOfPackages)
		printf("(%s) %s %s-%s %s\n", s.set.c_str(), s.name.c_str(), s.ver.c_str(), s.rel.c_str(), s.desc.c_str());
	
	return 0;
}
// vim:set ts=2 :
