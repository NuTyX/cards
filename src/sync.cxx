/* SPDX-License-Identifier: LGPF-2.1-or-later */
#include "sync.h"

namespace cards {

const std::string sync::DEFAULT_PKG_REPO = ".REPO";
const std::string sync::DEFAULT_PKG_FILES = ".FILES";

sync::sync(const CardsArgumentParser& argParser,
	const std::string& configFileName)
	: m_argParser(argParser)
	, m_root("/")
	, m_configFile(configFileName)
	, m_config(m_root + m_configFile)
{
    m_pkgRepoFile = DEFAULT_PKG_REPO;
    m_pkgFilesFile = DEFAULT_PKG_FILES;
	if (!getuid())
		run();

}

void sync::run() 
{
	bool progress = true;
	if (m_argParser.isSet(CardsArgumentParser::OPT_NO_PROGRESS))
		progress = false;

	fileToDownload destinationFile;
	std::vector<fileToDownload> destinationFiles;
	for (auto collection : m_config.dirUrl()) {
		if (collection.url.size() == 0 )
			continue;
		destinationFile.url = collection.url
			+ "/"
			+ m_config.arch()
			+ "/"
			+ m_config.version()
			+ "/"
			+ collection.collection
			+ "/";
		destinationFile.dirName = collection.depot
			+ "/"
			+ collection.collection
			+ "/";
		destinationFile.fileName = PUBLICKEY;
		destinationFiles.push_back(destinationFile);

		destinationFile.fileName = m_pkgRepoFile;
		destinationFiles.push_back(destinationFile);

		destinationFile.fileName = m_pkgFilesFile;
		destinationFiles.push_back(destinationFile);
	}

	cards::dwl(destinationFiles, progress);

	for (auto file : destinationFiles) {
		if (file.fileName == PUBLICKEY )
			continue;

		uncompress(file.dirName + file.fileName);
	}
	if (progress)
		std::cout << std::endl;
}
void sync::uncompress(const std::string& fileName)
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
