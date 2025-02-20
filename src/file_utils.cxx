/* SPDX-License-Identifier: LGPL-2.1-or-later */

//  Copyright (c) 2000 - 2005 Per Liden
//  Copyright (c) 2006 - 2013 by CRUX team (https://crux.nu)
//  Copyright (c) 2013 - .... by NuTyX team (https://nutyx.org)

#include "file_utils.h"

void * getDatas ( void * var, FILE * file, long offset, size_t size, size_t nmemb)
{
  void * mvar;
  if (size == 0 || nmemb == 0)
    return nullptr;
  if (fseek (file, offset, SEEK_SET))
  {
    std::cerr << "Cannot seek" << std::endl;
    return nullptr;
  }
  mvar = var;
  if (mvar == nullptr)
  {
  /* Check for overflow.  */
    if (nmemb < (~(size_t) 0 - 1) / size)
      /* + 1 so that we can '\0' terminate invalid std::string table sections.  */
      mvar = malloc (size * nmemb + 1);
    if (mvar == nullptr)
    {
      std::cerr << "Out of memory" << std::endl;
      return nullptr;
    }
    ((char *) mvar)[size * nmemb] = '\0';
  }
  if (fread (mvar, size, nmemb, file) != nmemb)
  {
    printf("Unable to read in 0x%lx bytes", (unsigned long)(size * nmemb));
    return nullptr;
  }
  return mvar;
}

std::string trimFileName(const std::string& filename)
{
  std::string search("//");
  std::string result = filename;

  for (std::string::size_type pos = result.find(search); pos != std::string::npos; pos = result.find(search))
    result.replace(pos, search.size(), "/");

  return result;
}
time_t getEpochModifyTimeFile(const std::string& filename)
{
	struct stat buf;
	if (stat(filename.c_str(), &buf) != 0) {
		return 0;
	}
	return buf.st_mtime;
}
std::string getDateFromEpoch(const time_t& epoch)
{
	const struct tm* time = localtime(&epoch);
	char datetime[100] = {0};
	if ( strftime(datetime, 100, "%c", time) > 0 )
		return datetime;
	else
		return "";
}
std::string getModifyTimeFile(const std::string& filename)
{
	std::string sTimeFile = "";
	char * c_time_s;
	struct stat buf;
	if (stat(filename.c_str(), &buf) != 0) {
		return sTimeFile;
	}
	return getDateFromEpoch(buf.st_mtime);
}
bool checkFileExist(const std::string& filename)
{
  struct stat buf;
  return !lstat(filename.c_str(), &buf);
}
bool checkFileEmpty(const std::string& filename)
{
  struct stat buf;

  if (lstat(filename.c_str(), &buf) == -1)
    return false;

  return (S_ISREG(buf.st_mode) && buf.st_size == 0);
}
bool checkRegularFile(const std::string& filename)
{
	struct stat buf;
	if (!checkFileExist(filename))
		return false;
	stat(filename.c_str(), &buf);
	return (S_ISREG(buf.st_mode));

}
bool checkFilesEqual(const std::string& file1, const std::string& file2)
{
  struct stat buf1, buf2;
	const int length = BUFSIZ;

  if (lstat(file1.c_str(), &buf1) == -1)
    return false;

  if (lstat(file2.c_str(), &buf2) == -1)
    return false;

  // Regular files
  if (S_ISREG(buf1.st_mode) && S_ISREG(buf2.st_mode)) {
    std::ifstream f1(file1.c_str());
    std::ifstream f2(file2.c_str());

    if (!f1 || !f2)
      return false;

    while (!f1.eof()) {
      char buffer1[length];
      char buffer2[length];
      f1.read(buffer1, length);
      f2.read(buffer2, length);
      if (f1.gcount() != f2.gcount() ||
          memcmp(buffer1, buffer2, f1.gcount()) ||
          f1.eof() != f2.eof())
        return false;
    }

    return true;
  }
  // Symlinks
  else if (S_ISLNK(buf1.st_mode) && S_ISLNK(buf2.st_mode)) {
    char symlink1[MAXPATHLEN];
    char symlink2[MAXPATHLEN];

    memset(symlink1, 0, MAXPATHLEN);
    memset(symlink2, 0, MAXPATHLEN);

    if (readlink(file1.c_str(), symlink1, MAXPATHLEN - 1) == -1)
      return false;

    if (readlink(file2.c_str(), symlink2, MAXPATHLEN - 1) == -1)
      return false;

    return !strncmp(symlink1, symlink2, MAXPATHLEN);
  }
  // Character devices
  else if (S_ISCHR(buf1.st_mode) && S_ISCHR(buf2.st_mode)) {
    return buf1.st_dev == buf2.st_dev;
  }
  // Block devices
  else if (S_ISBLK(buf1.st_mode) && S_ISBLK(buf2.st_mode)) {
    return buf1.st_dev == buf2.st_dev;
  }

  return false;
}
bool checkPermissionsEqual(const std::string& file1, const std::string& file2)
{
  struct stat buf1;
  struct stat buf2;

  if (lstat(file1.c_str(), &buf1) == -1)
    return false;

  if (lstat(file2.c_str(), &buf2) == -1)
    return false;

  return(buf1.st_mode == buf2.st_mode) &&
    (buf1.st_uid == buf2.st_uid) &&
    (buf1.st_gid == buf2.st_gid);
}
void cleanupMetaFiles(const std::string& basedir)
{
	if ( checkFileExist( basedir + "/.INFO") ) {
#ifdef DEBUG
		std::cerr << basedir << "/.INFO removed" << std::endl;
#endif
		removeFile ( basedir, "/.INFO");
	}
	if ( checkFileExist( basedir + "/.META") ) {
#ifdef DEBUG
		std::cerr << basedir << "/.META removed" << std::endl;
#endif
		removeFile ( basedir, "/.META");
	}
	if ( checkFileExist ( basedir + "/.PRE" ) ) {
#ifdef DEBUG
		std::cerr << basedir << "/.PRE removed" << std::endl;
#endif
		removeFile ( basedir, "/.PRE");
	}
	if ( checkFileExist ( basedir + "/.POST" ) ) {
#ifdef DEBUG
		std::cerr << basedir << "/.POST removed" << std::endl;
#endif
		removeFile ( basedir , "/.POST");
	}
	if ( checkFileExist ( basedir + "/.MTREE" ) ) {
#ifdef DEBUG
		std::cerr << basedir << "/.MTREE removed" << std::endl;
#endif
		removeFile ( basedir, "/.MTREE");
	}
}
void removeFile(const std::string& basedir, const std::string& filename)
{
  if (filename != basedir && !remove(filename.c_str())) {
    char* path = strdup(filename.c_str());
    removeFile(basedir, dirname(path));
    free(path);
  }
}
int copyFile( const char *  destFile, const char *  origFile)
{
	FILE * infile  = fopen(origFile,  "rb");
	FILE * outfile = fopen(destFile, "wb");

	char buffer[BUFSIZ];

	while (!feof(infile)) {
		int n = fread(buffer, 1, BUFSIZ, infile);
		fwrite(buffer, 1, n, outfile);
	}

	fflush(outfile);

	fclose(infile);
	fclose(outfile);
	return 0;
}
	
int findDir(std::set<std::string>& filesList, const std::string& path)
{
  DIR *d;
  struct dirent *dir;
  d = opendir(path.c_str());
  if (d)
  {
    while ((dir = readdir(d)) != nullptr)
    {
			if ( dir->d_name[0] != '.' ) { // ignore any .directories
        	filesList.insert(dir->d_name);
      }
    }
    closedir(d);
  } else {
		std::cerr << YELLOW << path << " not exist." << NORMAL << std::endl;
		return -1;
	}
  return 0;
}
bool createRecursiveDirs(const std::string& path)
{
  char opath[MAXPATHLEN];
  char *p;
  size_t len;
  strcpy(opath, path.c_str());
  len = strlen(opath);
  if ( len > 1)
  {
    if(opath[len - 1] == '/')
      opath[len - 1] = '\0';
    for(p = opath; *p; p++)
    {
      if(*p == '/')
      {
        *p='\0';
        if(access(opath, 0))
          mkdir(opath,S_CARDS_MODE);
        *p = '/';
      }
    }
    if(access(opath, 0))
      mkdir(opath,S_CARDS_MODE);
    return true;
  }
  return false;
}

int findRecursiveFile(std::set<std::string>& filenameList, const char *filename, int spec)
{
	struct dirent *dent;
	DIR *dir;
	struct stat st;
	char fn[FILENAME_MAX];
	int res = WALK_OK;
	int len = strlen(filename);
	std::string sfilename;
	if (len >= FILENAME_MAX - 1)
                return WALK_NAMETOOLONG;

	strcpy(fn, filename);
	fn[len++] = '/';

	if (!(dir = opendir(filename)))
	{
		warn("can't open %s", filename);
		return WALK_BADIO;
	}
	errno = 0;
	while ((dent = readdir(dir)))
	{
		if (!(spec & WS_DOTFILES) && dent->d_name[0] == '.')
			continue;
		if (!strcmp(dent->d_name, ".") || !strcmp(dent->d_name, ".."))
			continue;
		strncpy(fn + len, dent->d_name, FILENAME_MAX - len);
		if (lstat(fn, &st) == -1)
		{
			warn("Can't stat %s", fn);
			res = WALK_BADIO;
			continue;
		}

		/* don't follow symlink unless told so */
		if (S_ISLNK(st.st_mode) && !(spec & WS_FOLLOWLINK))
			continue;

		/* will be false for symlinked dirs */
		if (S_ISDIR(st.st_mode))
		{
			/* recursively follow dirs */
			if ((spec & WS_RECURSIVE))
				findRecursiveFile(filenameList, fn, spec);

			if (!(spec & WS_MATCHDIRS))
				continue;
		}
		sfilename=fn;
		filenameList.insert(sfilename);
	}

	if (dir) closedir(dir);
	return res ? res : errno ? WALK_BADIO : WALK_OK;
}
int parseFile(std::set<std::string>& fileContent, const char* fileName)
{
	FILE* fp = fopen(fileName, "r");
	if (!fp)
		return -1;
	const int length = BUFSIZ;
  char input[length];
	std::string line;
  while (fgets(input, length, fp)) {
		input[strlen(input)-1] = '\0';
		line = input;
    fileContent.insert(line);
  }
  fclose(fp);
  return 0;
}

int parseFile(std::vector<std::string>& fileContent, const char* fileName)
{
	FILE* fp = fopen(fileName, "r");
	if (!fp)
		return -1;
	const int length = BUFSIZ;
	char input[length];
	std::string line;
	while (fgets(input, length, fp)) {
		input[strlen(input)-1] = '\0';
		line = input;
		fileContent.push_back(line);
	}
	fclose(fp);
	return 0;
}
int parseFile(std::string& Depends, const char* key, const char* fileName)
{
	FILE* fp = fopen(fileName, "r" );
	if (!fp)
		return -1;
	const int length = BUFSIZ;
	char input[length];
	std::string k = key;
	std::string line;
	std::string::size_type pos;
	bool find_end = false;
	while ( fgets( input, length, fp ) ) {
		line = stripWhiteSpace( input );
		if ( find_end ) {
			pos = line.find( ')' );
			if ( pos != std::string::npos ) {
#ifdef DEBUG
				std::cerr << line << std::endl;
#endif
				Depends += line.substr(0, pos);
#ifdef DEBUG
				std::cerr << Depends << std::endl;
#endif
				break;
			} else {
				// Check for comment lines and inline comments
				pos = line.find( '#' );
				if ( pos != std::string::npos ) {
					Depends += line.substr(0, pos) + " ";
				} else {
					Depends += line + " ";
				}
			}
		}
		if ( line.substr( 0, k.size() ) == key ){
			pos = line.find( ')' );
			if ( pos != std::string::npos ) {
#ifdef DEBUG
				std::cout << line << std::endl;
#endif
				Depends += line.substr(k.size(), pos - k.size());
#ifdef DEBUG
				std::cerr << Depends << std::endl;
#endif
				break;
			} else {
				// Check for comments lines and inline comments
				pos = line.find( '#' );
				if ( pos != std::string::npos ) {
					Depends += line.substr(k.size(), pos - k.size()) + " ";
				} else {
					Depends += line.substr(k.size()) + " ";
				}
				find_end=true;
			}
		}
	}
	fclose(fp);
	return 0;
}

