/* SPDX-License-Identifier: LGPF-2.1-or-later */
#include "sync.h"

namespace cards {

const std::string sync::DEFAULT_PKG_REPO = ".REPO";
const std::string sync::DEFAULT_PKG_FILES = ".FILES";

sync::sync()
	:m_config("/etc/cards.conf")
{
    m_pkgRepoFile = DEFAULT_PKG_REPO;
    m_pkgFilesFile = DEFAULT_PKG_FILES;
    m_root = "/";
    m_configFile = "/etc/cards.conf";
}
sync::sync(const std::string configFile)
	: m_root("/")
	, m_configFile(configFile)
	, m_config(m_root + m_configFile)
{
    m_pkgRepoFile = DEFAULT_PKG_REPO;
    m_pkgFilesFile = DEFAULT_PKG_FILES;
}
void sync::purge()
{
	std::set<std::string> listOfFiles;
	for (auto collection : m_config.dirUrl()) {
		std::string path = collection.depot
			+ "/"
			+ collection.collection;

		if (collection.url.size() == 0 )
			continue;

		findRecursiveFile(listOfFiles, path.c_str(),WS_DEFAULT);
		for (auto file : listOfFiles)
			remove(file.c_str());
	}
}
void sync::run() 
{
	for (auto collection : m_config.dirUrl()) {
		if (collection.url.size() == 0 ) {
			continue;
		}
		cards::dwl key(collection.url
			+ "/"
			+ m_config.arch()
			+ "/"
			+ m_config.version()
			+ "/"
			+ collection.collection
			+ "/"
			+ PUBLICKEY,
			collection.depot + "/" + collection.collection,
			PUBLICKEY, false);

		cards::dwl repo(collection.url
			+ "/"
			+ m_config.arch()
			+ "/"
			+ m_config.version()
			+ "/"
			+ collection.collection
			+ "/"
			+ m_pkgRepoFile
			+ ".zst",
			collection.depot + "/" + collection.collection,
			m_pkgRepoFile + ".zst", false);

		uncompress(collection.depot + "/"
					+ collection.collection
					+ "/"
					+ m_pkgRepoFile);

		cards::dwl files(collection.url
			+ "/"
			+ m_config.arch()
			+ "/"
			+ m_config.version()
			+ "/"
			+ collection.collection
			+ "/"
			+ m_pkgFilesFile
			+ ".zst",
			collection.depot + "/" + collection.collection,
			m_pkgFilesFile + ".zst", false);

		uncompress(collection.depot + "/"
					+ collection.collection
					+ "/"
					+ m_pkgFilesFile);
	}
}
void sync::uncompress(const std::string fileName)
{
	std::ifstream inFile(fileName + ".zst", std::ios::binary);
	if (!inFile) {
	std::cerr << "Cannot open file: " << fileName << "\n";
	return;
	}

	inFile.seekg(0, std::ios::end);
	size_t compressSize = inFile.tellg();
	inFile.seekg(0, std::ios::beg);

	std::vector<char> compressData(compressSize);
	inFile.read(compressData.data(), compressSize);
	inFile.close();

	// Findout memory buffer
	size_t decompressSize = ZSTD_getFrameContentSize(compressData.data(), compressSize);
	if (decompressSize == ZSTD_CONTENTSIZE_UNKNOWN) {
		std::cerr << "Unknown size"
			<< std::endl;
		return;
	} else if (decompressSize == ZSTD_CONTENTSIZE_ERROR) {
		std::cerr << "Something wrong in the compress file:"
			<< fileName
			<< std::endl;
		return;
	}

	// Decompress
	std::vector<char> decompressData(decompressSize);
	size_t result = ZSTD_decompress(decompressData.data(), decompressSize, compressData.data(), compressSize);

	if (ZSTD_isError(result)) {
	std::cerr << "Error uncompressing: "
		<< ZSTD_getErrorName(result)
		<< std::endl;
	return;
	}

	// Write to file
	std::ofstream outFile(fileName, std::ios::binary|std::ios::out);
	if (!outFile)
		std::cerr << "Can't open file: "
		<< fileName
		<< std::endl;

	outFile.write(decompressData.data(),decompressData.size());

	outFile.close();
}

}
