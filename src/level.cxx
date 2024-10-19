/* SPDX-License-Identifier: LGPL-2.1-or-later  */
#include "level.h"

namespace cards {

void level::print()
{
    unsigned int level = 0;
    bool found = false;
    while (true) {
        for (auto i : m_tree)
            if (i.second.level() == level) {
                std::cout << level
                          << ": "
                          << i.second.collection()
                          << "/"
                          << i.first
                          << std::endl;
                found = true;
            }
        if (!found)
            break;
        ++level;
        found = false;
    }
    for (auto i : badDependencies())
        std::cout << i
                  << std::endl;
}
std::vector<std::string>& level::getListOfPackagesFromCollection(std::string collectionName)
{
    for (auto i : m_tree) {
        if (i.second.collection() == collectionName)
            m_listOfPackages.push_back(i.first);
    }
    return m_listOfPackages;
}
} // endof cards namespace
