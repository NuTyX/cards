/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "repo.h"

namespace cards {

repo::repo(const CardsArgumentParser& argParser,
            const char *configFileName)
            : m_argParser(argParser)
            , m_configFileName(configFileName)
            , m_pkgrepo(configFileName)
{
    cards::conf config(m_configFileName);
    std::string keyfile(config.keypath());
	keyfile += PRIVATEKEY;
    if ((config.keypath().empty()) ||
        (!checkFileExist(keyfile))) {
        for (auto pkg : m_pkgrepo.getListOfPackages()) {
            if (!m_pkgrepo.checkSign(pkg.first)) {
                std::cout << "Package "
                    << pkg.first
                    << ": Wrong signature !!!"
                    << std::endl;
                break;
            }
            rotatingCursor();
        }
        std::cout << "All "
            << m_pkgrepo.getListOfPackages().size()
            << " packages checked"
            << std::endl;
    } else {
        struct stat buf;
        std::string::size_type pos;

        for (auto dir :config.dirUrl()) {
            std::set<std::string> listOfFiles;
            std::string path = dir.depot
                + "/"
                + dir.collection;
            std::string repofile = path + PKG_REPO_META;
            std::cout << "Generating "
                << repofile
                << std::endl;

            FILE* fp = fopen(repofile.c_str(),"w");
            if (!fp) {
                std::cout << "Cannot open "
                    << repofile
                    << std::endl;
                return;
            }
            findRecursiveFile(listOfFiles,path.c_str(),WS_DEFAULT);
            for (auto file : listOfFiles) {
                archive a(file);
                if (stat(file.c_str(), &buf) != 0) {
                    std::cerr << "Cannot find size of " << file << std::endl;
                    return;
                }

                m_pkgrepo.generateHash(file);
                pos = file.rfind( '/' );
                if (pos != std::string::npos)
                    file = '@'
                        + file.substr( pos + 1 );

                file += "\n";
                fwrite(file.c_str(),1, file.size(),fp);
                for (auto meta : a.contentMeta()) {
                    meta += "\n";
                    fwrite(meta.c_str(),1,meta.size(),fp);
                }

                m_pkgrepo.generateSign();
                file = HASHSUM
                    + m_pkgrepo.hash()
                    + "\n";
                fwrite(file.c_str(),1, file.size(),fp);

                file = SIGNATURE
                    + m_pkgrepo.sign()
                    + "\n";
                fwrite(file.c_str(),1, file.size(),fp);

                file = SIZE
                    + itos(buf.st_size)
                    + "\n";
                fwrite(file.c_str(),1, file.size(),fp);

                file = "\n";
                fwrite(file.c_str(),1, file.size(),fp);

                rotatingCursor();
            }
            fclose(fp);
            std::cout << std::endl;
        }
        std::cout << "All "
            << m_pkgrepo.getListOfPackages().size()
            << " packages signed"
            << std::endl;
    }

}
}
