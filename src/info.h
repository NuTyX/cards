/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "pkginfo.h"
#include "pkgrepo.h"
#include "cards_argument_parser.h"

namespace cards {
/**
 * \brief Cards info class
 *
 * This class takes care of printing out information about availables packages.
 *
 */

class info
{
        const CardsArgumentParser& m_argParser;
        cards::pkgrepo             m_pkgrepo;

    public:
        /**
         * Default constructor
         *
         * \param argParser CardsArgumentParser type
         * \param configFileName Specify the configuration file name to use
         */
        info(const CardsArgumentParser& argParser,const std::string& configFileName);
};


}
