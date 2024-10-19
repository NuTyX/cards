/* SPDX-License-Identifier: LGPL-2.1-or-later  */
#pragma once
#include "cards_argument_parser.h"
#include "pkgfile.h"

namespace cards {
class level : public pkgfile
{
    std::vector<std::string>   m_listOfPackages;
    ports_t                    m_tree;

public:
    level()
    : pkgfile("/etc/cards.conf")
    , m_tree(getListOfPackages()) {};

    ~level() {};
    void print();
    std::vector<std::string>& getListOfPackagesFromCollection(std::string collectionName);
};

}  // endof cards namespace
