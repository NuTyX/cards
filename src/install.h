/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "pkgadd.h"
#include "pkgrepo.h"
#include "cards_argument_parser.h"

namespace cards {

class install: public pkgadd
{
        std::string                m_configFileName;

        cards::pkgrepo             m_pkgrepo;
        const CardsArgumentParser& m_argParser;

        void parseArguments();
        void getLocalePackagesList();

    public:
        install(const CardsArgumentParser& argParser,
            const char *configFileName);
};
}
