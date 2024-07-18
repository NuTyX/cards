/* SPDX-License-Identifier: LGPF-2.1-or-later */
#include "depends.h"

namespace cards {
void depends::list(std::string& name)
{
    auto found = false;
    for (auto i : m_list) {
        if (name == i) {
            found = true;
        }
    }
    if (!found) {
        m_list.insert(name);
        if (m_tree[name].dependencies().size() > 0) {
            for (auto n : m_tree[name].dependencies()) {
                for (auto i : m_list)
                    if (n == name)
                        break;
                list(n);
            }
        }
    }
}
void depends::print(std::string name)
{
    list(name);
    auto level = m_tree[name].level();
    unsigned int currentLevel = 0;
    while (currentLevel < level) {
        for (auto i : m_list) {
            if (m_tree[i].level() == currentLevel)
                std::cout << currentLevel
                          << ": "
                          << i
                          << std::endl;
        }
        ++currentLevel;
    }
    std::cout << level
              << ": "
              << name
              << std::endl;
}
}
