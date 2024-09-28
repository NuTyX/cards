/* SPDX-License-Identifier: LGPF-2.1-or-later */
#include "dependent.h"

namespace cards {
void dependent::list(std::string& name)
{
    for (auto i : m_tree) {
        for (auto d : i.second.dependencies()) {
            if (d == name)
                m_list.insert("("
                    + i.second.collection()
                    + ") "
                    + i.first
                    + " "
                    + i.second.version()
                    + "-"
                    + itos(i.second.release()));
        }
    }
}
void dependent::print(std::string name)
{
    list(name);

    for (auto name : m_list)
        std::cout << name
              << std::endl;
}
}
