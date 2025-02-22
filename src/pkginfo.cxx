/* SPDX-License-Identifier: LGPL-2.1-or-later */
//  Copyright (c) 2000 - 2005 Per Liden
//  Copyright (c) 2006 - 2013 by CRUX team (https://crux.nu)
//  Copyright (c) 2013 - .... by NuTyX team (https://nutyx.org)

#include "pkginfo.h"

pkginfo::pkginfo(const std::string& commandName)
    : pkgdbh(commandName)
    , m_runtimedependencies_mode(0)
    , m_runtimelibs_mode(0)
    , m_footprint_mode(0)
    , m_archiveinfo(0)
    , m_metainfo(0)
    , m_installed_mode(0)
    , m_number_mode(0)
    , m_list_mode(0)
    , m_owner_mode(0)
    , m_details_mode(0)
    , m_libraries_mode(0)
    , m_runtime_mode(0)
    , m_epoc(0)
    , m_fulllist_mode(false)
{
}
pkginfo::pkginfo()
    : pkgdbh("pkginfo")
    , m_runtimedependencies_mode(0)
    , m_footprint_mode(0)
    , m_archiveinfo(0)
    , m_metainfo(0)
    , m_installed_mode(0)
    , m_list_mode(0)
    , m_owner_mode(0)
    , m_details_mode(0)
    , m_libraries_mode(0)
    , m_runtime_mode(0)
    , m_epoc(0)
    , m_fulllist_mode(true)
{
}
void pkginfo::parseArguments(int argc, char** argv)
{
    for (int i = 1; i < argc; ++i) {
        std::string option(argv[i]);
        if (option == "-r" || option == "--root") {
            assertArgument(argv, argc, i);
            m_root = argv[i + 1] + m_root;
            i++;
        } else if (option == "-i" || option == "--installed") {
            m_installed_mode += 1;
        } else if (option == "-n" || option == "--number") {
            m_number_mode += 1;
        } else if (option == "-d" || option == "--details") {
            assertArgument(argv, argc, i);
            m_details_mode += 1;
            m_arg = argv[i + 1];
            i++;
        } else if (option == "-l" || option == "--list") {
            assertArgument(argv, argc, i);
            m_list_mode += 1;
            m_arg = argv[i + 1];
            i++;
        } else if (option == "-R" || option == "--runtimedep") {
            assertArgument(argv, argc, i);
            m_runtime_mode += 1;
            m_arg = argv[i + 1];
            i++;
        } else if (option == "-o" || option == "--owner") {
            assertArgument(argv, argc, i);
            m_owner_mode += 1;
            m_arg = argv[i + 1];
            i++;
        } else if (option == "-a" || option == "--archive") {
            assertArgument(argv, argc, i);
            m_archiveinfo += 1;
            m_arg = argv[i + 1];
            i++;
        } else if (option == "-m" || option == "--meta") {
            assertArgument(argv, argc, i);
            m_metainfo += 1;
            m_arg = argv[i + 1];
            i++;
        } else if (option == "-f" || option == "--footprint") {
            assertArgument(argv, argc, i);
            m_footprint_mode += 1;
            m_arg = argv[i + 1];
            i++;
        } else if (option == "-L" || option == "--libraries") {
            assertArgument(argv, argc, i);
            m_libraries_mode += 1;
            m_arg = argv[i + 1];
            i++;
        } else if (option == "--runtimedepfiles") {
            assertArgument(argv, argc, i);
            m_runtimedependencies_mode += 1;
            m_arg = argv[i + 1];
            i++;
        } else if (option == "--runtimelibs") {
            assertArgument(argv, argc, i);
            m_runtimelibs_mode += 1;
            m_arg = argv[i + 1];
            i++;

        } else if (option == "-b" || option == "--buildtime") {
            assertArgument(argv, argc, i);
            m_epoc += 1;
            m_arg = argv[i + 1];
            i++;
        } else {
            m_actualError = cards::ERROR_ENUM_INVALID_OPTION;
            treatErrors(option);
        }
    }
    if (m_runtimedependencies_mode
            + m_runtimelibs_mode
            + m_footprint_mode
            + m_details_mode
            + m_installed_mode
            + m_list_mode
            + m_owner_mode
            + m_epoc
            + m_archiveinfo
            + m_metainfo
            + m_footprint_mode
            + m_libraries_mode
            + m_runtime_mode
            + m_number_mode
        == 0) {
        m_actualError = cards::ERROR_ENUM_OPTION_MISSING;
        treatErrors(m_arg);
    }
    if (m_runtimedependencies_mode
            + m_runtimelibs_mode
            + m_footprint_mode
            + m_installed_mode
            + m_archiveinfo
            + m_list_mode
            + m_owner_mode
            + m_number_mode
        > 1) {
        m_actualError = cards::ERROR_ENUM_TOO_MANY_OPTIONS;
        treatErrors(m_arg);
    }
}
void pkginfo::list(const std::string& packageName)
{
    m_list_mode = 1;
    m_arg = packageName;
}
void pkginfo::installed()
{
    m_installed_mode = 1;
}
void pkginfo::query(const std::string& fileName)
{
    m_owner_mode = 1;
    m_arg = fileName;
}
void pkginfo::details(const std::string& packageName)
{
    m_details_mode = 1;
    m_arg = packageName;
}
void pkginfo::run()
{
    if (m_metainfo) {
        archive package(m_arg);
        package.printMeta();
    }
    if (m_archiveinfo) {
        archive package(m_arg);
        std::string name = package.name() + " : ";
        std::cout
            << name << _("Description    : ") << package.description() << std::endl
            << name << _("Group          : ") << package.group() << std::endl
            << name << _("URL            : ") << package.url() << std::endl
            << name << _("License        : ") << package.license() << std::endl
            << name << _("Contributor(s) : ") << package.contributors() << std::endl
            << name << _("Packager       : ") << package.packager() << std::endl
            << name << _("Version        : ") << package.version() << std::endl
            << name << _("Release        : ") << package.release() << std::endl
            << name << _("Architecture   : ") << package.arch() << std::endl
            << name << _("Build date     : ") << package.build() << std::endl;
        if (package.listofDependencies().size() > 0) {
            std::cout << name << _("Dependencies   : ");
            package.printDeps();
            std::cout << std::endl;
        }

    }
    /*
     *  Make footprint
     */
    if (m_footprint_mode) {
        getFootprintPackage(m_arg);
    } else if (m_number_mode) {
        buildDatabase(false);
        std::cout << m_listOfPackages.size() << std::endl;
    } else {
        /*
         *  Modes that require the database to be opened
         *
         */
        cards::lock Lock(m_root, false);

        if (m_installed_mode) {
            /*
             *  List installed packages
             */
            buildDatabase(false);
            for (auto i : m_listOfPackages) {
                if (m_fulllist_mode) {
                    if (i.second.dependency() == true)
                        std::cout << "AUTO: ";
                    else
                        std::cout << " MAN: ";

                    std::cout << "(" << i.second.collection() << ")"
                              << " " << i.first << " "
                              << i.second.version() << "-" << i.second.release() << std::endl;
                } else {
                    if (i.second.dependency() == true)
                        continue;
                    std::cout << "(" << i.second.collection() << ")"
                              << " " << i.first << " "
                              << i.second.version() << "-" << i.second.release() << std::endl;
                }
            }
        } else if (m_list_mode) {
            /*
             *  List package or file contents
             */
            if (checkFileExist(m_arg)) {
                archive package(m_arg);
                for (auto f : package.setofFiles())
                    std::cout << f << std::endl;
            } else {
                buildDatabase(true);
                if (!checkPackageNameExist(m_arg)) {
                    m_actualError = cards::ERROR_ENUM_NOT_INSTALL_PACKAGE_NEITHER_PACKAGE_FILE;
                    treatErrors(m_arg);
                }
                if (checkPackageNameExist(m_arg)) {
                    std::string arg = m_listOfAlias[m_arg];
                    for (auto f : m_listOfPackages[arg].files)
                        std::cout << f << std::endl;
                }
            }

        } else if (m_runtimelibs_mode) {
            buildDatabase(false);
            int Result;
            std::set<std::string> filenameList;
            Result = findRecursiveFile(filenameList, m_arg.c_str(), WS_DEFAULT);
            /*
             * get the list of libraries for all the possible files
             */
            std::set<std::string> librariesList;
            for (auto i : filenameList)
                Result = getRuntimeLibrariesList(librariesList, i);

            for (auto i : librariesList)
                std::cout << i << std::endl;

            std::cout<< std::endl;
        } else if (m_runtimedependencies_mode) {
            /* 	Get runtimedependencies of the file found in the directory path
                    get the list of installed packages silently */
            buildDatabase(true);
            int Result;
            std::set<std::string> filenameList;
            Result = findRecursiveFile(filenameList, m_arg.c_str(), WS_DEFAULT);

            // get the list of libraries for all the possible files
            std::set<std::string> librariesList;
            for (auto i : filenameList)
                Result = getRuntimeLibrariesList(librariesList, i);

            // Get the own package for all the elf files dependencies libraries
            if ((librariesList.size() > 0) && (Result > -1)) {
                std::set<std::string> runtimeList;
                for (auto i : librariesList) {
                    for (auto j : m_listOfPackages) {
                        bool found = false;
                        for (auto k : j.second.files) {
                            if (k.find('/' + i) != std::string::npos) {
                                std::string dependency = j.first + ultos(j.second.build());
                                runtimeList.insert(dependency);
                                break;
                                found = true;
                            }
                        }
                        if (found == true) {
                            found = false;
                            break;
                        }
                    }
                }
                if (runtimeList.size() > 0) {
                    unsigned int s = 1;
                    for (auto i : runtimeList) {
                        std::cout << i << std::endl;
                        s++;
                    }
                    std::cout << std::endl;
                }
            }
        } else if (m_libraries_mode + m_runtime_mode > 0) {
            // Get the list of installed packages silently
            buildDatabase(true);
            std::set<std::string> librariesList;
            int Result = -1;
            std::cout << "Librairies"<< std::endl;

            if (checkPackageNameExist(m_arg)) {
                for (auto i : m_listOfPackages[m_arg].files) {
                    std::string filename('/' + i);
                    Result = getRuntimeLibrariesList(librariesList, filename);
                }
                if ((librariesList.size() > 0) && (Result > -1)) {
                    if (m_runtime_mode) {
                        std::set<std::string> runtimeList;
                        for (auto i : librariesList) {
                            for (auto j : m_listOfPackages) {
                                bool found = false;
                                for (auto k : j.second.files) {
                                    if (k.find('/' + i) != std::string::npos) {
                                        runtimeList.insert(j.first);
                                        break;
                                        found = true;
                                    }
                                }
                                if (found == true) {
                                    found = false;
                                    break;
                                }
                            }
                        }
                        if (runtimeList.size() > 0) {
                            unsigned int s = 1;
                            for (auto i : runtimeList) {
                                std::cout << i;
                                s++;
                                if (s <= runtimeList.size())
                                    std::cout << ",";
                            }
                            std::cout << std::endl;
                        }
                    } else {
                        for (auto i : librariesList)
                            std::cout << i << std::endl;
                    }
                }
            }
        } else if (m_epoc) {
            //  get the buildtime of the package: return 0 if not found
            buildDatabase(false);
            if (checkPackageNameExist(m_arg)) {
                std::cout << m_listOfPackages[m_arg].build() << std::endl;
            } else {
                std::cout << "0" << std::endl;
            }
        } else if (m_details_mode) {
            // get all details of a package
            buildDatabase(false);
            if (checkPackageNameExist(m_arg)) {
                std::string arg = m_listOfAlias[m_arg];
                std::cout << _("Name           : ") << arg << std::endl
                          << _("Description    : ") << m_listOfPackages[arg].description() << std::endl;
                if (m_listOfPackages[arg].alias().size() > 0) {
                    std::cout << _("Alias          :");
                    for (auto a : m_listOfPackages[arg].alias())
                        std::cout << " "
                                  << a;
                    std::cout << std::endl;
                }
                if (m_listOfPackages[arg].categories().size() > 0) {
                    std::cout << _("Categories     :");
                    for (auto c : m_listOfPackages[arg].categories())
                        std::cout << " "
                                  << c;
                    std::cout << std::endl;
                }
                if (m_listOfPackages[arg].sets().size() > 0) {
                    std::cout << _("Sets           :");
                    for (auto s : m_listOfPackages[arg].sets())
                        std::cout << " "
                                  << s;
                    std::cout << std::endl;
                }
                if (m_listOfPackages[arg].group().size() > 0)
                    std::cout << _("Group          : ")
                        << m_listOfPackages[arg].group()
                        << std::endl;

                std::cout << _("Collection     : ") << m_listOfPackages[arg].collection() << std::endl
                          << _("URL            : ") << m_listOfPackages[arg].url() << std::endl
                          << _("License        : ") << m_listOfPackages[arg].license() << std::endl
                          << _("Contributor(s) : ") << m_listOfPackages[arg].contributors() << std::endl
                          << _("Packager       : ") << m_listOfPackages[arg].packager() << std::endl
                          << _("Version        : ") << m_listOfPackages[arg].version() << std::endl
                          << _("Release        : ") << m_listOfPackages[arg].release() << std::endl
                          << _("Build date     : ") << getDateFromEpoch(m_listOfPackages[arg].build()) << std::endl
                          << _("Installed date : ") << getDateFromEpoch(m_listOfPackages[arg].install()) << std::endl
                          << _("Installed Size : ") << sizeHumanRead(m_listOfPackages[arg].space())
                          << _("bytes") << std::endl
                          << _("Arch           : ") << m_listOfPackages[arg].arch() << std::endl;
                if (m_listOfPackages[m_arg].dependency() == false)
                    std::cout << _("Man. installed : Yes");
                else
                    std::cout << _("Man. installed : No");
                std::cout << std::endl;
                if (m_listOfPackages[m_arg].dependencies().size() > 0) {
                    std::cout << _("Dependencies   : ");
                    for (auto i : m_listOfPackages[arg].dependencies())
                        std::cout << i.first << " ";
                    std::cout << std::endl;
                }
            }
        } else if (m_owner_mode) {
            // List owner(s) of file or directory
            buildDatabase(true);
            regex_t preg;
            if (regcomp(&preg, m_arg.c_str(), REG_EXTENDED | REG_NOSUB)) {
                m_actualError = cards::ERROR_ENUM_CANNOT_COMPILE_REGULAR_EXPRESSION;
                treatErrors(m_arg);
            }
            struct info
            {
                std::string collection;
                std::string file;
            };
            info Info;
            std::vector<std::pair<std::string, info>> result;
            Info.collection=_("Collection");
            Info.file=_("File");
            result.push_back(std::pair<std::string, info>(_("Package"),Info));
            unsigned int width1 = result.begin()->second.collection.length(); // Width of "Collection"
            unsigned int width2 = result.begin()->first.length(); // Width of "Package"

            for (auto i : m_listOfPackages) {
                for (auto j : i.second.files) {
                    const std::string file('/' + j);
                    if (!regexec(&preg, file.c_str(), 0, 0, 0)) {
                        info Info;
                        Info.collection=i.second.collection();
                        Info.file=j;
                        result.push_back(std::pair<std::string, info>(i.first, Info));
                        if (i.second.collection().length() > width1) {
                            width1 = i.second.collection().length();
                        }
                        if (i.first.length() > width2) {
                            width2 = i.first.length();
                        }
                    }
                }
            }

            regfree(&preg);
            if (result.size() > 1) {
                for (auto i : result) {
                    std::cout << std::left
                        << std::setw(width1 + 2)
                        << i.second.collection
                        << std::setw(width2 + 2)
                        << i.first
                        << i.second.file
                        << std::endl;
                }
            } else {
                std::cout << m_utilName << _(": no owner(s) found") << std::endl;
            }
        }
    }
}
void pkginfo::finish()
{
}
void pkginfo::printHelp() const
{
    std::cout << HELP_USAGE << m_utilName << " [options]" << std::endl
              << HELP_OPTIONS << std::endl
              << "  -i, --installed             "
              << _("list of installed packages") << std::endl
              << "  -n, --number                "
              << _("number of installed packages") << std::endl
              << "  -d, --details               "
              << _("list details about the <package>") << std::endl
              << "  -L, --libraries             "
              << _("list all the runtime libraries for the <package>") << std::endl
              << "  -l, --list <package|file>   "
              << _("list files in <package> or <file>") << std::endl
              << "  -m, --meta                  "
              << _("print metainfo datas for the repository") << std::endl
              << "  -o, --owner <pattern>       "
              << _("list owner(s) of file(s) matching <pattern>") << std::endl
              << "  -f, --footprint <file>      "
              << _("print footprint for <file>") << std::endl
              << "  -a, --archive <file>        "
              << _("print Name, Version, Release, BuildDate and Deps of the <file>") << std::endl
              << "  -b, --buildtime <package>   "
              << _("return the build time of the package") << std::endl
              << "  -R, --runtimedep <package>  "
              << _("return on a single line all the runtime dependencies") << std::endl
              << "  --runtimedepfiles <path>    "
              << _("return on a single line all the runtime dependencies for the files found in the <path>") << std::endl
              << "  --runtimelibs <path>    "
              << _("return all the runtime libraries for the files found in the <path>") << std::endl
              << "  -r, --root <path>           "
              << _("specify alternative installation root") << std::endl
              << "  -v, --version               "
              << _("print version and exit") << std::endl
              << "  -h, --help                  "
              << _("print help and exit") << std::endl;
}
