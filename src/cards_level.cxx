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
    static unsigned int Niveau = level;
    generate_level(Niveau);
}
void cards_level::generate_level(unsigned int& level)
{
    bool found = false;
    for (auto &i : m_listOfPackages) {
        if (i.second.decount() == 0) {
            found = true;
            i.second.decount(-1);
            i.second.level(level);
            for (auto j : m_listOfPackages) {
                if (j.second.decount() > 0) {
                    for (auto k : j.second.indexlevel()) {
                        if (k.index == i.second.index())
                            i.second.decrement(i.second.decrement() + 1);
                    }
                }
            }
        }
    }
    for (auto &i : m_listOfPackages) {
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
    std::cout << m_listOfPackages.size()
              << " found packages."
              << std::endl;
}

}  // endof cards namespace
