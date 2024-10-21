/* SPDX-License-Identifier: LGPL-2.1-or-later  */
#pragma once
#include "cards_argument_parser.h"
#include "pkgfile.h"

namespace cards {
class level : public pkgfile
{
    ports_t                    m_tree;

public:
    level()
    : pkgfile("/etc/cards.conf")
    , m_tree(getListOfPackages()) {};

    ~level() {};
    void print();
};

}  // endof cards namespace
