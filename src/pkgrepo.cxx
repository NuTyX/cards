/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "pkgrepo.h"

namespace cards {
	
pkgrepo::pkgrepo(const std::string& fileName)
    : m_configFileName(fileName)
	, m_config(fileName)
{
	m_dbh.buildSimpleDatabase();
	parse();
	OpenSSL_add_all_digests();
	OpenSSL_add_all_algorithms();
	ERR_load_crypto_strings();
}
void pkgrepo::generateDependencies
	(const std::pair<std::string,time_t>& packageName)
{
	m_packageName = packageName.first;
	generateDependencies();
}
void pkgrepo::generateDependencies
	(const std::string& packageName)
{
	m_packageName = packageName;
	generateDependencies();
}
bool pkgrepo::generateKeys()
{
	EVP_PKEY*     key = nullptr;
	EVP_PKEY_CTX* ctx = nullptr;

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
		throw std::runtime_error("Failed to generate the private key");
	}
	EVP_PKEY_CTX_free(ctx);

	std::string keyfile(m_config.keypath());
	keyfile += PRIVATEKEY;
	FILE *f = fopen(keyfile.c_str(), "w");
	if (!f) {
		EVP_PKEY_free(key);
		throw std::runtime_error("Failed to create file: " + keyfile);
	}

	if (PEM_write_PrivateKey(f, key, nullptr, nullptr, 0, nullptr, nullptr) == 0 ) {
		EVP_PKEY_free(key);
		fclose(f);
		throw std::runtime_error("Failed to save the private key: " + keyfile);
	}
	fclose(f);
	if (chmod(keyfile.c_str(),0600) == -1)
		throw std::runtime_error("Failed to chmod key: " + keyfile + " to 0600");

	keyfile = ".";
	keyfile += PUBLICKEY;

	f = fopen(keyfile.c_str(), "w");
	if (!f) {
		EVP_PKEY_free(key);
		throw std::runtime_error("Failed to create file: " + keyfile);
	}
	if (PEM_write_PUBKEY(f, key) == 0 ) {
		EVP_PKEY_free(key);
		fclose(f);
		throw std::runtime_error("Failed to save the public key: " + keyfile);
	}
	fclose(f);
	EVP_PKEY_free(key);
	return true;
}
void pkgrepo::generateDependencies()
{
	std::vector<std::pair<std::string,time_t>> dependenciesWeMustAdd,depencenciestoSort;
	std::pair<std::string,time_t> PackageTime;
	PackageTime.first=m_packageName;
	PackageTime.second=0;

	/* Always insert the final package first (twice) */
	dependenciesWeMustAdd.push_back(PackageTime);
	dependenciesWeMustAdd.push_back(PackageTime);

	std::vector< std::pair<std::string,time_t> >::iterator vit;
	std::set< std::pair<std::string,time_t> >::iterator sit;
	std::string packageNameSignature, packageName, packageFileName;

	/* Main while loop */
	while ( ! dependenciesWeMustAdd.empty() ) {
		vit = dependenciesWeMustAdd.begin();
		packageName = vit->first;
		PackageTime = *vit;

		/* Erase the current treated packageName */
		dependenciesWeMustAdd.erase(vit);

		std::set<std::pair<std::string,time_t>> directDependencies;
		if ( getListOfPackages().find(packageName) != getListOfPackages().end() ) {
			directDependencies = getListOfPackages()[packageName].dependencies();
		} else {

			/* The packageName is not found in the listOfPackages */

			if ( checkBinaryExist(packageName)) {
				/* The packageName binary is found.
				 * Directs dependencies are not availables yes
				 * We need to get PackageFileName of the packageName
				 * And check it's signature
				 */

				packageFileName = fileName(packageName);
				packageNameSignature = fileSignature(packageName);

				/* archive PackageName exist
				 * Let's see if we need to download it */

			} else {
				/*
				 * FIXME
				m_actualError = cards::ERROR_ENUM_PACKAGE_NOT_EXIST;
				treatErrors (packageName);
				*/
			}

			/* If the binary archive is not yet downloaded or is corrupted */
			if (!checkFileHash(packageFileName, packageNameSignature)) {
				/* Try to get it */
				downloadPackageFileName(packageName);
			}
			directDependencies = getPackageDependencies(packageFileName);
		}

		/* If the package is not yet installed or not uptodate */
		if (!m_dbh.checkPackageNameBuildDateSame(PackageTime))
		{
			/* checkPackageNameBuildDateSame is no then add it */
			depencenciestoSort.push_back(PackageTime);
		}

		/* else checkPackageNameBuildDateSame is yes*/

		for ( auto sit : directDependencies) {
			if ( sit.first == PackageTime.first )
				continue;
			for ( vit = dependenciesWeMustAdd.begin(); vit != dependenciesWeMustAdd.end();++vit) {
				if ( sit == *vit) {
					dependenciesWeMustAdd.erase(vit);
					break;
				}
			}
		}
		for ( sit = directDependencies.begin(); sit != directDependencies.end();sit++) {
			if ( PackageTime.first != sit->first ) {
				if (!m_dbh.checkPackageNameBuildDateSame(*sit))
					dependenciesWeMustAdd.push_back(*sit);
			}
		}
	}

	/* Let's revert the order of dependencies
	 * We need the last founds (lower level) first
	 */
	bool found = false ;
	for ( std::vector<std::pair<std::string,time_t>>::reverse_iterator vrit = depencenciestoSort.rbegin();
		vrit != depencenciestoSort.rend();
		++vrit) {
		found = false ;
		for (auto i : m_dependenciesList) {
			if ( i.first == vrit->first) {
				found = true ;
				break;
			}
		}
		if (!found) {
			m_dependenciesList.push_back(*vrit);
		}

		/* else no deps founds */

	}
}
void pkgrepo::downloadPackageFileName(const std::string& packageName)
{
	/* FIXME */
}
std::vector<std::pair<std::string,time_t>>&
pkgrepo::getDependenciesList()
{
	return m_dependenciesList;
}
void pkgrepo::addDependenciesList
	(std::pair<std::string,time_t>& name)
{
	m_dependenciesList.push_back(name);
}
std::set<std::string>& pkgrepo::getBinaryPackageList() {
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
	if (m_listOfPackages.find(name) != m_listOfPackages.end()) {
		m_binaryPackageInfo = _("Name           : ")
			+ name + '\n'
			+ _("Description    : ")
			+ m_listOfPackages[name].description()
			+ '\n'
			+ _("Groups         : ")
			+ m_listOfPackages[name].group()
			+ '\n'
			+ _("URL            : ")
			+ m_listOfPackages[name].url()
			+ '\n'
			+ _("License        : ")
			+ m_listOfPackages[name].license()
			+ '\n'
			+ _("Version        : ")
			+ m_listOfPackages[name].version()
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
			+ '\n';
		if (m_listOfPackages[name].dependencies().size() > 0 ) {
			std::string dependencies;
			for (auto d:m_listOfPackages[name].dependencies()) {
				dependencies+=d.first;
				dependencies+=" ";
			}
			m_binaryPackageInfo += _("Dependencies   : ");
			m_binaryPackageInfo += dependencies;
			m_binaryPackageInfo += '\n';
		}
	}
	return m_binaryPackageInfo;
}
void pkgrepo::parse()
{
	if (m_listOfPackages.size() > 0)
		return;

	cards:cache info;
	

	for (auto i : m_config.dirUrl()) {
		info.dirName(i.depot + "/" + i.collection);
		std::string repoFile = info.dirName() 
			+ PKG_REPO;

		info.collection(i.collection);
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
		std::set<std::pair<std::string,time_t>> pkgDependencies;
		for (auto p : repoFileContent) {
			if (p[0] == '@')  {
				pos = p.find(".cards-");
				if ( pos != std::string::npos) {
					pkgName = p.substr(1,pos - 1);
					info.version(p.substr(pos + 7));
					info.fileName(p.substr(1));
					pkgFound = true;
				}
			}
			if (pkgFound)
				if (p[0] == DESCRIPTION)
					info.description(p.substr(1));
			if (pkgFound)
				if (p[0] == URL)
					info.url(p.substr(1));
			if (pkgFound)
				if (p[0] == LICENSE)
					info.license(p.substr(1));
			if (pkgFound)
				if (p[0] == MAINTAINER)
					info.maintainer(p.substr(1));
			if (pkgFound)
				if (p[0] == CONTRIBUTORS)
					info.contributors(p.substr(1));
			if (pkgFound)
				if (p[0] == COLLECTION)
					info.collection(p.substr(1));
			if (pkgFound)
				if (p[0] == GROUP)
					info.group(p.substr(1));
			if (pkgFound)
				if (p[0] == PACKAGER)
					info.packager(p.substr(1));
			if (pkgFound)
				if (p[0] == ARCHITECTURE)
					info.arch(p.substr(1));
			if (pkgFound)
				if (p[0] == HASHSUM)
					info.hash(p.substr(1));
			if (pkgFound)
				if (p[0] == SIGNATURE)
					info.signature(p.substr(1));
			if (pkgFound)
				if (p[0] == SPACE)
					info.space(atoi(p.substr(1).c_str()));
			if (pkgFound)
				if (p[0] == SETS)
                        pkgSet.insert(p.substr(1));
            if (pkgFound)
				if (p[0] == ALIAS)
				    pkgAlias.insert(p.substr(1));
            if (pkgFound)
				if (p[0] == CATEGORIES)
				    pkgCategories.insert(p.substr(1));
			if (pkgFound)
				if (p[0] == RUNTIME_DEPENDENCY) {
					std::pair<std::string,time_t> dep;
					dep.first = p.substr(1);
					pkgDependencies.insert(dep);
				}
			if (pkgFound)
				if (p.size() == 0) {
					pkgFound = false;
                    info.alias(pkgAlias);
					info.sets(pkgSet);
                    info.categories(pkgCategories);
					info.dependencies(pkgDependencies);
                    pkgAlias.clear();
					pkgSet.clear();
                    pkgCategories.clear();
					pkgDependencies.clear();
					m_listOfPackages[pkgName] = info;
				}
		}
	}
}
std::set<std::pair<std::string,time_t>>
pkgrepo::getPackageDependencies (const std::string& filename)
{
	archive                                 packageArchive(filename);
	cards::db                               package;
	std::set<std::pair<std::string,time_t>> packageNameDepsBuildTime;
	std::set<std::pair<std::string,time_t>> packageNameDepsBuildTimeTemp;

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

	for (auto it : packageNameDepsBuildTimeTemp ) {
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
repo_t& pkgrepo::getListOfPackages()
{
    if (m_listOfPackages.size() == 0)
        parse();

    return m_listOfPackages;
}
std::set<std::string>& pkgrepo::getListOfPackagesFromSet(const std::string& name)
{
	if (m_listOfPackages.size() == 0)
        parse();

	for (auto p: m_listOfPackages) {
		for (auto s : m_listOfPackages[p.first].sets()) {
			if (s == name) {
				m_binarySetList.insert(p.first);
				break;
			}
		}
	}

	return m_binarySetList;
}
std::set<std::string>& pkgrepo::getListOfPackagesFromCollection(const std::string& name)
{
	if (m_listOfPackages.size() == 0)
        parse();

	for (auto p: m_listOfPackages) {
		if (m_listOfPackages[p.first].collection() == name) {
			m_binaryCollectionList.insert(p.first);
		}
	}

	return m_binaryCollectionList;
}
bool pkgrepo::checkBinaryExist(const std::string& name)
{
	if (m_listOfPackages.size() == 0)
        parse();

	return (m_listOfPackages.find(name) != m_listOfPackages.end());
}
std::string& pkgrepo::dirName(const std::string& name)
{
    if (m_listOfPackages.size() == 0)
        parse();

    m_packageDirName = m_listOfPackages[name].dirName();

    return m_packageDirName;
}
std::string& pkgrepo::fileName(const std::string& name)
{
    if (m_listOfPackages.size() == 0)
        parse();

    m_packageFileName = m_listOfPackages[name].fileName();

    return m_packageFileName;
}
std::string& pkgrepo::fileHash(const std::string& name)
{
    if (m_listOfPackages.size() == 0)
        parse();

     m_packageFileNameHash = m_listOfPackages[name].hash();

    return m_packageFileNameHash;
}
std::string& pkgrepo::fileSignature(const std::string& name)
{
    if (m_listOfPackages.size() == 0)
        parse();

    m_packageFileNameSignature = m_listOfPackages[name].signature();

    return m_packageFileNameSignature;
}
std::vector<std::string> pkgrepo::getListofGroups()
{
	return m_config.groups();
}
time_t pkgrepo::getBinaryBuildTime (const std::string& name)
{

	if (m_listOfPackages.size() == 0)
		parse();

	return m_listOfPackages[name].build();
}
std::string& pkgrepo::version(const std::string& name)
{
	if (m_listOfPackages.size() == 0)
		parse();

	m_packageVersion = m_listOfPackages[name].version();
	return m_packageVersion;
}
unsigned short int pkgrepo::release(const std::string& name)
{
	if (m_listOfPackages.size() == 0)
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

	if (1 != EVP_DigestInit_ex(mdctx,EVP_sha512(), nullptr)) {
		errors();
		EVP_MD_CTX_free(mdctx);
		throw std::runtime_error("Failed to init digest context");
	}

	std::ifstream file(fileName,std::ios::binary);
	if(!file.is_open()) {
		throw std::runtime_error("Failed to open file: " + fileName);
	}

	char buffer[BUFSIZ];

	while (file.good())  {
		file.read(buffer,BUFSIZ);
		std::streamsize bytesRead = file.gcount();
		if (bytesRead > 0) {
			if ( 1 != EVP_DigestUpdate(mdctx, buffer, bytesRead)) {
				errors();
				throw std::runtime_error("Failed to update digest context");
			}
		}
	}

	if (file.bad()) {
		throw std::runtime_error("Failed to read file: " + fileName);
	}

	unsigned char mdValue[EVP_MAX_MD_SIZE];
	unsigned int  mdLength;

	if ( 1 != EVP_DigestFinal_ex(mdctx, mdValue, &mdLength)) {
		errors();
		throw std::runtime_error("Failed to finalize digest context");
	}

	EVP_MD_CTX_free(mdctx);

	std::ostringstream oss;
	for ( unsigned int i = 0; i < mdLength; i++)
		oss << std::hex
			<< std::setw(2)
			<< std::setfill('0')
			<< static_cast<int>(mdValue[i]);

	m_packageFileNameHash = oss.str();
}
std::string& pkgrepo::hash()
{
	return  m_packageFileNameHash;
}
void pkgrepo::hash(const std::string& packageName)
{
	if (m_listOfPackages.size() == 0)
		parse();

	m_listOfPackages[packageName].hash(m_packageFileNameHash);
}
bool pkgrepo::checkHash(const std::string& name)
{
	if (m_listOfPackages.size() == 0)
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
	std::vector<unsigned char> mesBytes(hash.length() / 2);
	for (size_t i = 0; i < hash.length(); i += 2 )
		mesBytes[i / 2] = static_cast<unsigned char>(std::stoi(hash.substr(i,2), nullptr, 16));

	FILE* keyFileStream = fopen(keyfile.c_str(), "r");
	if (!keyFileStream) {
		throw std::runtime_error("Failed to open key file:" + keyfile);
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
	if (m_listOfPackages.size() == 0)
		parse();

	m_listOfPackages[packageName].signature(m_packageFileNameSignature);
}
bool pkgrepo::checkSign(const std::string& name)
{
	EVP_PKEY* key = nullptr;
	std::string hash(m_listOfPackages[name].hash());

	std::string keyfile(m_listOfPackages[name].dirName());
	keyfile += PUBLICKEY;

	std::vector<unsigned char> mesBytes(hash.length() / 2);
	for (size_t i = 0; i < hash.length(); i += 2 )
		mesBytes[i / 2] = static_cast<unsigned char>(std::stoi(hash.substr(i,2), nullptr, 16));

	FILE* keyFileStream = fopen(keyfile.c_str(), "r");
	if (!keyFileStream)
		throw std::runtime_error("Failed to open key file:" + keyfile);

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
	for (size_t i = 0; i < signature.length(); i += 2 )
		sigBytes[i / 2] = static_cast<unsigned char>(std::stoi(signature.substr(i,2), nullptr, 16));

	int verify = EVP_DigestVerify(mdctx, sigBytes.data(),sigBytes.size(), mesBytes.data(), mesBytes.size());
	EVP_MD_CTX_free(mdctx);
	EVP_PKEY_free(key);

	return verify == 1;
}
void pkgrepo::errors()
{
	ERR_print_errors_fp(stderr);
}
} // end of 'cards' namespace
