#include "create.h"

namespace cards {

create::create(CardsArgumentParser& argParser)
    : pkgfile("/etc/cards.conf")
    , pkgadd("cards create")
    , m_argParser(argParser)
    , m_tree(getListOfPackages())
    , m_config("/etc/cards.conf")
{
    parseArguments();
    
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
    }
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
    if (!m_dependencies.empty()) {
        
        pkgadd add("cards add");
        for (auto i : m_dependencies) {
            std::string name = basename(const_cast<char*>(i.c_str()));
            if ( name == packageName)
                break;
            std::set<std::string> listofBinaries;
            if (findDir(listofBinaries, i) != 0) {
            m_actualError = cards::ERROR_ENUM_CANNOT_READ_DIRECTORY;
                treatErrors(i);
            }
            for (auto j : listofBinaries) {
                if ( j.find(".cards-") == std::string::npos )
                    continue;
                packageArchiveName = i + "/" + j;
                archive packageArchive(packageArchiveName.c_str());
                if ( add.checkPackageNameExist(packageArchive.name()) )
                    continue;
                if ( m_config.groups().empty() ) {
                    add.run(packageArchiveName);
                    continue;
                }
                if ( packageArchive.group() == "" ) {
                    add.run(packageArchiveName);
                    continue;
                }
                for (auto k : m_config.groups()) {
                    if ( packageArchive.group() == k ) {
                        add.run(packageArchiveName);
                        continue;
                    }
                }
            }
        }
    }
}
void create::build(std::string packageName)
{
    list(packageName);
    installDependencies(packageName);
    std::cout << "create of " << packageName << std::endl;
    std::string pkgdir = getPortDir(packageName);
    if (pkgdir == "") {
        m_actualError = cards::ERROR_ENUM_PACKAGE_NOT_FOUND;
        treatErrors(packageName);
    }
    std::string timestamp;
    std::string commandName = "cards create: ";
    std::string message;
    std::string packageFileName;
    int fdlog = -1;

    if (m_config.logdir() != "") {
        if (!createRecursiveDirs(m_config.logdir())) {
            m_actualError = cards::ERROR_ENUM_CANNOT_CREATE_DIRECTORY;
            treatErrors(m_config.logdir());
        }
        std::string logFile = m_config.logdir() + "/" + packageName + ".log";
        unlink(logFile.c_str());
        fdlog = open(logFile.c_str(), O_APPEND | O_WRONLY | O_CREAT, 0666);
        if (fdlog == -1) {
            m_actualError = cards::ERROR_ENUM_CANNOT_OPEN_FILE;
            treatErrors(logFile);
        }
    }
    message = commandName + pkgdir + " package(s)";
    std::cout << message << std::endl;

    if (m_config.logdir() != "") {
        write(fdlog, message.c_str(), message.length());
        time_t startTime;
        time(&startTime);
        timestamp = ctime(&startTime);
        timestamp = " starting build " + timestamp;
        write(fdlog, timestamp.c_str(), timestamp.length());
        write(fdlog, "\n", 1);
    }

    chdir(pkgdir.c_str());

    std::string runscriptCommand = "sh";
    std::string cmd = "pkgmk";
    std::string args = "-d ";

    process makeprocess(cmd, args, fdlog);
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

    if (m_config.logdir() != "") {
        time_t endTime;
        time(&endTime);
        timestamp = ctime(&endTime);
        timestamp = commandName + "build done " + timestamp;

        write(fdlog, "\n", 1);
        write(fdlog, timestamp.c_str(), timestamp.length());
        write(fdlog, "\n", 1);
    }

    cards::pkgrepo pkgrepo("/etc/cards.conf");

    for (auto packageFile : pkgrepo.getListOfPackagesFromGroup(packageName)) {
        archive packageArchive(packageFile);
        std::string name = packageArchive.name();
        std::string version = packageArchive.version();
        message = "CREATED: " + name + " " + version;
        m_upgrade = 0;
        buildSimpleDatabase();
		if (pkgdbh::checkPackageNameExist(name)) {
			message = name + ": is ALLREADY installed";
			m_upgrade = 1;
		}
        m_packageArchiveName = packageFile;
        run();
        std::cout << message << std::endl;
        if (m_config.logdir() != "") {
            write( fdlog, message.c_str(), message.length());
            write( fdlog, "\n", 1 );
        }
    }
	if ( m_config.logdir() != "" ) {
		time_t finishTime;
		time(&finishTime);
		timestamp = ctime(&finishTime);
		timestamp = commandName + "finish " + timestamp;
		write( fdlog, "\n", 1 );
		write( fdlog, timestamp.c_str(), timestamp.length());
		write( fdlog, "\n", 1 );
		close ( fdlog );
	}

}
void create::parseArguments()
{
    if (getuid()) {
        m_actualError = cards::ERROR_ENUM_ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
        treatErrors("");
    }
}
}
