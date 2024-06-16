#pragma once

#include "conf.h"
#include "file_utils.h"
#include "pkg.h"

namespace cards {

class port : public pkg {
    std::set<std::string> m_dependencies;

public:
    std::set<std::string> dependencies();
    void dependencies(const std::set<std::string> dependencies);
};

typedef std::map<std::string, cards::port> ports_t;

class pkgfile {

    std::string m_configFileName;
    cards::conf m_config;
    ports_t m_listOfPackages;

    /**
     * parse the "Pkgfile" file for each Package
     *
     */
    void parsePackagePkgfileFile();

public:
    pkgfile(const std::string& fileName);
    virtual ~pkgfile() { }

    /**
     * return the set of dependencies of the port name
     *
     */
     std::set<std::string> getDependencies(const std::string& portName);

    /**
     * return the version of the port name
     *
     */
    std::string getPortVersion(const std::string& portName);

    /**
     * return the release of the port name
     *
     */
    unsigned short int getPortRelease(const std::string& portName);

    /**
     * return true if port name exist
     *
     */
    bool checkPortExist(const std::string& portName);

    /**
     * return the list of found Packages
     *
     */
    ports_t getListOfPackages();
};

} // end of 'cards' namespace
