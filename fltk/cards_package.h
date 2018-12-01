/*
 * cards_package.h
 *
 * Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#ifndef  CARDS_PACKAGE_H
#define  CARDS_PACKAGE_H

#include <cstddef>
#include <string>
#include <libcards.h>

using namespace std;

namespace cards
{
    class CWrapper;
    class CClient;

    class CPackage
    {
        friend CWrapper;
        friend CClient;
    public:
        CPackage();
        virtual ~CPackage(){}
        string getCollection();
        string getName();
        string getVersion();
        string getPackager();
        string getDescription();
        string getAlias();
        string getBase();
        string getGroup();
        string getUrl();
        string getContributor();
        string getRelease();
        string getBuildDate();
        string getFileNumber();
        string getArch();
        string getDependencies();
        string getSize();
        bool isInstalled();
        bool isToBeInstalled();
        bool isToBeRemoved();
        void setStatus(CPSTATUS pstatus);
        void unSetStatus(CPSTATUS pstatus);
        CPSTATUS getStatus();

    protected:
        string _collection;
        string _name;
        string _alias;
        string _version;
        string _packager;
        string _description;
        string _base;
        string _group;
        string _url;
        string _contributor;
        string _release;
        string _build_date;
        string _number_files;
        string _arch;
        string _dependencies;
        string _size;
        CPSTATUS _status;
    };
}
#endif
