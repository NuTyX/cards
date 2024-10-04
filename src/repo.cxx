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
    unsigned int i = 0;
    if ((config.keypath().empty()) ||
        (!checkFileExist(keyfile))) {
        for (auto pkg : m_pkgrepo.getListOfPackages()) {
            i++;
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
            << i
            << " packages checked"
            << std::endl;
    }
}
}
