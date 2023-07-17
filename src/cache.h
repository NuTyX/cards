#ifndef CACHE_H
#define CACHE_H

#include "enum.h"
#include "db.h"

namespace cards {

class Cache: public Db
{
public:
	Cache();
	~Cache();
	unsigned int size();
	StatusEnum status();
	bool installed();
	const std::string name();

	void size(unsigned int size);
	void status(StatusEnum status);
	void installed(bool installed);
	void name(const std::string& name);

private:
	StatusEnum m_status;
	unsigned int m_size;
	bool m_installed;
	std::string m_name;

};

} // endof cards namespace
#endif
