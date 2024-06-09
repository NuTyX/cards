#pragma once

#include "file_utils.h"
#include "pkg.h"
#include "conf.h"

namespace cards {

typedef std::map<std::string, cards::pkg> ports_t;

struct dirName {
    std::string dir;
    std::string name;
};

class pkgfile {

        std::string m_configFileName;
        cards::conf m_config;
        ports_t m_listOfPackages;
        std::vector<dirName> m_listOfPackagesNames;
        void parseCollectionDirectory();
        /**
        * parse the "Pkgfile" file for each Package
        *
        */
        void parsePackagePkgfileFile();


    public:
        pkgfile(const std::string& fileName);
        virtual ~pkgfile() { }

        /**
        * return the version of the port name
        *
        */
        std::string getPortVersion (const std::string& portName);

        /**
        * return the release of the port name
        *
        */
        unsigned short int getPortRelease(const std::string& portName);

        /**
        * return true if port name exist
        *
        */ 
        bool checkPortExist(const std::string& portName);

        /**
         * return the list of found Packages names
         *
         */
        std::vector<dirName> getListOfPackagesNames();
        
        /**
         * return the list of found Packages
         *
         */
        ports_t getListOfPackages();
        
};

} //end of 'cards' namespace
