/* SPDX-License-Identifier: LGPL-2.1-or-later */
//  Copyright (c) 2026 - .... by NuTyX team (https://nutyx.org)

#pragma once

#include "pkginfo.h"
#include "conf.h"
#include "cards_argument_parser.h"


namespace cards {

class files{
    const CardsArgumentParser& m_argParser;
    std::string                m_configFileName;
    void parse();

    public:
        /**
         * Default constructor
         * 
         * \param argParser CardsArgumentParser type
         * \param configFileName Specify the configuration file name to use
         */
        files(const CardsArgumentParser& argParser, const std::string& configFileName);

};

} // cards namespace