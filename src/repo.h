/* SPDX-License-Identifier: LGPL-2.1-or-later  */
#pragma once

#include "cards_argument_parser.h"
#include "pkgrepo.h"

namespace cards {

class repo
{
        std::string                m_configFileName;

        cards::pkgrepo             m_pkgrepo;
        const CardsArgumentParser& m_argParser;

    public:
        repo(const CardsArgumentParser& argParser,
            const char *configFileName);
};
}