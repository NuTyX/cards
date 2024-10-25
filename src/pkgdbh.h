/* SPDX-License-Identifier: LGPL-2.1-or-later */
//  Copyright (c) 2000 - 2005 Per Liden
//  Copyright (c) 2006 - 2013 by CRUX team (https://crux.nu)
//  Copyright (c) 2013 - .... by NuTyX team (https://nutyx.org)

#pragma once

#include "archive.h"
#include "db.h"
#include "file_utils.h"
#include "lock.h"
#include "process.h"

#include <algorithm>

#include <ext/stdio_filebuf.h>
#include <grp.h>
#include <pwd.h>
#include <regex.h>

typedef std::map<std::string, cards::db> packages_t;

struct rule_t {
    cards::rule_event_t event;
    std::string pattern;
    bool action;
};

class pkgdbh {

    std::set<std::string>   m_filesList;
    std::set<std::string>   m_listOfPackagesNames;

    unsigned int            m_filesNumber;
    unsigned int            m_installedFilesNumber;

    bool                    m_dependency;
    bool                    m_DB_Empty;

    void                    runLastPostInstall();
    std::string             getSingleItem(const std::string& PackageName, const char i) const;

    std::set<std::string>   getSetOfItems(const std::string& PackageName, const char i) const;

protected:
    // Database

    int                     getListOfPackagesNames(const std::string& path);
    std::pair<std::string, cards::db>
                            getInfosPackage(const std::string& packageName);

    void                    buildSimpleDependenciesDatabase();

    void                    addPackageFilesRefsToDB(const std::string& name,
                                const cards::db& info);

    /**
     * Remove the physical files after followings some rules
     */
    void                    removePackageFiles(const std::string& name);
    void                    removePackageFiles(const std::string& name,
                                const std::set<std::string>& keep_list);

    /**
     * Remove meta data about the removed package
     */
    void                    removePackageFilesRefsFromDB(const std::string& name);
    void                    removePackageFilesRefsFromDB(std::set<std::string> files,
                                const std::set<std::string>& keep_list);

    std::set<std::string>   getConflictsFilesList(const std::string& name, cards::db& info);

    // Archive Package handling
    std::pair<std::string, cards::db>
                            openArchivePackage(const std::string& filename);
    void                    extractAndRunPREfromPackage(const std::string& filename);
    void                    installArchivePackage(const std::string& filename,
                                const std::set<std::string>& keep_list,
                                const std::set<std::string>& non_install_files);

    /**
     * The folder holding the meta datas is going to be create here
     */
    void                    moveMetaFilesPackage(const std::string& name, cards::db& info);

    void                    readRulesFile();
    void                    getInstallRulesList(const std::vector<rule_t>& rules,
                                cards::rule_event_t event, std::vector<rule_t>& found) const;

    bool                    checkRuleAppliesToFile(const rule_t& rule,
                                const std::string& file);

    void                    getFootprintPackage(std::string& filename);

    std::string             m_packageName;
    std::string             m_packageArchiveVersion;
    std::string             m_packageArchiveRelease;
    std::string             m_packageArchiveCollection;
    std::string             m_packageVersion;
    std::string             m_packageRelease;
    std::string             m_packageCollection;
    std::string             m_utilName;
    std::string             m_root;
    std::vector<rule_t>     m_actionRules;
    std::set<std::pair<std::string, int>>
                            m_postInstallList;
    alias_t                 m_listOfAlias;

    packages_t              m_listOfPackages;

    std::set<std::pair<std::string, std::set<std::string>>>
                            m_listOfPackagesWithDeps;

    cards::ActionEnum       m_actualAction;
    cards::ErrorEnum        m_actualError;

public:
    /**
     * Constructors
    */
    explicit                pkgdbh(const std::string& name);
    explicit                pkgdbh();

    /**
     * Destructor
    */
    virtual                 ~pkgdbh();

    /**
     *  Following methods can be redefined in derivated class
     *
     */
    virtual void            parseArguments(int argc, char** argv);
    virtual void            run(int argc, char** argv);
    virtual void            run() {};
    virtual void            printHelp() const {};
    virtual void            progressInfo();
    virtual void            progressInfo(cards::ActionEnum action);
    virtual void            treatErrors(const std::string& s) const;

    void                    print_version() const;
    void                    buildDatabase(const bool& progress, const bool& files);
    void                    setDependency();
    void                    resetDependency();

    std::set<std::string>&  getListOfPackagesNames();
    std::string             getDescription(const std::string& name) const;
    std::string             getVersion(const std::string& name) const;
    std::string             getCollection(const std::string& name) const;
    std::string             getSets(const std::string& name) const;
    std::set<std::string>   getSetOfSets(const std::string& name) const;
    std::string             getArch(const std::string& name) const;
    std::string             getLicense(const std::string& name) const;
    std::string             getCategories(const std::string& name) const;
    std::set<std::string>   getSetOfCategories(const std::string& name) const;
    std::set<std::string>   getSetOfFiles(const std::string& packageName);
    std::set<std::string>   getFilesList();

    unsigned int            getFilesNumber();
    unsigned int            getInstalledFilesNumber();
    unsigned int            getSize(const std::string& name) const;
    unsigned int            getRelease(const std::string& name) const;

    bool                    checkPackageNameUptodate(archive& archiveName);
    bool                    checkPackageNameUptodate(std::pair<std::string, cards::db>& archiveName);
    bool                    checkPackageNameBuildDateSame(const std::pair<std::string,time_t>& dependencieNameBuild);
    bool                    checkPackageNameExist(const std::string& name) const;
    bool                    checkDependency(const std::string& name);

};
