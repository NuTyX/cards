//
//  enum.h
//
//  Copyright (c) 2023 by NuTyX team (http://nutyx.org)
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
//  USA.
//

 #pragma once
namespace cards {

enum StatusEnum {
	STATUS_ENUM_UNKNOWN,
	STATUS_ENUM_INSTALLED,
	STATUS_ENUM_TO_INSTALL,
	STATUS_ENUM_TO_REMOVE,
	STATUS_ENUM_TO_UPGRADE
};
enum ActionEnum {
	ACTION_ENUM_PKG_DOWNLOAD_START,
	ACTION_ENUM_PKG_DOWNLOAD_RUN,
	ACTION_ENUM_PKG_DOWNLOAD_END,
	ACTION_ENUM_DB_OPEN_START,
	ACTION_ENUM_DB_OPEN_RUN,
	ACTION_ENUM_DB_OPEN_END,
	ACTION_ENUM_PKG_PREINSTALL_START,
	ACTION_ENUM_PKG_PREINSTALL_END,
	ACTION_ENUM_PKG_INSTALL_START,
	ACTION_ENUM_PKG_INSTALL_END,
	ACTION_ENUM_PKG_INSTALL_RUN,
	ACTION_ENUM_PKG_POSTINSTALL_START,
	ACTION_ENUM_PKG_POSTINSTALL_END,
	ACTION_ENUM_PKG_MOVE_META_START,
	ACTION_ENUM_PKG_MOVE_META_END,
	ACTION_ENUM_DB_ADD_PKG_START,
	ACTION_ENUM_DB_ADD_PKG_END,
	ACTION_ENUM_LDCONFIG_START,
	ACTION_ENUM_LDCONFIG_END,
	ACTION_ENUM_RM_PKG_FILES_START,
	ACTION_ENUM_RM_PKG_FILES_RUN,
	ACTION_ENUM_RM_PKG_FILES_END
};
enum RuleEnum {
	RULE_ENUM_LDCONF,
	RULE_ENUM_UPGRADE,
	RULE_ENUM_INSTALL,
	RULE_ENUM_INFO,
	RULE_ENUM_ICONS,
	RULE_ENUM_FONTS,
	RULE_ENUM_SCHEMAS,
	RULE_ENUM_DESKTOP_DB,
	RULE_ENUM_MIME_DB,
	RULE_ENUM_QUERY_PIXBUF,
	RULE_ENUM_GIO_QUERY,
	RULE_ENUM_QUERY_IMOD3,
	RULE_ENUM_QUERY_IMOD2
};

} // end of cards namespace

// vim:set ts:2 :
