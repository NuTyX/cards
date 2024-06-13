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
    for (auto i : m_config.dirUrl()) {
        DIR* d;
        struct dirent* dir;
        d = opendir(i.dir.c_str());
        if (d) {
            while ((dir = readdir(d)) != NULL) {
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
                for (auto p : pkgFileContent) {
                    std::string line = stripWhiteSpace(p);
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
                    m_listOfPackages[portName] = info;
                }
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
