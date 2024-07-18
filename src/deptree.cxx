#include "deptree.h"

namespace cards {
void deptree::print(std::string name, std::string space, int level)
{
    bool found = false;
    for (auto i : m_list) {
        if (name == i) {
            found = true;
            if (m_tree[i].dependencies().size() > 0)
                std::cout << space << level << ") " << name << " -->" << std::endl;
            else
                std::cout << space << level << ") " << name << std::endl;
        }
    }
    if (!found) {
        m_list.push_back(name);
        std::cout << space << level << ") " << name << std::endl;
        space = space + "  ";
        ++level;
        if (m_tree[name].dependencies().size() > 0) {
            for (auto n : m_tree[name].dependencies()) {
                if (n == name)
                    break;
                print(n, space, level);
            }
        }
    }
}
}
