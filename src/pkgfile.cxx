/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "pkgfile.h"

namespace cards {

pkgfile::pkgfile(const std::string& fileName)
    : m_level(0)
    , m_configFileName(fileName)
    , m_config(fileName)
{
}
bool pkgfile::checkCollectionNameExist(const std::string& collectionName)
{
    DIR* d;
    struct dirent* dir;
    d = opendir(collectionName.c_str());
    if (d)
        return true;

    return false;
}
void pkgfile::confirmDependencies()
{
    for (auto& i : m_listOfPackages) {
        std::vector<unsigned int> indexes;
        unsigned int n = 1;
        for (auto j : i.second.dependencies()) {
            std::string s = getValueBeforeLast(j, '.');
            if (m_listOfPackages.find(s) != m_listOfPackages.end()) {
                if (i.second.index() == m_listOfPackages[s].index())
                    continue;

                indexes.push_back(m_listOfPackages[s].index());
                i.second.decount(n);
                ++n;
            } else {
                std::string badDependency = "WARNING ";
                badDependency += s;
                badDependency += " from ";
                badDependency += i.first;
                badDependency += " (";
                badDependency += i.second.collection();
                badDependency += ") ";
                badDependency += " NOT FOUND ...";
                m_badDependencies.push_back(badDependency);
            }
        }
        i.second.indexes(indexes);
    }
}
void pkgfile::parse()
{
    if (m_listOfPackages.size() > 0)
        return;

    cards::port info;
    unsigned int index = 0;
    bool found = false;

    for (auto i : m_config.dirUrl()) {
        DIR* d;
        struct dirent* dir;
        d = opendir(i.collection.c_str());
        if (d) {
            while ((dir = readdir(d)) != nullptr) {
                if (dir->d_name[0] == '.')
                    continue;

                std::string portName(dir->d_name);
                std::string Pkgfile = i.collection + "/" + portName + PKG_RECEPT;
                if (!checkFileExist(Pkgfile))
                    continue;

                info.collection(i.collection);
                info.release(1);
                std::vector<std::string> pkgFileContent;
                if (parseFile(pkgFileContent, Pkgfile.c_str()) != 0) {
                    std::cerr << "Cannot read the file: " << Pkgfile
                              << std::endl
                              << "... continue with next" << std::endl;
                    m_listOfPackages[portName] = info;
                    continue;
                }
                std::string::size_type pos;
                std::string s;
                for (auto p : pkgFileContent) {
                    std::string line = stripWhiteSpace(p);
                    replaceAll(line, ",", " ");
                    replaceAll(line, "\\", " ");
                    if (line[0] == '#')
                        continue;
                    if (line.substr(0, 12) == "description=")
                        info.description(stripWhiteSpace(getValueBefore(getValue(line, '='), '#')));
                    else if (line.substr(0, 4) == "url=")
                        info.url(stripWhiteSpace(getValueBefore(getValue(line, '='), '#')));
                    else if (line.substr(0, 8) == "version=")
                        info.version(stripWhiteSpace(getValueBefore(getValue(line, '='), '#')));
                    else if (line.substr(0, 8) == "release=")
                        info.release(atoi((stripWhiteSpace(getValueBefore(getValue(line, '='), '#'))).c_str()));
                    else if (line.substr(0, 9) == "packager=")
                        info.packager(stripWhiteSpace(getValueBefore(getValue(line, '='), '#')));
                    else if (line.substr(0, 13) == "contributors=")
                        info.contributors(stripWhiteSpace(getValueBefore(getValue(line, '='), '#')));
                    else if (line.substr(0, 11) == "maintainer=")
                        info.maintainer(stripWhiteSpace(getValueBefore(getValue(line, '='), '#')));
                    else if (line.substr(0, 13) == "makedepends=(") {
                        s += line.substr(13);
                        found = true;
                        pos = s.find(')');
                        if (pos != std::string::npos) {
                            s[pos] = ' ';
                            found = false;
                        } else
                            s = s + " ";
                        continue;
                    } else if (line.substr(0, 5) == "run=(") {
                        s += line.substr(5);
                        found = true;
                        pos = s.find(')');
                        if (pos != std::string::npos) {
                            s[pos] = ' ';
                            found = false;
                        } else
                            s = s + " ";
                        continue;
                    }
                    if (found) {
                        pos = line.find(')');
                        if (pos != std::string::npos) {
                            s += line.substr(0, pos);
                            s = s + " ";
                            found = false;
                        } else
                            s = s + line + " ";
                    }
                }
                info.dependencies(parseDelimitedSetList(s, " "));
                info.index(index);
                m_listOfPackages[portName] = info;
                ++index;
            }
        }
    }
    confirmDependencies();
    generate_level();
}
void pkgfile::generate_level()
{
    bool found = false;
    for (auto& i : m_listOfPackages) {
        // decount == 0 means no more dependencies needs
        // to be treated.
        if (i.second.decount() == 0) {
            found = true;
            // at next loop we don't need to look at it
            // decount = -1
            i.second.decount(-1);
            // Set the package level at current level value
            i.second.level(m_level);
            // For all the packages
            // that still have some deps
            // that need to be check
            for (auto& j : m_listOfPackages) {
                // if both packages in this loop and
                // the upper loop are same, ignore it
                if (j.second.index() == i.second.index())
                    continue;

                // Still some deps that need to be check
                if (j.second.decount() > 0) {
                    // for all the deps of the package 'j''
                    for (auto k : j.second.indexes()) {
                        // if the depname is the same as the name of the
                        // package we check the deps, ignore it
                        if (k == j.second.index())
                            continue;

                        // if the package 'i' is found in the deps List
                        if (k == i.second.index()) {
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
        m_level++;
        generate_level();
    }
}
ports_t& pkgfile::getListOfPackages()
{
    if (m_listOfPackages.size() == 0)
        parse();

    return m_listOfPackages;
}
std::vector<std::string>& pkgfile::badDependencies()
{
    if (m_listOfPackages.size() == 0)
        parse();

    return m_badDependencies;
}
std::set<std::string> pkgfile::getDependencies(const std::string& portName)
{
    std::set<std::string> deps;
    if (m_listOfPackages.size() == 0)
        parse();

    if (checkPackageNameExist(portName))
        return m_listOfPackages[portName].dependencies();

    return deps;
}
bool pkgfile::checkPackageNameExist(const std::string& name)
{
    if (m_listOfPackages.size() == 0)
        parse();

    return (m_listOfPackages.find(name) != m_listOfPackages.end());
}
std::string pkgfile::pkgfile::getPortVersion(const std::string& portName)
{
    if (m_listOfPackages.size() == 0)
        parse();

    return m_listOfPackages[portName].version();
}
std::string pkgfile::pkgfile::getPortDir(const std::string& portName)
{
    if (m_listOfPackages.size() == 0)
        parse();

    return m_listOfPackages[portName].collection() + "/" + portName;
}
unsigned short int pkgfile::pkgfile::getPortRelease(const std::string& portName)
{
    if (m_listOfPackages.size() == 0)
        parse();

    return m_listOfPackages[portName].release();
}
int pkgfile::pkgfile::getLevel(const std::string& portName)
{
    if (m_listOfPackages.size() == 0)
        parse();

    return m_listOfPackages[portName].level();
}
std::vector<std::string>& pkgfile::pkgfile::getListOfPackagesFromCollection(std::string collectionName)
{
    if (m_listOfPackages.size() == 0)
        parse();

    unsigned int level, levelMax;
    level = 0;
    levelMax = 0;

    bool found = false;
    while (true) {
        for (auto i : m_listOfPackages)
            if (i.second.level() == level)
                found = true;
        if (!found)
            break;

        ++level;
        found = false;
    }
    levelMax = level;
    level = 0;
    while (level <= levelMax) {
        for (auto i : m_listOfPackages) {
            if ((i.second.collection() == collectionName) &&
                (i.second.level() == level)) {
                m_listOfPackagesFromCollection.push_back(i.first);
            }
        }
        level++;
    }
    return m_listOfPackagesFromCollection;
}

} // end of 'cards' namespace
