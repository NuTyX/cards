#include "create.h"

namespace cards {

create::create(CardsArgumentParser& argParser)
    : pkgadd("cards create")
    , m_pkgfile("/etc/cards.conf")
    , m_argParser(argParser)
    , m_config("/etc/cards.conf")
    , m_fdlog(-1)
{
    char pwd[MAXPATHLEN];
    m_portsDir = getcwd(pwd,MAXPATHLEN);

    parseArguments();
    if (!checkFileExist(m_root + "tmp/setup")) {
        m_actualError = cards::ERROR_ENUM_NOT_IN_CHROOT;
        treatErrors(m_root + "tmp/setup");
    }

    m_tree = m_pkgfile.getListOfPackages();

    if (isACollection())
        buildCollection();
    else
        buildBinary(m_argParser.otherArguments()[0]);
}
create::~create()
{
    if (m_fdlog != -1)
	    close(m_fdlog);
}
void create::base()
{
    bool found = false;
    std::vector<std::string> list;
    buildDatabase(false, true);
    for ( auto i : m_listOfPackages) {
        for (auto j : i.second.sets()) {
            if (j == "chroot")
                found = true;
            if (j == "devel")
                found = true;
        }
        if (!found) {
            list.push_back(i.first);
        }
        found = false;
    }
    cards::lock Lock(m_root, true);
    for (auto i : list) {
        m_packageName = i;

        // Remove metadata about the package removed
        removePackageFilesRefsFromDB(m_packageName);

        // Remove the files on hd
        removePackageFiles(m_packageName);

    }
}
bool create::isACollection()
{
    DIR* d;
    struct dirent* dir;
    d = opendir(m_argParser.otherArguments()[0].c_str());
    if (d)
        return true;

    return false;
}
void create::parseArguments()
{
    if (getuid()) {
        m_actualError = cards::ERROR_ENUM_ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
        treatErrors("");
    }
    if (m_root == "")
        m_root = "/";
}
void create::treatErrors(const std::string& s) const
{
    switch (m_actualError) {
    case ERROR_ENUM_ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE:
        throw std::runtime_error(s + _(" only root can install / upgrade / remove packages"));
        break;
    case ERROR_ENUM_PACKAGE_NOT_FOUND:
        throw std::runtime_error(_("The package ") + s + _(" is not found"));
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
    case ERROR_ENUM_CANNOT_CHANGE_DIRECTORY:
        throw RunTimeErrorWithErrno(_("could not change directory ") + s);
        break;
    case ERROR_ENUM_NOT_IN_CHROOT:
        throw RunTimeErrorWithErrno(_("You are not in chroot mode. The file ") + s
        + _(" is not found"));
        break;
    }
}
void create::checkBinaries()
{
    cards::pkgrepo pkgrepo("/etc/cards.conf");
    cards::pkgfile pkgfile("/etc/cards.conf");

    for (auto i : pkgrepo.getListOfPackagesFromCollection(m_argParser.otherArguments()[0])) {
        if (pkgfile.checkPackageNameExist(i)) {
            continue;
        } else {
            std::cout << "Package: "
                << i
                << " package should be removed and delete from the .REPO file."
                << std::endl;
            if (m_argParser.isSet(CardsArgumentParser::OPT_DRY))
                std::cout << "Remove : "
                    << pkgrepo.dirName(i)
                    << "/"
                    << pkgrepo.fileName(i)
                    << std::endl;
            else
                removeFile(pkgrepo.dirName(i),pkgrepo.fileName(i));
        }
    }
    // TODO
    // When all the obsolets binaries are remove
    // We need to clean up the meta data
    // probably firing cards repo sometime in the process
    // with the collection as argument would be the easiest way
}
void create::list(std::string& packageName)
{
    auto found = false;
    for (auto i : m_list) {
        if (packageName == i) {
            found = true;
        }
    }
    if (!found) {
        m_list.insert(packageName);
        if (m_tree[packageName].dependencies().size() > 0) {
            for (auto n : m_tree[packageName].dependencies()) {
                for (auto i : m_list)
                    if (n == packageName)
                        break;
                list(n);
            }
        }
    }
}
void create::installDependencies(std::string& packageName)
{
    std::string message, commandName;
    cards::pkgrepo pkgrepo("/etc/cards.conf");

    m_dependencies.clear();
    auto level = m_tree[packageName].level();
    unsigned int currentLevel = 0;
    std::string packageArchiveName;
    while (currentLevel < level) {
        for (auto i : m_list) {
            if (m_tree[i].level() == currentLevel)
                m_dependencies.push_back(i);
        }
        ++currentLevel;
    }
    if (! m_dependencies.empty()) {
        for (auto i : m_dependencies) {
            if ( i == packageName)
                break;
            for (auto packageFile : pkgrepo.getListOfPackagesFromGroup(i)) {
                archive packageArchive(packageFile);

                if (packageArchive.group().empty()) {
                    if (!checkPackageNameExist(packageArchive.name())) {
                        commandName = "ADD: ";
                        message = commandName + packageFile;
                        m_packageArchiveName = packageFile;
                        if (!m_fdlog != -1) {
                            write(m_fdlog, message.c_str(), message.length());
                            write(m_fdlog, "\n", 1);
                        }
                        run();
                    }
                } else {
                    for (auto k : m_config.groups()) {
                        if (packageArchive.group() == k ) {
                            if (!checkPackageNameExist(packageArchive.name())) {
                                commandName = "ADD: ";
                                message = commandName + packageFile;
                                m_packageArchiveName = packageFile;
                                if (!m_fdlog != -1) {
                                    write(m_fdlog, message.c_str(), message.length());
                                    write(m_fdlog, "\n", 1);
                                }
                                run();
                            }
                        }
                    }
                }
           }
        }
    }
}
void create::buildCollection()
{
    checkBinaries();
    cards::pkgrepo pkgrepo("/etc/cards.conf");
    cards::pkgfile pkgfile("/etc/cards.conf");
    for (auto i : pkgfile.getListOfPackagesFromCollection(m_argParser.otherArguments()[0])) {
        if (!pkgrepo.checkBinaryExist(i)) {
            std::cout << i
                << " ===> SHOULD BE BUILD !"
                << std::endl;

            if (m_argParser.isSet(CardsArgumentParser::OPT_DRY))
                continue;
            buildBinary(i);
            continue;
        }
        if (!checkFileExist(pkgrepo.dirName(i)
            + "/"
            + pkgrepo.fileName(i))) {
            std::cout << i
                << " ===> SHOULD BE BUILD !"
                << std::endl;

            if (m_argParser.isSet(CardsArgumentParser::OPT_DRY))
                continue;
            buildBinary(i);
            continue;
        }
        if (pkgrepo.version(i) != pkgfile.getPortVersion(i)) {
            std::cout << i
                <<": Binary: "
                << pkgrepo.version(i)
                << ", Port: "
                << pkgfile.getPortVersion(i)
                << " ===> SHOULD BE BUILD !"
                << std::endl;

            if (m_argParser.isSet(CardsArgumentParser::OPT_DRY))
                continue;
            buildBinary(i);
            continue;
        }
        if (pkgrepo.release(i) != pkgfile.getPortRelease(i)) {
            std::cout << i
                << ": Binary: "
                << pkgrepo.version(i)
                << "-"
                << pkgrepo.release(i)
                << ", Port: "
                << pkgfile.getPortVersion(i)
                << "-"
                << pkgfile.getPortRelease(i)
                << " ===> SHOULD BE BUILD !"
                << std::endl;

            if (m_argParser.isSet(CardsArgumentParser::OPT_DRY))
                continue;
            buildBinary(i);
            continue;
        }
        // Last chance for build the i package
        // We check if runtime libs are installed, then
        // we check if all the runtime libs are still existing.
        // We search through all the packages per level
        // And for each packages we search through each librairies
        // for a matching one.
        bool found = true;
        std::string missingSharedLib;
        for (auto lib : pkgrepo.getLibs(i)) {
            found = false;
            int level = 0;
            while (level < pkgfile.getLevel(i)) {
                for (auto pkg : pkgfile.getListOfPackages()) {
                    if (!pkgrepo.checkBinaryExist(pkg.first))
                        continue;
                    if (pkg.second.level() != level)
                        continue;
                    for (auto deplib : pkgrepo.getLibs(pkg.first) ) {
                        if (deplib == lib) {
                            found = true;
                            break;
                        }
                    }
                    if (found)
                        break;
                }
                if (found)
                    break;
                level++;
            }
            if (!found) {
                if (checkFileNameExist(lib)) {
                    found = true;
                }
                if (!found) {
                    missingSharedLib = lib;
                    break;
                }
            }
        }
        if (!found) {
            std::cout << "==> MAYBE NEEDS TO BE REBUILD "
                << i
                << ": "
                << missingSharedLib
                << " SHARE LIBRARY is not found !!! "
                << std::endl;
        } else {
            if (m_argParser.isSet(CardsArgumentParser::OPT_DRY))
                continue;
            std::cout << "WILL NOT BUILD "
                << i
                << " AGAIN..."
                << std::endl;
        }
    }
}
void create::buildBinary(std::string packageName)
{
    if (m_config.logdir() != "") {
        if (!createRecursiveDirs(m_config.logdir())) {
            m_actualError = cards::ERROR_ENUM_CANNOT_CREATE_DIRECTORY;
            treatErrors(m_config.logdir());
        }
        std::string logFile = m_config.logdir()
            + "/"
            + packageName
            + ".log";

        unlink(logFile.c_str());
        m_fdlog = open(logFile.c_str(), O_APPEND | O_WRONLY | O_CREAT, 0666);
        if (m_fdlog == -1) {
            m_actualError = cards::ERROR_ENUM_CANNOT_OPEN_FILE;
            treatErrors(logFile);
        }
    }

    std::string timestamp, message;
    std::string commandName = "cards create: ";

    message = commandName + packageName + " package(s)";
    std::cout << message << std::endl;

    if (m_fdlog != -1) {
        write(m_fdlog, message.c_str(), message.length());
        time_t startTime;
        time(&startTime);
        timestamp = ctime(&startTime);
        timestamp = " starting build " + timestamp;
        write(m_fdlog, timestamp.c_str(), timestamp.length());
        write(m_fdlog, "\n", 1);
    }

    base();
    m_list.clear();
    list(packageName);
    installDependencies(packageName);

    if (m_pkgfile.getPortDir(packageName).empty()) {
        m_actualError = cards::ERROR_ENUM_PACKAGE_NOT_FOUND;
        treatErrors(packageName);
    }

    std::string pkgdir = m_portsDir
        + "/"
        + m_pkgfile.getPortDir(packageName);

    if (chdir(pkgdir.c_str())) {
        m_actualError = cards::ERROR_ENUM_CANNOT_CHANGE_DIRECTORY;
        treatErrors(pkgdir);
    };

    std::string runscriptCommand = "sh";
    std::string cmd = "pkgmk";
    std::string args = "-d ";

    process makeprocess(cmd, args, m_fdlog);
    int result = 0;
    result = makeprocess.executeShell();
    std::string s = RED;
    switch (result) {
    case 1:
        s += "General error";
        break;
    case 2:
        s += "Invalid Pkgfile";
        break;
    case 3:
        s += "Sources / build directory missing or missing read/write permission";
        break;
    case 4:
        s += "Download of sources file(s)";
        break;
    case 5:
        s += "Unpacking of sources file(s)";
        break;
    case 6:
        s += "Checking md5sum from sources";
        break;
    case 7:
        s += "Checking footprint";
        break;
    case 8:
        s += "While running 'build()'";
        break;
    case 10:
        s += "Searching runtime dependancies";
        break;
    case 11:
        s += "Duplicate alias";
        break;
    }

    if (result > 0) {
        s += NORMAL;
        m_actualError = cards::ERROR_ENUM_CANNOT_PARSE_FILE;
        treatErrors("Pkgfile: " + s);
    }

    if (m_fdlog != -1) {
        time_t endTime;
        time(&endTime);
        timestamp = ctime(&endTime);
        timestamp = commandName
            + "build done "
            + timestamp;

        write(m_fdlog, "\n", 1);
        write(m_fdlog, timestamp.c_str(), timestamp.length());
        write(m_fdlog, "\n", 1);
    }

    cards::pkgrepo pkgrepo("/etc/cards.conf");

    for (auto packageFile : pkgrepo.getListOfPackagesFromGroup(packageName)) {
        archive packageArchive(packageFile);
        std::string name = packageArchive.name();
        std::string version = packageArchive.version();
        message = "CREATED: "
            + name
            + " "
            + version;
        m_upgrade = 0;

		if (checkPackageNameExist(name)) {
			message = name + ": is ALLREADY installed";
			m_upgrade = 1;
		}
        m_packageArchiveName = packageFile;
        run();
        std::cout << message << std::endl;
        if (m_config.logdir() != "") {
            write(m_fdlog, message.c_str(), message.length());
            write(m_fdlog, "\n", 1 );
        }
    }
	if ( m_fdlog != -1 ) {
		time_t finishTime;
		time(&finishTime);
		timestamp = ctime(&finishTime);
		timestamp = commandName + "finish " + timestamp;
		write(m_fdlog, "\n", 1 );
		write(m_fdlog, timestamp.c_str(), timestamp.length());
		write(m_fdlog, "\n", 1 );
	}

}
}
