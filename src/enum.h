/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

namespace cards {

enum ErrorEnum {
	ERROR_ENUM_CANNOT_FIND_DEPOT,
	ERROR_ENUM_CANNOT_DOWNLOAD_FILE,
	ERROR_ENUM_CANNOT_CREATE_FILE,
	ERROR_ENUM_CANNOT_OPEN_FILE,
	ERROR_ENUM_CANNOT_FIND_FILE,
	ERROR_ENUM_CANNOT_READ_FILE,
	ERROR_ENUM_CANNOT_COPY_FILE,
	ERROR_ENUM_CANNOT_PARSE_FILE,
	ERROR_ENUM_CANNOT_READ_DIRECTORY,
	ERROR_ENUM_CANNOT_CHANGE_DIRECTORY,
	ERROR_ENUM_CANNOT_WRITE_FILE,
	ERROR_ENUM_CANNOT_SYNCHRONIZE,
	ERROR_ENUM_CANNOT_RENAME_FILE,
	ERROR_ENUM_CANNOT_DETERMINE_NAME_BUILDNR,
	ERROR_ENUM_WRONG_ARCHITECTURE,
	ERROR_ENUM_EMPTY_PACKAGE,
	ERROR_ENUM_CANNOT_FORK,
	ERROR_ENUM_WAIT_PID_FAILED,
	ERROR_ENUM_DATABASE_LOCKED,
	ERROR_ENUM_CANNOT_LOCK_DIRECTORY,
	ERROR_ENUM_CANNOT_REMOVE_FILE,
	ERROR_ENUM_CANNOT_CREATE_DIRECTORY,
	ERROR_ENUM_CANNOT_RENAME_DIRECTORY,
	ERROR_ENUM_OPTION_ONE_ARGUMENT,
	ERROR_ENUM_INVALID_OPTION,
	ERROR_ENUM_OPTION_MISSING,
	ERROR_ENUM_TOO_MANY_OPTIONS,
	ERROR_ENUM_ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE,
	ERROR_ENUM_PACKAGE_IN_USE,
	ERROR_ENUM_PACKAGE_NOT_EXIST,
	ERROR_ENUM_PACKAGE_NOT_FOUND,
	ERROR_ENUM_PACKAGE_ALLREADY_INSTALL,
	ERROR_ENUM_PACKAGE_NOT_INSTALL,
	ERROR_ENUM_PACKAGE_NOT_PREVIOUSLY_INSTALL,
	ERROR_ENUM_LISTED_FILES_ALLREADY_INSTALLED,
	ERROR_ENUM_PKGADD_CONFIG_LINE_TOO_LONG,
	ERROR_ENUM_PKGADD_CONFIG_WRONG_NUMBER_ARGUMENTS,
	ERROR_ENUM_PKGADD_CONFIG_UNKNOWN_ACTION,
	ERROR_ENUM_PKGADD_CONFIG_UNKNOWN_EVENT,
	ERROR_ENUM_CANNOT_COMPILE_REGULAR_EXPRESSION,
	ERROR_ENUM_NOT_IN_CHROOT,
	ERROR_ENUM_CANNOT_GENERATE_LEVEL,
	ERROR_ENUM_NOT_INSTALL_PACKAGE_NEITHER_PACKAGE_FILE,
	ERROR_ENUM_PRIVATE_KEY_PATH_NOT_DEFINE,
	ERROR_ENUM_PUBLIC_KEY_NOT_EXIST,
	ERROR_ENUM_CANNOT_GENERATE_PRIVATE_KEY,
	ERROR_ENUM_CANNOT_SAVE_PRIVATE_KEY,
	ERROR_ENUM_CANNOT_CHMOD_PRIVATE_KEY_TO_600,
	ERROR_ENUM_CANNOT_SAVE_PUBLIC_KEY,
	ERROR_ENUM_WRONG_SIGNATURE
};

enum StatusEnum {
	STATUS_ENUM_UNKNOWN = 0x00,
	STATUS_ENUM_INSTALLED = 0x01,
	STATUS_ENUM_TO_INSTALL = 0x02,
	STATUS_ENUM_TO_REMOVE = 0x04,
	STATUS_ENUM_TO_UPGRADE = 0x08
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
enum rule_event_t {
	LDCONF,
	UPGRADE,
	INSTALL,
	INFO,
	ICONS,
	FONTS,
	SCHEMAS,
	DESKTOP_DB,
	MIME_DB,
	QUERY_PIXBUF,
	GIO_QUERY,
	QUERY_IMOD3,
	QUERY_IMOD2
};
enum archive_error {
    CANNOT_OPEN_ARCHIVE,
    CANNOT_READ_ARCHIVE,
    CANNOT_FIND_META_FILE,
    CANNOT_FIND_MTREE_FILE,
    CANNOT_FIND_NAME,
    CANNOT_FIND_ARCH,
    EMPTY_ARCHIVE
};

} // end of cards namespace

