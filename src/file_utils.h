//
//  file_utils.h
// 
//  Copyright (c) 2002 by Johannes Winkelmann
//  Copyright (c) 2013-2015 by NuTyX team (http://nutyx.org)
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

#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include "md5.h"
#include "string_utils.h"

#include <utime.h>
#include <dirent.h>
#include <sys/param.h>
#include <sys/file.h>
#include <unistd.h>
#include <libgen.h>
#include <err.h>

#define S_CARDS_MODE  0755

#define WS_NONE         0
#define WS_RECURSIVE    (1 << 0)
#define WS_DEFAULT      WS_RECURSIVE
#define WS_FOLLOWLINK   (1 << 1)        /* follow symlinks */
#define WS_DOTFILES     (1 << 2)        /* per unix convention, .file is hidden */
#define WS_MATCHDIRS    (1 << 3)        /* if pattern is used on dir names too */

enum {
        WALK_OK = 0,
        WALK_BADPATTERN,
        WALK_NAMETOOLONG,
        WALK_BADIO
};

struct InfoFile
{
	std::string url;
	std::string filename;
	std::string dirname;
	std::string md5sum;
	long int filetime;
	utimbuf acmodtime;
	FILE *stream;
};

struct DirUrl {
	std::string Dir;
	std::string Url;
};

struct Config {
	Config() {}
	std::string hostname;
	std::string database;
	std::string username;
	std::string password;
	std::string socket;
	std::string collection;
	std::string name;
	std::string version;
	std::string arch;
	std::string logdir;
	std::vector<std::string> locale;
	std::vector<DirUrl> dirUrl;
	std::vector<std::string> baseDir;
	std::vector<std::string> archs;
};

int getConfig(const char *fileName, Config& config);
void * getDatas ( void * var, FILE * file, long offset, size_t size, size_t nmemb);
std::string trimFileName(const std::string& filename);
time_t getEpochModifyTimeFile(const std::string& filename);
std::string getDateFromEpoch(const time_t& epoch);
std::string getModifyTimeFile(const std::string& filename);
bool checkFileExist(const std::string& filename);
bool checkFileEmpty(const std::string& filename);
bool checkRegularFile(const std::string& filename);
bool checkFileSignature(const std::string& filename, const std::string& signature);
bool checkFilesEqual(const std::string& file1, const std::string& file2);
bool checkPermissionsEqual(const std::string& file1, const std::string& file2);
bool createRecursiveDirs(const std::string& pathname);
void cleanupMetaFiles(const std::string& basedir);
void removeFile(const std::string& basedir, const std::string& filename);
int copyFile(const char *  destFile, const char *  origFile);
int findFile(std::set<std::string>& filesList, const std::string& basedir);
int findDir(itemList *filenameList, const char *path);
int findRecursiveFile(std::set<std::string>& filenameList, const char *filename, int spec);
int readFileStripSpace(itemList *fileContent, const char *fileName);
int readFile(itemList *fileContent, const char *fileName);
int parseFile(std::set<std::string>& fileContent, const char* fileName);
int parseFile(std::vector<std::string>& fileContent, const char* fileName);
bool findMD5sum(const std::string& fileName, unsigned char* result);
bool checkMD5sum(const char * fileName, const char * MD5Sum);
/* read a file
   return it into a template container */
template <class T>
int parseFile( T& target, const char* fileName)
{
	FILE *fp = fopen (fileName, "r");
	if (!fp)
		return -1;
	const int length = BUFSIZ;
	char input[length];
	std::string line;
	while (fgets(input, length, fp)) {
		input[strlen(input)-1] = '\0';
		line = input;
		target.push_back(line);
	}
	fclose(fp);
	return 0;
}


#endif /* FILE_UTILS_H */
// vim:set ts=2 :
