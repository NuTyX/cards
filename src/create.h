/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "cards_argument_parser.h"
#include "enum.h"
#include "error_treat.h"
#include "pkgfile.h"
#include "process.h"

namespace cards {

class create : public pkgfile {
    const CardsArgumentParser& m_argParser;
    cards::ErrorEnum m_actualError;
    ports_t m_tree;

    void treatErrors(const std::string& s) const;
    void list(std::string& packageName);
    void parseArguments();

public:
    create(CardsArgumentParser& argParser);
    ~create() {};
    void build(std::string name);
};
}
