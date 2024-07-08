/* SPDX-License-Identifier: LGPL-2.1-or-later  */
#include "cards_level.h"

namespace cards {

cards_level::cards_level()
    : pkgfile("/etc/cards.conf")
{
}
cards_level::~cards_level()
{
}
void cards_level::level()
{
    if (m_listOfPackages.size() == 0)
        parsePackagePkgfileFile();
    unsigned int level = 0;
    generate_level(level);
}
void cards_level::generate_level(unsigned int& level)
{
    bool found = false;
    for (auto& i : m_listOfPackages) {
        // decount == 0 means no dependencies anymore
        if (i.second.decount() == 0) {
            found = true;
            // at next loop we don't need to look at it
            // decount = -1
            i.second.decount(-1);
            // Set the package level at current level value
            i.second.level(level);
            // For all the packages
            // that still have some deps
            // that need to be check
            for (auto& j : m_listOfPackages) {
                // Still some deps that need to be check
                if (j.second.decount() > 0) {
                    // for all the deps of the package 'j''
                    for (auto k : j.second.indexlevel()) {
                        // if the package 'i' is found in the deps List
                        if (k.index == i.second.index()) {
                            // we increment the 'decrement' variable
                            j.second.decrement(j.second.decrement() + 1);
                        }
                    }
                }
            }
        }
    }
    for (auto& i : m_listOfPackages) {
        if (i.second.decrement() > 0) {
            i.second.decount(i.second.decount() - i.second.decrement());
            i.second.decrement(0);
        }
    }
    if (found) {
        level++;
        generate_level(level);
    }
}
void cards_level::printout()
{
    level();

    unsigned int level = 0;
    bool found = false;
    while (true) {
        for (auto i : m_listOfPackages)
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
            return;
        ++level;
        found = false;
    }
}

} // endof cards namespace
