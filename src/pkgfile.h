/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "conf.h"
#include "file_utils.h"
#include "pkg.h"

namespace cards {

class port : public pkg {

    // The index Name used to check
    // if dependencies is valid
    unsigned int    m_index;

    // The level of dependency:
    // 0: No dependency
    // 1: some deps from level 0 dependencies
    // 2: some deps from level 1 dependencies
    // 3: ... 2  ...
    // etc
    int             m_level;

    // number of removed dependencies
    // when searching them
    unsigned int m_decrement;

    // number of dependencies that are still
    // need to be treated
    int             m_decount;

    // Dependencies made of index Name
    // This list validate all the dependencies
    // as they are pointing the actual name via
    // they index
    std::vector<unsigned int>
                    m_indexes;

    std::set<std::string>
                    m_dependencies;

public:
    port()
        : m_level(0)
        , m_decount(0)
        , m_decrement(0)
    {
    }
    int level()
    {
        return m_level;
    }
    void level(int level)
    {
        m_level = level;
    }
    int decount()
    {
        return m_decount;
    }
    void decount(int decount)
    {
        m_decount = decount;
    }
    unsigned int index()
    {
        return m_index;
    }
    void index(unsigned int index)
    {
        m_index = index;
    }
    unsigned int decrement()
    {
        return m_decrement;
    }
    void decrement(unsigned int decrement)
    {
        m_decrement = decrement;
    }
    std::vector<unsigned int> indexes()
    {
        return m_indexes;
    }
    void indexes(std::vector<unsigned int>& indexes)
    {
        m_indexes = indexes;
    }
    std::set<std::string> dependencies()
    {
        return m_dependencies;
    }
    void dependencies(const std::set<std::string>& dependencies)
    {
        m_dependencies = dependencies;
    }
};

typedef std::map<std::string, cards::port> ports_t;

class pkgfile {

    std::string                 m_configFileName;
    cards::conf                 m_config;
    ports_t                     m_listOfPackages;
    std::vector<std::string>    m_listOfPackagesFromCollection;
    std::vector<std::string>    m_badDependencies;
    unsigned int                m_level;

    void                        parse();
    void                        generate_level();
    std::string                 getVariable(const std::string& file,
        const std::string& name);

public:
    pkgfile(const std::string& fileName);
    virtual ~pkgfile() { }

    /**
     * Get the level of the portName
     *
     */
    int                        getLevel(const std::string& portName);

    /**
     * check that all the dependencies exist in each package
     *
     */
    void                        confirmDependencies();


    std::vector<std::string>&   getListOfPackagesFromCollection(std::string collectionName);

    /**
     * return the set of dependencies of the port name
     *
     */
    std::set<std::string>       getDependencies(const std::string& portName);

    /**
     * return the version of the port name
     *
     */
    std::string                getPortVersion(const std::string& portName);

    /**
     * return the folder of the port name
     *
     */
    std::string                getPortDir(const std::string& portName);

    /**
     * return the release of the port name
     *
     */
    unsigned short int          getPortRelease(const std::string& portName);

    /**
     * return true if port name exist
     *
     */
    bool                        checkPackageNameExist(const std::string& name);

    /**
     * return true if it's collection
     *
     */
    bool                        checkCollectionNameExist(const std::string& name);
    /**
     * return the list of found Packages
     *
     */
    ports_t&                    getListOfPackages();

    /**
     * return the list of bad Dependencies
     *
     */
    std::vector<std::string>&   badDependencies();
};

} // end of 'cards' namespace
