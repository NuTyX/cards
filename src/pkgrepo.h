/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "cache.h"
#include "conf.h"
#include "dwl.h"
#include "pkgdbh.h"

#include <iomanip>

#include <openssl/pem.h>
#include <openssl/err.h>

namespace cards {

typedef std::map<std::string, cards::cache> repo_t;

class pkgrepo {

    std::string              m_packageName;
    std::string              m_configFileName;
    cards::ErrorEnum         m_actualError;
    cards::conf              m_config;
    repo_t                   m_listOfPackages;
    packages_t               m_listOfRepoPackages;

    pkgdbh                   m_dbh;

    std::set<std::string>    m_binaryLibsList;
    std::set<std::string>    m_binarySetList;
    std::set<std::string>    m_binaryCollectionList;
    std::set<std::string>    m_binaryGroupList;
    std::set<std::string>    m_binaryPackageList;

    std::string              m_binaryPackageInfo;
    std::string              m_packageDirName;
    std::string              m_packageFileName;
    std::string              m_packageFileNameSignature;
    std::string              m_packageFileNameHash;
    std::string              m_packageVersion;

    std::vector<std::pair<std::string,time_t>>
                             m_dependenciesList;

    void                     treatErrors(const std::string& s) const;
    void                     parse();
    void                     errors();

public:
    pkgrepo(const std::string& fileName);
    virtual ~pkgrepo() { ERR_free_strings(); }

    void                     generateDependencies(const std::pair<std::string,time_t>& packageName);
    void                     generateDependencies();
    void                     generateDependencies(const std::string& packageName);
    void                     downloadPackageFileName(const std::string& packageName);

    std::set<std::pair<std::string,time_t>>
                             getPackageDependencies (const std::string& filename);

    bool                     checkBinaryExist(const std::string& name);

    // To check the validity of an archive
    //
    // 1. generateHash(filename("gcc")); m_packageFileHash is set
    //
    // 2. checkHash("gcc"); m_packageFileHash is compared with the hash found in the DB
    //
    // Return True if matching.

    // Generate the hashsum for the archive file of the package name and store the result
    // in the m_packageFileNameHash member variable
    void                     generateHash(const std::string& packageName);

    // Store the computed hash stored in m_packageFileNameHash
    // into the database packageName hash property
    void                     hash(const std::string& packageName);

    // Check the Hash and signature of the archive file of the package name
    bool                     checkHash(const std::string& name);

    // Return the m_packageNameFileNameHash value
    std::string&            hash();

    // To generate hash and signature
    //
    // 1. std::string packageName = getName("gcc.cards-11.2-1.xz");
    // 2. generateHash("gcc.cards-11.2-1.xz");  m_packageFileNameHash is set

    // 3. hash(packageName);
    // 4. generateSign(hash());
    // 5. sign(packageName);

    // OR

    // 1. std::string packageName = getName("gcc.cards-11.2-1.xz");
    // 2. generateHash("gcc.cards-11.2-1.xz"); m_packageFileNameHash is set
    // 3. generateSign(); m_packageFileNameHas is used and signed
    // 4. hash(packageName)
    // 5. sign(packageName)
    // Infos stored in the DB

    // Generate the signature of the hashsum stored in the m_packageFileNameHash
    // member variable
    void                     generateSign();

    // Generate the signature of the hashsum for the archive file of the package name
    // and store the result in the m_packageFileNameSignature member variable
    // The private key location is defined in cards.conf file.
    void                     generateSign(const std::string& hash);

    // Store the computed signature stored in m_packageFileNameSignature
    // into the database packageName signature property
    void                    sign(const std::string& packageName);

    // Return the m_packageNameFileNameSignature value
    std::string&            sign();

    // Check the signature of the hashsum of an archive file of the package name
    // The public key location is at the root of the repository or collection
    bool                     checkSign(const std::string& name);

    // Generate the keys pair for signing and verifying the package archive hashing
    void                     generateKeys();

    void                     refresh();
    time_t                   getBinaryBuildTime (const std::string& name);

    std::string&             dirName(const std::string& name);
    std::string&             fileName(const std::string& name);
    std::string&             fileSignature(const std::string& name);
    std::string&             fileHash(const std::string& name);
    std::string&             getBinaryPackageInfo(const std::string& name);
    std::string&             version(const std::string& name);
    unsigned short int       release(const std::string& name);

    std::vector<std::string> getListofGroups();
    std::set<std::string>    getLibs(const std::string& name);
    std::set<std::string>&   getBinaryPackageList();
    std::set<std::string>&   getListOfPackagesFromSet(const std::string& name);
    std::set<std::string>&   getListOfPackagesFromCollection(const std::string& name);
    std::set<std::string>&   getListOfPackagesFromGroup(const std::string& name);

    std::vector<std::pair<std::string,time_t>>&
                             getDependenciesList();
    void                     addDependenciesList(std::pair<std::string,time_t>& name);

    const repo_t&            getListOfPackages();
};

} // end of 'cards' namespace
