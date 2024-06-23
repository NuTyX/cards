/* SPDX-License-Identifier: LGPL-2.1-or-later */
//  Copyright (c) 2002 by Johannes Winkelmann
//  Copyright (c) 2013 - .... by NuTyX team (https://nutyx.org)

#pragma once

#include "md5.h"
#include "string_utils.h"
#include "conf.h"

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
int findDir(std::set<std::string>& filesList, const std::string& basedir);
int findDir(itemList *filenameList, const char *path);
int findRecursiveFile(std::set<std::string>& filenameList, const char *filename, int spec);
int readFileStripSpace(itemList *fileContent, const char *fileName);
int readFile(itemList *fileContent, const char *fileName);
int parseFile(std::set<std::string>& fileContent, const char* fileName);
int parseFile(std::vector<std::string>& fileContent, const char* fileName);
int parseFile(std::string& Depends, const char* key, const char* fileName);
bool findMD5sum(const std::string& fileName, unsigned char* result);
bool checkMD5sum(const char * fileName, const char * MD5Sum);
