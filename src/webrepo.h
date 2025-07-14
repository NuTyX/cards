/* SPDX-License-Identifier: LGPL-2.1-or-later  */
#pragma once

#include "file_utils.h"
#include "conf.h"
#include "pkg.h"

namespace cards {

typedef std::map<std::string, cards::pkg> webrepo_t;

class webrepo
{
        cards::conf                m_config;
        std::string                m_configFileName;
        webrepo_t                  m_listOfPackages;
        void parse();

    public:
        webrepo(const std::string& configFileName);
        const webrepo_t& getListOfPackages();
};
}
