/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "archive.h"

archive::archive(const std::string& fileName)
    : m_fileName(fileName)
{
    extractFileContent(m_contentMtree,MTREEFILE);
    extractFileContent(m_contentMeta,METAFILE);

    if (m_contentMeta.size() == 0) {
        m_actualError = cards::CANNOT_FIND_META_FILE;
        treatErrors(m_fileName);
    }
    if (m_contentMtree.size() == 0) {
        m_actualError = cards::CANNOT_FIND_MTREE_FILE;
        treatErrors(m_fileName);
    }
    m_packageName = getPackageName();
    m_packageArch = getPackageArch();
    for (auto fileName : m_contentMtree) {
        m_filesList.insert(fileName);
    }
}
archive::~archive()
{
}
void archive::treatErrors(const std::string& message) const
{
    using namespace cards;
    switch (m_actualError) {
    case CANNOT_OPEN_ARCHIVE:
        throw std::runtime_error(gettext("could not open the archive ") + message);
        break;
    case CANNOT_READ_ARCHIVE:
        throw std::runtime_error(gettext("could not read the archive ") + message);
        break;
    case CANNOT_FIND_META_FILE:
        throw std::runtime_error(gettext("Invalid meta data in file ") + message);
        break;
    case CANNOT_FIND_MTREE_FILE:
        throw std::runtime_error(gettext("Invalid mtree in file ") + message);
        break;
    case EMPTY_ARCHIVE:
        throw std::runtime_error(message + gettext(" is not an archive"));
        break;
    case CANNOT_FIND_NAME:
        throw std::runtime_error(message + gettext(" is not a CARDS archive"));
    case CANNOT_FIND_ARCH:
        throw std::runtime_error(gettext("archive ") + message + gettext(": invalid architecture"));
        break;
    }
}
std::set<std::string> archive::setofFiles()
{
    return m_filesList;
}
unsigned int long archive::size()
{
    return m_contentMtree.size();
}
void archive::list()
{
    if (m_contentMtree.size() == 0) {
        std::cout << gettext("Not found") << std::endl;
    } else {
        for (auto i : m_contentMtree) {
            std::cout << i << std::endl;
        }
    }
}
void archive::extractFileContent
    (std::vector<std::string>& list
    ,const char* fileName)
{
    struct archive* ar;
    struct archive_entry* ae;

    ar = archive_read_new();
    INIT_ARCHIVE(ar);
    if (archive_read_open_filename(ar,
            m_fileName.c_str(),
            DEFAULT_BYTES_PER_BLOCK)
        != ARCHIVE_OK) {
        m_actualError = cards::CANNOT_OPEN_ARCHIVE;
        treatErrors(m_fileName);
    }
    unsigned int i = 0;
    int64_t entry_size;
    while (archive_read_next_header(ar, &ae) == ARCHIVE_OK) {
        const char* currentFile = archive_entry_pathname(ae);
        if (strcmp(currentFile, fileName) == 0) {
            entry_size = archive_entry_size(ae);
            char* fC = (char*)malloc(entry_size);
            archive_read_data(ar, fC, entry_size);
            fC[entry_size-1] = '\0';
            list = parseDelimitedVectorList(fC,'\n');
            free(fC);
            break;
        }
    }
    archive_read_close(ar);
    FREE_ARCHIVE(ar);
}
void archive::getRunTimeDependenciesEpoch()
{
    std::pair<std::string, time_t> NameEpoch;
    for (auto dependency : m_contentMeta) {
        if (dependency[0] == 'R') {
            NameEpoch.first = dependency.substr(1, dependency.size() - 11);
            NameEpoch.second = strtoul((dependency.substr(dependency.size() - 10)).c_str(), NULL, 0);
            m_rtDependenciesEpochList.insert(NameEpoch);
        }
    }
}
void archive::getRunTimeDependencies()
{
    for (auto dependency : m_contentMeta) {
        if (dependency[0] == RUNTIME_DEPENDENCY) {
            m_rtDependenciesList.insert(dependency.substr(1));
        }
    }
}
void archive::getAliasList()
{
    for (auto i : m_contentMeta) {
        if (i[0] == ALIAS) {
            m_aliasList.insert(i.substr(1));
        }
    }
}
void archive::getCategoriesList()
{
    for (auto i : m_contentMeta) {
        if (i[0] == CATEGORIES) {
            m_categoriesList.insert(i.substr(1));
        }
    }
}
void archive::getSetsList()
{
    for (auto i : m_contentMeta) {
        if (i[0] == SETS) {
            m_setsList.insert(i.substr(1));
        }
    }
}
void archive::printDeps()
{
    getRunTimeDependencies();
    for (auto i : m_rtDependenciesList)
        std::cout << i << " ";
}
const std::vector<std::string>& archive::contentMeta() {
    return m_contentMeta;
}
void archive::printMeta()
{
    for (auto i : m_contentMeta)
        std::cout << i << std::endl;
}
std::string archive::getPackageName()
{
    if (m_contentMeta.size() == 0) {
        m_actualError = cards::CANNOT_FIND_NAME;
        treatErrors(m_fileName);
        return "";
    }
    for (auto name : m_contentMeta) {
        if (name[0] == NAME) {
            return name.substr(1);
            break;
        }
    }
    m_actualError = cards::CANNOT_FIND_NAME;
    treatErrors(m_fileName);
    return "";
}
std::string archive::getPackageArch()
{
    if (m_contentMeta.size() == 0) {
        m_actualError = cards::CANNOT_FIND_ARCH;
        treatErrors(m_fileName);
        return "";
    }
    for (auto arch : m_contentMeta) {
        if (arch[0] == ARCHITECTURE) {
            return arch.substr(1);
            break;
        }
    }
    return "";
}
std::string archive::namebuildn()
{
    return getPackageName() + epochBuildDate();
}
std::set<std::string> archive::listofDependencies()
{
    m_rtDependenciesList.insert(namebuildn());
    getRunTimeDependencies();

    return m_rtDependenciesList;
}
std::set<std::string> archive::listofAlias()
{
    getAliasList();
    return m_aliasList;
}
std::set<std::string> archive::listofCategories()
{
    getCategoriesList();
    return m_categoriesList;
}
std::set<std::string> archive::listofSets()
{
    getSetsList();
    return m_setsList;
}
std::set<std::pair<std::string, time_t>> archive::listofDependenciesBuildDate()
{
    std::pair<std::string, time_t> NameEpoch;
    NameEpoch.first = getPackageName();
    NameEpoch.second = build();
    m_rtDependenciesEpochList.insert(NameEpoch);
    getRunTimeDependenciesEpoch();
    return m_rtDependenciesEpochList;
}
std::string archive::name()
{
    return m_packageName;
}
std::string archive::arch()
{
    return m_packageArch;
}
std::string archive::version()
{
    for (auto s : m_contentMeta) {
        if (s[0] == VERSION) {
            return s.substr(1);
            break;
        }
    }
    return "";
}
int archive::release()
{
    for (auto s : m_contentMeta) {
        if (s[0] == RELEASE) {
            return stoi(s.substr(1));
            break;
        }
    }
    return 0;
}
std::string archive::url()
{
    for (auto s : m_contentMeta) {
        if (s[0] == URL) {
            return s.substr(1);
            break;
        }
    }
    return "";
}
std::string archive::license()
{
    for (auto s : m_contentMeta) {
        if (s[0] == LICENSE) {
            return s.substr(1);
            break;
        }
    }
    return "";
}
std::string archive::description()
{
    for (auto s : m_contentMeta) {
        if (s[0] == DESCRIPTION) {
            return s.substr(1);
            break;
        }
    }
    return "";
}
std::string archive::group()
{
    for (auto s : m_contentMeta) {
        if (s[0] == GROUP) {
            return s.substr(1);
            break;
        }
    }
    return "";
}
std::string archive::maintainer()
{
    for (auto s : m_contentMeta) {
        if (s[0] == MAINTAINER) {
            return s.substr(1);
            break;
        }
    }
    return "";
}
std::string archive::contributors()
{
    for (auto s : m_contentMeta) {
        if (s[0] == CONTRIBUTORS) {
            return s.substr(1);
            break;
        }
    }
    return "";
}
std::string archive::packager()
{
    for (auto s : m_contentMeta) {
        if (s[0] == PACKAGER) {
            return s.substr(1);
            break;
        }
    }
    return "";
}
std::string archive::collection()
{
    for (auto s : m_contentMeta) {
        if (s[0] == COLLECTION) {
            return s.substr(1);
            break;
        }
    }
    return "";
}
std::string archive::builddate()
{
    char* c_time_s;
    std::string buildtime;
    for (auto bt : m_contentMeta) {
        if (buildtime[0] == BUILD) {
            buildtime = bt.substr(1);
            break;
        }
    }
    time_t ct = strtoul(buildtime.c_str(), nullptr, 0);
    c_time_s = ctime(&ct);
    std::string build = c_time_s;
    return build;
}
std::string archive::epochBuildDate()
{
    for (auto epochSVal : m_contentMeta) {
        if (epochSVal[0] == BUILD) {
            return epochSVal.substr(1);
            break;
        }
    }
    return "";
}
time_t archive::build()
{
    time_t epochVal = 0;
    for (auto epochSVal : m_contentMeta) {
        if (epochSVal[0] == BUILD) {
            epochVal = strtoul((epochSVal.substr(1)).c_str(), nullptr, 0);
            return epochVal;
            break;
        }
    }
    return 0;
}
