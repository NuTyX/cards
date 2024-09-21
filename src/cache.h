/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "enum.h"
#include "db.h"

namespace cards {

class cache: public db
{
	StatusEnum			m_status;
	unsigned 			int m_size;
	bool				m_installed;
	std::string			m_name;
	std::string			m_baseName;
	std::string			m_fileName;
	std::string			m_fileDate;
	std::string			m_dirName;

public:
	cache();
	~cache();
	unsigned int		size();
	StatusEnum			status();
	bool				installed();
	bool				toinstall();
	bool				toremove();
	const std::string&	name();
	const std::string&	baseName();
	const std::string&	fileName();
	const std::string&	fileDate();
	const std::string&	dirName();

	void				size(unsigned int size);
	void				setStatus(StatusEnum status);
	void				unsetStatus(StatusEnum status);
	void				installed(bool installed);
	void				name(const std::string& name);
	void				baseName(const std::string& baseName);
	void				fileName(const std::string& fileName);
	void				fileDate(const std::string& filedate);
	void				dirName(const std::string& dirname);


};

} // endof cards namespace
