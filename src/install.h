/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "pkgrepo.h"
#include "cards_argument_parser.h"

namespace cards {

class install: public pkgrepo
{
        const CardsArgumentParser& m_argParser;
        void parseArguments();
        void getLocalePackagesList();
        std::string m_configFileName;

    public:
        install(const CardsArgumentParser& argParser,
            const char *configFileName);
};
}
