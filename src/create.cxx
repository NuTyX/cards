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
    m_tree = m_pkgfile.getListOfPackages();

    std::string packageName = argParser.otherArguments()[0];

    if (m_config.logdir() != "") {
        if (!createRecursiveDirs(m_config.logdir())) {
            m_actualError = cards::ERROR_ENUM_CANNOT_CREATE_DIRECTORY;
            treatErrors(m_config.logdir());
        }
        std::string logFile = m_config.logdir() + "/" + packageName + ".log";
        unlink(logFile.c_str());
        m_fdlog = open(logFile.c_str(), O_APPEND | O_WRONLY | O_CREAT, 0666);
        if (m_fdlog == -1) {
            m_actualError = cards::ERROR_ENUM_CANNOT_OPEN_FILE;
            treatErrors(logFile);
        }
    }

    
}
create::~create()
{
    if (m_fdlog != -1)
	    close(m_fdlog);
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
    std::string message;
    std::string commandName = "pkgadd ";

    cards::pkgrepo pkgrepo("/etc/cards.conf");

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
        buildSimpleDatabase();
        for (auto i : m_dependencies) {
            std::string name;
            if ( i == packageName)
                break;
            for (auto packageFile : pkgrepo.getListOfPackagesFromGroup(i)) {
                archive packageArchive(packageFile);

                if (packageArchive.group().empty()) {
                    if (!checkPackageNameExist(packageArchive.name())) {
                        commandName = "pkgadd ";
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
                                commandName = "pkgadd ";
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
void create::build(std::string packageName)
{
    list(packageName);
    installDependencies(packageName);

    if (m_pkgfile.getPortDir(packageName).empty()) {
        m_actualError = cards::ERROR_ENUM_PACKAGE_NOT_FOUND;
        treatErrors(packageName);
    }

    m_portsDir += "/";
    m_portsDir += m_pkgfile.getPortDir(packageName);

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

    if (chdir(m_portsDir.c_str())) {
        m_actualError = cards::ERROR_ENUM_CANNOT_CHANGE_DIRECTORY;
        treatErrors(m_portsDir);
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
        buildSimpleDatabase();
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
void create::parseArguments()
{
    if (getuid()) {
        m_actualError = cards::ERROR_ENUM_ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
        treatErrors("");
    }
    if (m_root == "")
        m_root = "/";
}
}
