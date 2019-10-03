/*
 * cards_package.cxx
 *
 * Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
 * Copyright 2019 by NuTyX team (http://nutyx.org)
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
        _status = (CPSTATUS) 0;
    }

    /// Return
    string CPackage::getSetList()
    {
        return m_set;
    }

    string CPackage::getName()
    {
        return _name;
    }

    string CPackage::getVersion()
    {
        return _version;
    }

    string CPackage::getPackager()
    {
        return _packager;
    }

    string CPackage::getDescription()
    {
        return _description;
    }

    bool CPackage::isInstalled()
    {
        return _status & CPSTATUS::INSTALLED;
    }

    bool CPackage::isToBeInstalled()
    {
        return _status & CPSTATUS::TO_INSTALL;
    }

    bool CPackage::isToBeRemoved()
    {
        return _status & CPSTATUS::TO_REMOVE;
    }

    void CPackage::setStatus(CPSTATUS pstatus)
    {
        _status = (CPSTATUS)(_status | pstatus);
    }

    void CPackage::unSetStatus(CPSTATUS pstatus)
    {
        _status = (CPSTATUS)(_status & (~pstatus));
    }

    CPSTATUS CPackage::getStatus()
    {
        return _status;
    }
}
