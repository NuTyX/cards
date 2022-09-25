/*
 * cards_package.h
 *
 * Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
 * Copyright 2017 - 2022 Thierry Nuttens <tnut@nutyx.org>
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

#include <libcards.h>


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
        std::string getCollection();
        std::string getName();
        std::string getVersion();
        std::string getPackager();
        std::string getDescription();

		void setCollection(std::string t);
        void setName(std::string t);
        void setVersion(std::string t);
        void setPackager(std::string t);
        void setDescription(std::string t);


        bool isInstalled();
        bool isToBeInstalled();
        bool isToBeRemoved();
        void setStatus(CPSTATUS pstatus);
        void unSetStatus(CPSTATUS pstatus);
        CPSTATUS getStatus();

    protected:
		pkginfo_t m_package;
        std::string m_name;
        CPSTATUS m_status;
    };
}
#endif
