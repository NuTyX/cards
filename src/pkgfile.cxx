/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "pkgfile.h"

namespace cards {

std::set<std::string> port::dependencies()
{
    return m_dependencies;
}
void port::dependencies(const std::set<std::string> dependencies)
{
    m_dependencies = dependencies;
}
pkgfile::pkgfile(const std::string& fileName)
    : m_configFileName(fileName)
{
}
void pkgfile::parsePackagePkgfileFile()
{
    if (m_listOfPackages.size() > 0)
        return;
    cards::conf m_config(m_configFileName);
    cards::port info;
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
                    }
                    else if (line.substr(0, 5) == "run=(") {
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
                            s += line.substr(0,pos);
                            s = s + " ";
                            found = false;
                        } else
                            s = s + line + " ";
                    }
                }
                info.dependencies(parseDelimitedSetList(s," "));
                m_listOfPackages[portName] = info;
            }
        }
    }
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
bool pkgfile::checkPortExist(const std::string& portName)
{
    if (m_listOfPackages.size() == 0)
        parsePackagePkgfileFile();
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
