/* SPDX-License-Identifier: LGPL-2.1-or-later */
//  Copyright (c) 2026 - .... by NuTyX team (https://nutyx.org)

#pragma once

#include "pkginfo.h"
#include "conf.h"
#include "cards_argument_parser.h"

#include <sys/mman.h>

namespace cards {

class search {
	struct package {
		std::string name;
		std::string version;
		std::string release;
		std::string collection;
		std::string description;
		std::string url;
		std::string packager;
		std::string group;
	};

    const CardsArgumentParser& m_argParser;
	std::vector<package>       m_listOfPackages;
	std::string                m_configFileName;
	void parse();
	void list();


    public:
        /**
         * Default constructor
         * 
         * \param argParser CardsArgumentParser type
         * \param configFileName Specify the configuration file name to use
         */
        search(const CardsArgumentParser& argParser, const std::string& configFileName);

};

} // cards namespace