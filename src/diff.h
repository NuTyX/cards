/* SPDX-License-Identifier: LGPL-2.1-or-later  */
#pragma once

#include "pkgdbh.h"
#include "sync.h"
#include "pkgrepo.h"
#include "cards_argument_parser.h"

namespace cards {

class diff : public pkgdbh
{
    std::string                m_configFileName;
    std::map<std::string, m_package>
                               m_listOfPackagesToDeal;
    cards::pkgrepo             m_pkgrepo;
    cards::sync                m_sync;
    cards::conf                m_config;
    unsigned int               m_packagesConflict;
    unsigned int               m_packagesReplace;
    unsigned int               m_packagesObsolet;
    unsigned int               m_packagesNewBuild;
    unsigned int               m_packagesOutOfDate;
    unsigned int               m_packagesOK;
    
    

    const CardsArgumentParser& m_argParser;

    
    public:
        diff(const CardsArgumentParser& argParser,
            const std::string& configFileName);

        void showInfo();
        void summary();

        const unsigned int packagesConflict();
        const unsigned int packagesReplace();
        const unsigned int packagesObsolet();
        const unsigned int packagesNewBuild();
        const unsigned int packagesOutOfDate();
        const unsigned int packagesOK();
        const unsigned int ratio();
        const unsigned int size();
};

}