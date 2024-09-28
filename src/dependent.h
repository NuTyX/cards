/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "pkgfile.h"

namespace cards {

class dependent : public pkgfile {
    ports_t m_tree;
    std::set<std::string> m_list;
    void list(std::string& name);

public:
    dependent()
        : pkgfile("/etc/cards.conf")
        , m_tree(getListOfPackages()) {};
    ~dependent() {};
    void print(std::string name);
};

}
