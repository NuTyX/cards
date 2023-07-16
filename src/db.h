#ifndef DB_H
#define DB_H


#include "pkg.h"

namespace cards {

class Db : public Pkg
{
public:
	Db();
	~Db();
	
	time_t install();
	unsigned int space();
	bool dependency();
	std::set<std::string> files();

	void install(time_t time);
	void space(unsigned int space); 
	void dependency(bool dependency);
	void files(std::set<std::string> files);

private:
	unsigned int m_space;      // disk occupation onces installed
	time_t       m_install;    // date of last installation
	bool         m_dependency; // If TRUE it's automatically installed, it's a dep
	std::set<std::string> m_files;
};

} //end of 'cards' namespace

#endif
