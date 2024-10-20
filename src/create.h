/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "cards_argument_parser.h"
#include "enum.h"
#include "pkgfile.h"
#include "level.h"
#include "process.h"
#include "pkgadd.h"
#include "pkgrepo.h"

namespace cards {

class create : public pkgadd {
    int                        m_fdlog;
    std::string                m_portsDir;
    ports_t                    m_tree;
    std::set<std::string>      m_list;
    std::vector<std::string>   m_dependencies;
    const CardsArgumentParser& m_argParser;
    cards::ErrorEnum           m_actualError;
    cards::conf                m_config;
    cards::pkgfile             m_pkgfile;

    bool isACollection();

    void treatErrors(const std::string& s) const;
    void list(std::string& packageName);
    void installDependencies(std::string& packageName);
    void parseArguments();
    void checkBinaries();
    void buildCollection();
    void base();
    void buildBinary(std::string packageName);

public:
    create(CardsArgumentParser& argParser);
    ~create();
};
}
