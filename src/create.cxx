#include "create.h"

namespace cards {

create::create(CardsArgumentParser& argParser)
    : pkgfile("/etc/cards.conf")
    , m_argParser(argParser)
    , m_tree(getListOfPackages())
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
void create::build(std::string packageName)
{
    cards::conf config("/etc/cards.conf");
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

    if (config.logdir() != "") {
        if (!createRecursiveDirs(config.logdir())) {
            m_actualError = cards::ERROR_ENUM_CANNOT_CREATE_DIRECTORY;
            treatErrors(config.logdir());
        }
        std::string logFile = config.logdir() + "/" + packageName + ".log";
        unlink(logFile.c_str());
        fdlog = open(logFile.c_str(), O_APPEND | O_WRONLY | O_CREAT, 0666);
        if (fdlog == -1) {
            m_actualError = cards::ERROR_ENUM_CANNOT_OPEN_FILE;
            treatErrors(logFile);
        }
    }
    message = commandName + pkgdir + " package(s)";
    std::cout << message << std::endl;

    if (config.logdir() != "") {
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

    if (config.logdir() != "") {
        time_t endTime;
        time(&endTime);
        timestamp = ctime(&endTime);
        timestamp = commandName + "build done " + timestamp;

        write(fdlog, "\n", 1);
        write(fdlog, timestamp.c_str(), timestamp.length());
        write(fdlog, "\n", 1);
    }

    std::set<std::string> listOfPackages;
    if (findDir(listOfPackages, pkgdir) != 0) {
        m_actualError = cards::ERROR_ENUM_CANNOT_READ_DIRECTORY;
        treatErrors(pkgdir);
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
