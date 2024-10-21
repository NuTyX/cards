/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#define _ gettext

#define INIT_ARCHIVE(ar)                     \
    archive_read_support_filter_gzip((ar));  \
    archive_read_support_filter_bzip2((ar)); \
    archive_read_support_filter_xz((ar));    \
    archive_read_support_filter_zstd((ar));  \
    archive_read_support_format_tar((ar))
#define FREE_ARCHIVE(ar) \
    archive_read_free((ar))

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"
#define WHITE "\033[1;37m"
#define NORMAL "\033[1;0m"

#define HELP_ACTION _("\033[1;33maction:\033[1;0m")
#define HELP_USAGE _("\033[1;33musage: \033[1;0m")
#define HELP_OPTIONS _("\033[1;33moptions\033[1;0m")
#define HELP_REQUIRED _("\033[1;33mrequired\033[1;0m")
#define HELP_DESCRIPTION _("\033[1;33mdescription: \033[1;0m")
#define HELP_COMMAND _("\033[1;34mcommand\033[1;0m")

#define CARDS_CONF_FILE "/etc/cards.conf"
#define NUTYX_VERSION_FILE "/var/lib/pkg/nutyx-version"
#define PRIVATEKEY "/.private.key"
#define PUBLICKEY "/.PKEY"

#define GIGA 1e9
#define MEGA 1e6
#define KILO 1e3
#define PACKAGE_LOCALE_DIR "/usr/share/locale"
#define GETTEXT_PACKAGE "cards"

#define uint8 unsigned char
#define uint32 unsigned long int

#define DEFAULT_BYTES_PER_BLOCK (20 * 512)
#define METAFILE ".META"
#define INFOFILE ".INFO"
#define MTREEFILE ".MTREE"

#define PKG_DB_DIR "var/lib/pkg/DB/"

#define PKG_FILES "/files"
#define PKG_META "/META"
#define PKG_RECEPT "/Pkgfile"
#define PKG_REPO "/.REPO"
#define PKG_README "README"
#define PKG_PRE_INSTALL ".PRE"
#define PKG_POST_INSTALL ".POST"

// Package Attributes
#define NAME 'N'
#define VERSION 'V'
#define RELEASE 'r'
#define SIZE_I 'S'
#define ARCHITECTURE 'a'
#define DESCRIPTION  'D'
#define URL 'U'
#define CONTRIBUTORS 'C'
#define MAINTAINER 'M'
#define PACKAGER 'P'
#define LICENSE 'L'
#define BUILD 'B'
#define COLLECTION 'c'
#define GROUP 'g'
#define RUNTIME_DEPENDENCY 'R'
#define SHARED_LIB 'l'
#define ALIAS 'A'
#define SETS 's'
#define CATEGORIES 'T'
#define HASHSUM 'H'
#define SIGNATURE 'I'
#define SIZE 'i'


#define PKG_REJECTED "var/lib/pkg/rejected"
#define PKGADD_CONF "var/lib/pkg/pkgadd.conf"
#define PKGADD_CONF_MAXLINE 1024

#define LDCONFIG "sbin/ldconfig"
#define LDCONFIG_CONF "etc/ld.so.conf"
#define LDCONFIG_CONF_ARGS "-r "
#define SHELL "bin/sh"

// /usr/bin/install-info --info-dir="/usr/share/info" /usr/share/info/<file>.info"
#define INSTALL_INFO "usr/bin/install-info"
#define INSTALL_INFO_ARGS "--info-dir=usr/share/info "

// /usr/bin/gtk-update-icon-cache -f -t /usr/share/icons/hicolor
#define UPDATE_ICON "usr/bin/gtk-update-icon-cache"
#define UPDATE_ICON_ARGS "-f -t "

// /usr/bin/glib-compile-schemas /usr/share/glib-2/schemas
#define COMPILE_SCHEMAS "usr/bin/glib-compile-schemas"
#define COMPILE_SCHEMAS_ARGS ""

// /usr/bin/update-desktop-database -q /usr/share/applications
#define UPDATE_DESKTOP_DB "usr/bin/update-desktop-database"
#define UPDATE_DESKTOP_DB_ARGS "-q "

// /usr/bin/update-mime-database usr/share/mime
#define UPDATE_MIME_DB "usr/bin/update-mime-database"
#define UPDATE_MIME_DB_ARGS "-n "

// /usr/bin/gdk-pixbuf-query-loaders --update-cache
#define GDK_PIXBUF_QUERY_LOADER "usr/bin/gdk-pixbuf-query-loaders"
#define GDK_PIXBUF_QUERY_LOADER_ARGS "--update-cache"

// /usr/bin/gio-querymodules /usr/lib/gio/modules
#define GIO_QUERYMODULES "usr/bin/gio-querymodules"
#define GIO_QUERYMODULES_ARGS "usr/lib/gio/modules"

// /usr/bin/gtk-query-immodules-3.0 --update-cache
#define QUERY_IMMODULES_3 "usr/bin/gtk-query-immodules-3.0"
#define QUERY_IMMODULES_3_ARGS "--update-cache"

// /usr/bin/gtk-query-immodules-2.0 --update-cache
#define QUERY_IMMODULES_2 "usr/bin/gtk-query-immodules-2.0"
#define QUERY_IMMODULES_2_ARGS "--update-cache"

// /usr/bin/mkfontdir /usr/share/fonts/<dir>/
#define MKFONTDIR "usr/bin/mkfontdir"
#define MKFONTDIR_ARGS ""

// /usr/bin/mkfontscale /usr/share/fonts/<dir>/
#define MKFONTSCALE "usr/bin/mkfontscale"
#define MKFONTSCALE_ARGS ""

// /usr/bin/fc-cache /usr/share/fonts/<dir>/
#define FC_CACHE "usr/bin/fc-cache"
#define FC_CACHE_ARGS ""

#define MAX_BUFFER 50
#define MARGIN 20

