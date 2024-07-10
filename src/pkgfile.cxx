/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "pkgfile.h"

namespace cards {

pkgfile::pkgfile(const std::string& fileName)
    : m_configFileName(fileName)
{
}
void pkgfile::confirmDependencies()
{
    for (auto& i : m_listOfPackages) {
        std::vector<IndexLevel> indexlevelList;
        IndexLevel indexlevel;
        unsigned int n = 1;
        for (auto j : i.second.dependencies()) {
            std::string s = getValueBeforeLast(j,'.');
            if (m_listOfPackages.find(s) != m_listOfPackages.end()) {
                if (i.second.index() == m_listOfPackages[s].index())
                    continue;

                indexlevel.index = m_listOfPackages[s].index();
                indexlevel.level = 0;
                indexlevelList.push_back(indexlevel);
                i.second.decount(n);
                ++n;
            } else {
                std::string badDependency = s ;
                badDependency += " from ";
                badDependency += i.first;
                m_badDependencies.push_back(badDependency);
            }
        }
        i.second.indexlevel(indexlevelList);
    }
}
void pkgfile::parsePackagePkgfileFile()
{
    if (m_listOfPackages.size() > 0)
        return;
    cards::conf m_config(m_configFileName);
    cards::port info;
    unsigned int index = 0;
    bool found;
    for (auto i : m_config.dirUrl()) {
        DIR* d;
        struct dirent* dir;
        d = opendir(i.dir.c_str());
        if (d) {
            while ((dir = readdir(d)) != nullptr) {
                if (dir->d_name[0] == '.')
                    continue;

                std::string portName(dir->d_name);
                std::string Pkgfile = i.dir + "/" + portName + PKG_RECEPT;
                if (!checkFileExist(Pkgfile))
                    continue;

                info.collection(i.dir);
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
}
ports_t pkgfile::getListOfPackages()
{
    if (m_listOfPackages.size() == 0)
        parsePackagePkgfileFile();
    return m_listOfPackages;
}
std::set<std::string> pkgfile::getDependencies(const std::string& portName)
{
    std::set<std::string> deps;
    if (m_listOfPackages.size() == 0)
        parsePackagePkgfileFile();

    if (checkPortExist(portName))
        return m_listOfPackages[portName].dependencies();

    return deps;
}
bool pkgfile::checkPortExist(const std::string& portName) const
{
    return (m_listOfPackages.find(portName) != m_listOfPackages.end());
}
std::string pkgfile::pkgfile::getPortVersion(const std::string& portName)
{
    if (m_listOfPackages.size() == 0)
        parsePackagePkgfileFile();
    return m_listOfPackages[portName].version();
}
unsigned short int pkgfile::pkgfile::getPortRelease(const std::string& portName)
{
    if (m_listOfPackages.size() == 0)
        parsePackagePkgfileFile();
    return m_listOfPackages[portName].release();
}
} // end of 'cards' namespace
