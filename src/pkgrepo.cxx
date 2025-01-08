/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "pkgrepo.h"

namespace cards {

pkgrepo::pkgrepo(const std::string& fileName)
    : m_configFileName(fileName)
    , m_config(fileName)
{
    m_dbh.buildDatabase(false);
    parse();
    OpenSSL_add_all_digests();
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();
}
void pkgrepo::treatErrors(const std::string& s) const
{
    switch (m_actualError) {
    case ERROR_ENUM_PRIVATE_KEY_PATH_NOT_DEFINE:
        throw RunTimeErrorWithErrno(_("The path of the private key is not set: ") + s);
        break;
    case ERROR_ENUM_CANNOT_GENERATE_PRIVATE_KEY:
        throw std::runtime_error(_("Failed to generate the private key: ") + s);
        break;
    case ERROR_ENUM_PACKAGE_NOT_EXIST:
        throw std::runtime_error(_("The package ") + s + _(" is not existing"));
        break;
    case ERROR_ENUM_CANNOT_OPEN_FILE:
        throw std::runtime_error(_("could not open ") + s);
        break;
    case ERROR_ENUM_CANNOT_PARSE_FILE:
        throw std::runtime_error(_("could not parse ") + s);
        break;
    case ERROR_ENUM_CANNOT_READ_DIRECTORY:
        throw RunTimeErrorWithErrno(_("could not read directory ") + s);
        break;
    case ERROR_ENUM_CANNOT_SAVE_PRIVATE_KEY:
        throw RunTimeErrorWithErrno(_("Could not save the private key: ") + s);
        break;
    case ERROR_ENUM_CANNOT_SAVE_PUBLIC_KEY:
        throw RunTimeErrorWithErrno(_("Could not save the public key: ") + s);
        break;
    case ERROR_ENUM_CANNOT_CHMOD_PRIVATE_KEY_TO_600:
        throw RunTimeErrorWithErrno(_("Failed to chmod key: ") + s + _(" to 0600"));
        break;
    case ERROR_ENUM_WRONG_SIGNATURE:
        throw RunTimeErrorWithErrno(_("Package: ") + s + _(" have a wrong signature."));
        break;
    }
}
void pkgrepo::generateDependencies(const std::pair<std::string, time_t>& packageName)
{
    m_packageName = packageName.first;
    generateDependencies();
}
void pkgrepo::generateDependencies(const std::string& packageName)
{
    m_packageName = packageName;
    generateDependencies();
}
void pkgrepo::generateKeys()
{
    EVP_PKEY* key = nullptr;
    EVP_PKEY_CTX* ctx = nullptr;
    std::string keyfile(m_config.keypath());
    keyfile += PRIVATEKEY;

    if (m_config.keypath().empty()) {
        m_actualError = cards::ERROR_ENUM_PRIVATE_KEY_PATH_NOT_DEFINE;
        treatErrors(keyfile);
    }

    ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_ED25519, nullptr);
    if (!ctx)
        throw std::runtime_error("Failed to create cipher context");

    if (1 != EVP_PKEY_keygen_init(ctx)) {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize key generation");
    }
    if (1 != EVP_PKEY_generate(ctx, &key)) {
        EVP_PKEY_CTX_free(ctx);
        if (key)
            EVP_PKEY_free(key);

        m_actualError = cards::ERROR_ENUM_CANNOT_GENERATE_PRIVATE_KEY;
        treatErrors(keyfile);
    }
    EVP_PKEY_CTX_free(ctx);

    FILE* f = fopen(keyfile.c_str(), "w");
    if (!f) {
        EVP_PKEY_free(key);
        m_actualError = cards::ERROR_ENUM_CANNOT_OPEN_FILE;
        treatErrors(keyfile);
    }

    if (PEM_write_PrivateKey(f, key, nullptr, nullptr, 0, nullptr, nullptr) == 0) {
        EVP_PKEY_free(key);
        fclose(f);
        m_actualError = cards::ERROR_ENUM_CANNOT_SAVE_PRIVATE_KEY;
        treatErrors(keyfile);
    }
    fclose(f);
    if (chmod(keyfile.c_str(), 0600) == -1) {
        m_actualError = cards::ERROR_ENUM_CANNOT_CHMOD_PRIVATE_KEY_TO_600;
        treatErrors(keyfile);
    }
    keyfile = ".";
    keyfile += PUBLICKEY;

    f = fopen(keyfile.c_str(), "w");
    if (!f) {
        EVP_PKEY_free(key);
        m_actualError = cards::ERROR_ENUM_CANNOT_OPEN_FILE;
        treatErrors(keyfile);
    }
    if (PEM_write_PUBKEY(f, key) == 0) {
        EVP_PKEY_free(key);
        fclose(f);
        m_actualError = cards::ERROR_ENUM_CANNOT_SAVE_PUBLIC_KEY;
        treatErrors(keyfile);
    }
    fclose(f);
    EVP_PKEY_free(key);
}
void pkgrepo::generateDependencies()
{
    std::string packageNameHash, packageName, packageFileName;
    std::pair<std::string, time_t> PackageTime;
    std::vector<std::pair<std::string, time_t>> dependenciesWeMustAdd, depencenciestoSort;
    std::vector<std::pair<std::string, time_t>>::iterator vit;
    std::set<std::pair<std::string, time_t>>::iterator sit;

    PackageTime.first = m_packageName;
    PackageTime.second = 0;

    // Insert the final package
    dependenciesWeMustAdd.push_back(PackageTime);

    // Main while loop
    while (!dependenciesWeMustAdd.empty()) {
        vit = dependenciesWeMustAdd.begin();
        packageName = vit->first;
        PackageTime = *vit;

        // Erase the current treated packageName
        dependenciesWeMustAdd.erase(vit);

        std::set<std::pair<std::string, time_t>> directDependencies;
        if (m_listOfRepoPackages.find(packageName) != m_listOfRepoPackages.end()) {
            directDependencies = m_listOfPackages[packageName].dependencies();
        } else {
            // The packageName is not found in the listOfPackages
            if (checkBinaryExist(packageName)) {

                // The packageName binary is found.
                // Directs dependencies are not yet availables
                // We need to get PackageFileName of the packageName
                // And check it's signature
                packageFileName = dirName(packageName) + "/" + fileName(packageName);
                packageNameHash = fileHash(packageName);
            } else {
                m_actualError = cards::ERROR_ENUM_PACKAGE_NOT_EXIST;
                treatErrors(packageName);
            }

            // archive PackageName exist:
            // Let's see if we need to download it
            // if the binary archive is not yet downloaded
            // Make no sense to download if no url exist
            if (m_config.url().size() > 0) {
                if (!checkFileExist(packageFileName))
                    downloadPackageFileName(packageName);
                // m_packageFileNameHash contains
                // the hashsum of packageFileName
                generateHash(packageFileName);

                // we compare m_packageFileNameHash
                // with the one stored in the database
                if (!checkHash(packageName)) {
                    // Last try to get it.
                    downloadPackageFileName(packageName);
                }
                if (!checkSign(packageName)) {
                    m_actualError = cards::ERROR_ENUM_WRONG_SIGNATURE;
                    treatErrors(packageName);
                }
            }
            directDependencies = getPackageDependencies(packageFileName);
        }

        // If the package is not yet installed or not uptodate
        if (!m_dbh.checkPackageNameBuildDateSame(PackageTime)) {
            // checkPackageNameBuildDateSame is no then add it
            depencenciestoSort.push_back(PackageTime);
        }

        // else checkPackageNameBuildDateSame is yes

        for (auto sit : directDependencies) {
            if (sit.first == PackageTime.first)
                continue;
            for (vit = dependenciesWeMustAdd.begin(); vit != dependenciesWeMustAdd.end(); ++vit) {
                if (sit == *vit) {
                    dependenciesWeMustAdd.erase(vit);
                    break;
                }
            }
        }
        for (sit = directDependencies.begin(); sit != directDependencies.end(); sit++) {
            if (PackageTime.first != sit->first) {
                if (!m_dbh.checkPackageNameBuildDateSame(*sit))
                    dependenciesWeMustAdd.push_back(*sit);
            }
        }
    }

    // Let's revert the order of dependencies
    // We need the last founds (lower level) first
    bool found = false;
    for (std::vector<std::pair<std::string, time_t>>::reverse_iterator vrit = depencenciestoSort.rbegin();
         vrit != depencenciestoSort.rend();
         ++vrit) {
        found = false;
        for (auto i : m_dependenciesList) {
            if (i.first == vrit->first) {
                found = true;
                break;
            }
        }
        if (!found) {
            m_dependenciesList.push_back(*vrit);
        }
        // else no deps founds
    }
}
void pkgrepo::downloadPackageFileName(const std::string& packageName)
{
    cards::dwl archive(m_listOfPackages[packageName].origin()
            + "/"
            + m_listOfPackages[packageName].fileName(),
        m_listOfPackages[packageName].dirName(),
        m_listOfPackages[packageName].fileName(),
        true);
}
std::vector<std::pair<std::string, time_t>>&
pkgrepo::getDependenciesList()
{
    return m_dependenciesList;
}
void pkgrepo::addDependenciesList(std::pair<std::string, time_t>& name)
{
    m_dependenciesList.push_back(name);
}
std::set<std::string>& pkgrepo::getBinaryPackageList()
{
    parse();
    std::string packageName;
    for (auto p : m_listOfPackages) {
        packageName = "("
            + p.second.collection()
            + ") "
            + p.first
            + " "
            + p.second.version()
            + " "
            + p.second.description();
        m_binaryPackageList.insert(packageName);
    }
    return m_binaryPackageList;
}
std::string& pkgrepo::getBinaryPackageInfo(const std::string& name)
{
    parse();
    std::string alias, categories, group, sets;

    if (m_listOfPackages.find(name) != m_listOfPackages.end()) {
        if (m_listOfPackages[name].group().size() > 0) {
            group = _("Group          : ");
            group += m_listOfPackages[name].group();
            group += '\n';
        }
        if (m_listOfPackages[name].alias().size() > 0) {
            alias = _("Alias          :");
            for (auto i : m_listOfPackages[name].alias()) {
                alias += " ";
                alias += i;
            }
            alias += '\n';
        }
        if (m_listOfPackages[name].sets().size() > 0) {
            sets = _("Sets           :");
            for (auto i : m_listOfPackages[name].sets()) {
                sets += " ";
                sets += i;
            }
            sets += '\n';
        }
        if (m_listOfPackages[name].categories().size() > 0) {
            categories = _("Categories     :");
            for (auto i : m_listOfPackages[name].categories()) {
                categories += " ";
                categories += i;
            }
            categories += '\n';
        }
        m_binaryPackageInfo = _("Name           : ")
            + name
            + '\n'
            + alias
            + _("Description    : ")
            + m_listOfPackages[name].description()
            + '\n'
            + _("Archive name   : ")
            + m_listOfPackages[name].fileName()
            + '\n'
            + group
            + categories
            + _("Collection     : ")
            + m_listOfPackages[name].collection()
            + '\n'
            + sets
            + _("URL            : ")
            + m_listOfPackages[name].url()
            + '\n'
            + _("License        : ")
            + m_listOfPackages[name].license()
            + '\n'
            + _("Version        : ")
            + m_listOfPackages[name].version()
            + '\n'
            + _("Release        : ")
            + itos(m_listOfPackages[name].release())
            + '\n'
            + _("Build date     : ")
            + getDateFromEpoch(m_listOfPackages[name].build())
            + '\n'
            + _("Hashsum        : ")
            + m_listOfPackages[name].hash()
            + '\n'
            + _("Signature      : ")
            + m_listOfPackages[name].signature()
            + '\n'
            + _("Archive Size   : ")
            + sizeHumanRead(m_listOfPackages[name].size())
            + _("bytes")
            + '\n'
            + _("Installed Size : ")
            + sizeHumanRead(m_listOfPackages[name].space())
            + _("bytes")
            + '\n'
            + _("Maintainer(s)  : ")
            + m_listOfPackages[name].maintainer()
            + '\n'
            + _("Packager       : ")
            + m_listOfPackages[name].packager()
            + '\n'
            + _("Contributor(s) : ")
            + m_listOfPackages[name].contributors()
            + '\n'
            + _("Arch           : ")
            + m_listOfPackages[name].arch()
            + '\n'
            + _("Binary URL     : ")
            + m_listOfPackages[name].origin()
            + '\n'
            + _("Local folder   : ")
            + m_listOfPackages[name].dirName()
            + '\n';
        if (m_listOfPackages[name].dependencies().size() > 0) {
            std::string dependencies;
            for (auto d : m_listOfPackages[name].dependencies()) {
                dependencies += d.first;
                dependencies += " ";
            }
            m_binaryPackageInfo += _("Dependencies   : ");
            m_binaryPackageInfo += dependencies;
            m_binaryPackageInfo += '\n';
        }
    }
    return m_binaryPackageInfo;
}
void pkgrepo::refresh()
{
    m_listOfPackages.clear();
    parse();
}
void pkgrepo::parse()
{
    if (m_listOfPackages.size() > 0)
        return;

    cache info;

    for (auto i : m_config.dirUrl()) {
        info.dirName(i.depot + "/" + i.collection);
        std::string repoFile = info.dirName()
            + PKG_REPO_META;

        info.collection(i.collection);
        info.origin(i.url
            + "/"
            + getMachineType()
            + "/"
            + m_config.version()
            + "/"
            + i.collection);
        std::vector<std::string> repoFileContent;

        if (parseFile(repoFileContent, repoFile.c_str()) != 0) {
            std::cerr << "Cannot read the file: "
                      << repoFile
                      << std::endl
                      << "... continue with next"
                      << std::endl;
            continue;
        }
        bool pkgFound = false;
        std::string::size_type pos;
        std::string pkgName;
        std::set<std::string> pkgSet;
        std::set<std::string> pkgAlias;
        std::set<std::string> pkgCategories;
        std::set<std::string> pkgConflicts;
        std::set<std::string> pkgLibs;
        std::set<std::pair<std::string, time_t>> pkgDependencies;
        std::pair<std::string, time_t> dependency;
        for (auto p : repoFileContent) {
            if (p[0] == '@') {
                pos = p.find(".cards-");
                if (pos != std::string::npos) {
                    info.fileName(p.substr(1));
                    info.group("");
                    info.baseName("");
                    pkgFound = true;
                }
                continue;
            }
            if (pkgFound) {
                switch (p[0]) {
                case NAME:
                    pkgName = p.substr(1);
                    continue;
                case VERSION:
                    info.version(p.substr(1));
                    continue;
                case RELEASE:
                    info.release(stoi(p.substr(1)));
                    continue;
                case SPACE:
                    info.space(atoi(p.substr(1).c_str()));
                    continue;
                case ARCHITECTURE:
                    info.arch(p.substr(1));
                    continue;
                case DESCRIPTION:
                    info.description(p.substr(1));
                    continue;
                case URL:
                    info.url(p.substr(1));
                    continue;
                case CONTRIBUTORS:
                    info.contributors(p.substr(1));
                    continue;
                case MAINTAINER:
                    info.maintainer(p.substr(1));
                    continue;
                case PACKAGER:
                    info.packager(p.substr(1));
                    continue;
                case LICENSE:
                    info.license(p.substr(1));
                    continue;
                case BUILD:
                    info.build(strtoul(p.substr(1).c_str(), nullptr, 0));
                    continue;
                case COLLECTION:
                    info.collection(p.substr(1));
                    continue;
                case GROUP:
                    info.group(p.substr(1));
                    info.baseName(pkgName.substr(0,pkgName.size() - p.size()));
                    continue;
                case SHARED_LIB:
                    pkgLibs.insert(p.substr(1));
                    continue;
                case RUNTIME_DEPENDENCY:
                    dependency.first = pkgName;
                    if (p.size() > 14) {
                        dependency.first = p.substr(1, p.size() - 11);
                        dependency.second = strtoul(p.substr((p.size()) - 10).c_str(), nullptr, 0);
                        pkgDependencies.insert(dependency);
                    }
                    continue;
                case ALIAS:
                    pkgAlias.insert(p.substr(1));
                    continue;
                case SETS:
                    pkgSet.insert(p.substr(1));
                    continue;
                case CATEGORIES:
                    pkgCategories.insert(p.substr(1));
                    continue;
                case CONFLICTS:
                    pkgConflicts.insert(p.substr(1));
                    continue;
                case HASHSUM:
                    info.hash(p.substr(1));
                    continue;
                case SIGNATURE:
                    info.signature(p.substr(1));
                    continue;
                case SIZE:
                    info.size(atoi(p.substr(1).c_str()));
                    continue;
                }
            }
            if (p.size() == 0) {
                if ( (info.collection().size() == 0) &&
                        (info.baseName().size() > 0) )
                    info.collection(m_listOfPackages[info.baseName()].collection());
                pkgFound = false;
                info.libs(pkgLibs);
                info.alias(pkgAlias);
                info.sets(pkgSet);
                info.categories(pkgCategories);
                info.dependencies(pkgDependencies);
                info.conflicts(pkgConflicts);
                pkgAlias.clear();
                pkgSet.clear();
                pkgCategories.clear();
                pkgDependencies.clear();
                pkgConflicts.clear();
                pkgLibs.clear();
                m_listOfPackages[pkgName] = info;
                continue;
            }
        }
        repoFile = i.depot
            + "/"
		    + i.collection
		    + PKG_REPO_FILES;
        repoFileContent.clear();
		if (parseFile(repoFileContent,repoFile.c_str()) != 0) {
            std::cerr << "Cannot read the file: "
                      << repoFile
                      << std::endl
                      << "... continue with next"
                      << std::endl;
            continue;
		}
        pkgFound = false;
		for ( auto p : repoFileContent) {
			if (p[0] == '@') {
                pos = p.find(".cards-");
				if (pos != std::string::npos) {
                    pkgName = p.substr(1,pos-1);
					pkgFound = true;
					continue;
				}
			}
			if (pkgFound) {
				if (p.size() > 0) {
					m_listOfPackages[pkgName].files.insert(p);
				} else {
					pkgFound = false;
				}
			}
		}
    }
}
std::set<std::pair<std::string, time_t>>
pkgrepo::getPackageDependencies(const std::string& filename)
{
    archive packageArchive(filename);
    cards::db package;
    std::set<std::pair<std::string, time_t>> packageNameDepsBuildTime;
    std::set<std::pair<std::string, time_t>> packageNameDepsBuildTimeTemp;

    /*
     * Begin of Direct Dependencies of packageArchive
     *
     * If the packageArchive is uptodate and installed
     * we don't need to go further. We return an empty
     * set<string, time_t> container
     *
     */
    if (m_dbh.checkPackageNameUptodate(packageArchive))
        return packageNameDepsBuildTime;

    if (!packageArchive.listofDependencies().empty()) {
        package.dependencies(packageArchive.listofDependenciesBuildDate());
        m_listOfRepoPackages[packageArchive.name()] = package;
    }

    packageNameDepsBuildTimeTemp = package.dependencies();

    for (auto it : packageNameDepsBuildTimeTemp) {
        /*
         * If 'it' package is actual and already present
         * do nothing and continue
         */
        if (m_dbh.checkPackageNameBuildDateSame(it))
            continue;
        /*
         * Otherwise add it to the deps
         */
        packageNameDepsBuildTime.insert(it);
    }

    if (!packageNameDepsBuildTime.empty())
        m_listOfRepoPackages[packageArchive.name()].dependencies(packageNameDepsBuildTime);

    return packageNameDepsBuildTime;
}
const repo_t& pkgrepo::getListOfPackages()
{
    parse();

    return m_listOfPackages;
}
std::set<std::string>& pkgrepo::getListOfPackagesFromSet(const std::string& name)
{
    m_binarySetList.clear();
    parse();

    for (auto p : m_listOfPackages) {
        for (auto s : m_listOfPackages[p.first].sets()) {
            if ((s == name) && (p.second.group() == "")) {
                m_binarySetList.insert(p.first);
                break;
            }
        }
    }

    return m_binarySetList;
}
std::set<std::string>& pkgrepo::getListOfPackagesFromCollection(const std::string& name)
{
    m_binaryCollectionList.clear();
    parse();

    for (auto p : m_listOfPackages) {
        if ((m_listOfPackages[p.first].collection() == name) && (m_listOfPackages[p.first].group() == "")) {
            m_binaryCollectionList.insert(p.first);
        }
    }

    return m_binaryCollectionList;
}
bool pkgrepo::checkBinaryExist(const std::string& name)
{
    parse();

    return (m_listOfPackages.find(name) != m_listOfPackages.end());
}
std::string& pkgrepo::dirName(const std::string& name)
{
    parse();

    m_packageDirName = m_listOfPackages[name].dirName();

    return m_packageDirName;
}
unsigned int pkgrepo::getSpace(const std::string& name)
{
    parse();
    return m_listOfPackages[name].space();
}
unsigned int pkgrepo::getSize(const std::string& name)
{
    parse();
    return m_listOfPackages[name].size();
}
std::string& pkgrepo::fileName(const std::string& name)
{
    parse();

    m_packageFileName = m_listOfPackages[name].fileName();

    return m_packageFileName;
}
std::string& pkgrepo::fileHash(const std::string& name)
{
    parse();

    m_packageFileNameHash = m_listOfPackages[name].hash();

    return m_packageFileNameHash;
}
std::string& pkgrepo::fileSignature(const std::string& name)
{
    parse();

    m_packageFileNameSignature = m_listOfPackages[name].signature();

    return m_packageFileNameSignature;
}
std::vector<std::string> pkgrepo::getListOfGroups()
{
    return m_config.groups();
}
time_t pkgrepo::getBinaryBuildTime(const std::string& name)
{

    parse();

    return m_listOfPackages[name].build();
}
std::string& pkgrepo::version(const std::string& name)
{
    parse();

    m_packageVersion = m_listOfPackages[name].version();
    return m_packageVersion;
}
unsigned short int pkgrepo::release(const std::string& name)
{
    parse();

    return m_listOfPackages[name].release();
}
void pkgrepo::generateHash(const std::string& fileName)
{
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();

    if (!mdctx) {
        errors();
        throw std::runtime_error("Failed to create digest context");
    }

    if (1 != EVP_DigestInit_ex(mdctx, EVP_sha512(), nullptr)) {
        errors();
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("Failed to init digest context");
    }

    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + fileName);
    }

    char buffer[BUFSIZ];

    while (file.good()) {
        file.read(buffer, BUFSIZ);
        std::streamsize bytesRead = file.gcount();
        if (bytesRead > 0) {
            if (1 != EVP_DigestUpdate(mdctx, buffer, bytesRead)) {
                errors();
                throw std::runtime_error("Failed to update digest context");
            }
        }
    }

    if (file.bad()) {
        throw std::runtime_error("Failed to read file: " + fileName);
    }

    unsigned char mdValue[EVP_MAX_MD_SIZE];
    unsigned int mdLength;

    if (1 != EVP_DigestFinal_ex(mdctx, mdValue, &mdLength)) {
        errors();
        throw std::runtime_error("Failed to finalize digest context");
    }

    EVP_MD_CTX_free(mdctx);

    std::ostringstream oss;
    for (unsigned int i = 0; i < mdLength; i++)
        oss << std::hex
            << std::setw(2)
            << std::setfill('0')
            << static_cast<int>(mdValue[i]);

    m_packageFileNameHash = oss.str();
}
std::string& pkgrepo::hash()
{
    return m_packageFileNameHash;
}
void pkgrepo::hash(const std::string& packageName)
{
    parse();

    m_listOfPackages[packageName].hash(m_packageFileNameHash);
}
bool pkgrepo::checkHash(const std::string& name)
{
    parse();

    return convertToLowerCase(m_packageFileNameHash) == convertToLowerCase(m_listOfPackages[name].hash());
}
void pkgrepo::generateSign()
{
    generateSign(m_packageFileNameHash);
}
void pkgrepo::generateSign(const std::string& hash)
{
    EVP_PKEY* key = nullptr;
    std::string keyfile(m_config.keypath());
    keyfile += PRIVATEKEY;

    if (m_config.keypath().empty()) {
        m_actualError = cards::ERROR_ENUM_PRIVATE_KEY_PATH_NOT_DEFINE;
        treatErrors(keyfile);
    }

    std::vector<unsigned char> mesBytes(hash.length() / 2);
    for (size_t i = 0; i < hash.length(); i += 2)
        mesBytes[i / 2] = static_cast<unsigned char>(std::stoi(hash.substr(i, 2), nullptr, 16));

    FILE* keyFileStream = fopen(keyfile.c_str(), "r");
    if (!keyFileStream) {
        m_actualError = cards::ERROR_ENUM_CANNOT_OPEN_FILE;
        treatErrors(keyfile);
    }
    key = PEM_read_PrivateKey(keyFileStream, nullptr, nullptr, nullptr);
    fclose(keyFileStream);
    if (!key)
        throw std::runtime_error("Failed to load private key from PEM file");

    if (EVP_PKEY_id(key) != EVP_PKEY_ED25519) {
        EVP_PKEY_free(key);
        throw std::runtime_error("Loaded key is not an ED25519 format key");
    }

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        errors();
        EVP_PKEY_free(key);
        throw std::runtime_error("Failed to create digest context");
    }

    if (1 != EVP_DigestSignInit(mdctx, nullptr, nullptr, nullptr, key)) {
        errors();
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(key);
        throw std::runtime_error("Failed to init digest context");
    }

    size_t sigLength = 0;
    if (1 != EVP_DigestSign(mdctx, nullptr, &sigLength, mesBytes.data(), mesBytes.size())) {
        errors();
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(key);
        throw std::runtime_error("Failed to sign digest context");
    }

    std::vector<unsigned char> signature(sigLength);
    if (1 != EVP_DigestSign(mdctx, signature.data(), &sigLength, mesBytes.data(), mesBytes.size())) {
        errors();
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(key);
        throw std::runtime_error("Failed to sign digest context (pass 2)");
    }

    EVP_MD_CTX_free(mdctx);
    std::ostringstream oss;
    for (auto b : signature) {
        oss << std::hex
            << std::setw(2)
            << std::setfill('0')
            << static_cast<int>(b);
    }
    m_packageFileNameSignature = oss.str();

    if (key)
        EVP_PKEY_free(key);
    EVP_cleanup();
}
std::string& pkgrepo::sign()
{
    return m_packageFileNameSignature;
}
void pkgrepo::sign(const std::string& packageName)
{
    parse();

    m_listOfPackages[packageName].signature(m_packageFileNameSignature);
}
bool pkgrepo::checkSign(const std::string& name)
{
    EVP_PKEY* key = nullptr;

    // generated hashsum is stored in m_packageFileNameHash
    generateHash(m_listOfPackages[name].dirName()
        + "/"
        + m_listOfPackages[name].fileName());

    std::string hash(m_packageFileNameHash);
    std::string keyfile(m_listOfPackages[name].dirName());
    keyfile += PUBLICKEY;

    std::vector<unsigned char> mesBytes(hash.length() / 2);
    for (size_t i = 0; i < hash.length(); i += 2)
        mesBytes[i / 2] = static_cast<unsigned char>(std::stoi(hash.substr(i, 2), nullptr, 16));

    FILE* keyFileStream = fopen(keyfile.c_str(), "r");
    if (!keyFileStream) {
        m_actualError = cards::ERROR_ENUM_CANNOT_OPEN_FILE;
        treatErrors(keyfile);
    }
    key = PEM_read_PUBKEY(keyFileStream, nullptr, nullptr, nullptr);
    fclose(keyFileStream);
    if (!key) {
        throw std::runtime_error("Failed to load public key from PEM file");
    }
    if (EVP_PKEY_id(key) != EVP_PKEY_ED25519) {
        EVP_PKEY_free(key);
        throw std::runtime_error("Loaded key is not an ED25519 format key");
    }
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        errors();
        return false;
    }

    if (1 != EVP_DigestVerifyInit(mdctx, nullptr, nullptr, nullptr, key)) {
        errors();
        EVP_MD_CTX_free(mdctx);
        return false;
    }

    std::string signature = m_listOfPackages[name].signature();
    std::vector<unsigned char> sigBytes(signature.length() / 2);
    for (size_t i = 0; i < signature.length(); i += 2)
        sigBytes[i / 2] = static_cast<unsigned char>(std::stoi(signature.substr(i, 2), nullptr, 16));

    int verify = EVP_DigestVerify(mdctx, sigBytes.data(), sigBytes.size(), mesBytes.data(), mesBytes.size());
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(key);

    return verify == 1;
}
void pkgrepo::errors()
{
    ERR_print_errors_fp(stderr);
}

// We needs to parse again, this function is use
// for installing fresh build packages in create object
std::set<std::string>& pkgrepo::getListOfPackagesFromGroup(const std::string& name)
{
    m_binaryGroupList.clear();

    parse();

    if (checkBinaryExist(name)) {
        m_binaryGroupList.insert(m_listOfPackages[name].dirName()
                + "/"
                + m_listOfPackages[name].fileName());

        for (auto p : m_listOfPackages) {
            if (p.second.baseName() == name) {
                m_binaryGroupList.insert(p.second.dirName()
                + "/"
                + p.second.fileName());
            }
        }
    }

    return m_binaryGroupList;
}
std::set<std::string> pkgrepo::getLibs(const std::string& name)
{
    parse();

    return m_listOfPackages[name].libs();
}
std::set<std::string>& pkgrepo::getListOfFiles(const std::string& name)
{
    parse();
    return m_listOfPackages[name].files;
}
} // end of 'cards' namespace
