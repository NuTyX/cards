/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "pkgfile.h"

namespace cards {

class deptree : public pkgfile {
    ports_t m_tree;
    std::vector<std::string> m_list;
    std::string m_space;
public:
    deptree()
        : pkgfile("/etc/cards.conf")
        , m_tree(getListOfPackages())
        , m_space("") {};
    ~deptree() {};
    void printDependencies(std::string name, std::string space, int level);
};

}
