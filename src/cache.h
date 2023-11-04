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
	bool toinstall();
	bool toremove();
	const std::string name();
	const std::string fileDate();
	const std::string dirName();

	void size(unsigned int size);
	void setStatus(StatusEnum status);
	void unsetStatus(StatusEnum status);
	void installed(bool installed);
	void name(const std::string& name);
	void fileDate(const std::string& filedate);
	void dirName(const std::string& dirname);


private:
	StatusEnum m_status;
	unsigned int m_size;
	bool m_installed;
	std::string m_name;
	std::string m_fileDate;
	std::string m_dirName;

};

} // endof cards namespace
#endif /* CACHE_H */
