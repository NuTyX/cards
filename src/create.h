/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "cards_argument_parser.h"
#include "enum.h"
#include "error_treat.h"
#include "pkgfile.h"
#include "process.h"
#include "pkgadd.h"

namespace cards {

class create : public pkgfile {
    ports_t m_tree;
    std::set<std::string> m_list;
    std::vector<std::string> m_dependencies;
    const CardsArgumentParser& m_argParser;
    cards::ErrorEnum m_actualError;
    cards::conf m_config;

    void treatErrors(const std::string& s) const;
    void list(std::string& packageName);
    void installDependencies(std::string& packageName);
    void parseArguments();

public:
    create(CardsArgumentParser& argParser);
    ~create() {};
    void build(std::string name);
};
}
