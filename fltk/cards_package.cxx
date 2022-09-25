/*
 * cards_package.cxx
 *
 * Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
 * Copyright 2017 - 2022  Thierry Nuttens <tnut@nutyx.org>
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

#include "cards_package.h"

namespace cards
{
    /// Constructor
    CPackage::CPackage()
    {
        m_status = (CPSTATUS) 0;
    }
    void CPackage::setCollection(std::string t)
    {
		m_package.collection=t;
	}
    void CPackage::setName(std::string t)
    {
		m_name=t;
	}
    void CPackage::setDescription(std::string t)
    {
		m_package.description=t;
	}
    void CPackage::setVersion(std::string t)
    {
		m_package.version=t;
	}
    void CPackage::setPackager(std::string t)
    {
		m_package.packager=t;
	}

    /// Return
    std::string CPackage::getCollection()
    {
        return m_package.collection;
    }

    std::string CPackage::getName()
    {
        return m_name;
    }

    std::string CPackage::getVersion()
    {
        return m_package.version;
    }

    std::string CPackage::getPackager()
    {
        return m_package.packager;
    }

    std::string CPackage::getDescription()
    {
        return m_package.description;
    }

    bool CPackage::isInstalled()
    {
        return m_status & CPSTATUS::INSTALLED;
    }

    bool CPackage::isToBeInstalled()
    {
        return m_status & CPSTATUS::TO_INSTALL;
    }

    bool CPackage::isToBeRemoved()
    {
        return m_status & CPSTATUS::TO_REMOVE;
    }

    void CPackage::setStatus(CPSTATUS pstatus)
    {
        m_status = (CPSTATUS)(m_status | pstatus);
    }

    void CPackage::unSetStatus(CPSTATUS pstatus)
    {
        m_status = (CPSTATUS)(m_status & (~pstatus));
    }

    CPSTATUS CPackage::getStatus()
    {
        return m_status;
    }
}
