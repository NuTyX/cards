//  files_utils.cc
//
//  Copyright (c) 2000-2005 Per Liden
//  Copyright (c) 2006-2013 by CRUX team (http://crux.nu)
//  Copyright (c) 2013-2014 by NuTyX team (http://nutyx.org)
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

#include "file_utils.h"
int getConfig(const string& fileName, Config& config)
{
	FILE* fp = fopen(fileName.c_str(), "r");
	if (!fp) {
		return -1;
	}

	char line[512];
	string s;
	while (fgets(line, 512, fp)) {
		if (line[strlen(line)-1] == '\n' ) {
			line[strlen(line)-1] = '\0';
		}
		s = line;

		// strip comments
		string::size_type pos = s.find('#');
		if (pos != string::npos) {
			s = s.substr(0,pos);
		}

		// whitespace separates
		pos = s.find(' ');
		if (pos == string::npos) {
			// try with a tab
			pos = s.find('\t');
		}
		if (pos != string::npos) {
			string key = s.substr(0, pos);
			string val = stripWhiteSpace(s.substr(pos));

			if (key == "dir") {
				string::size_type pos = val.find('|');
				DirUrl DU;
				if (pos != string::npos) {
					DU.Dir = stripWhiteSpace(val.substr(0,pos));
					DU.Url = stripWhiteSpace(val.substr(pos+1));
				} else {
					DU.Dir = stripWhiteSpace(val);
					DU.Url = "";
				}
				config.dirUrl.push_back(DU);
			}
			if (key == "logdir") {
				config.logdir = val;
			}
			config.arch = getMachineType();
      if ( config.arch== "" ) {
        return -1;
			}
			if (key == "locale") {
				config.locale.push_back(val);
			}
			if (key == "base") {
				config.baseDir.push_back(val);
			}
			if (key == "name") {
				config.name = val;
			}
			if (key == "version") {
				config.version = val;
			}
		}
	}
	fclose(fp);
	return 0;
}

FILE *openFile(const char *fileName)
{
	FILE *fileHandler = NULL;
	if ( (fileHandler = fopen(fileName,"r")) == NULL ) {
		fprintf(stderr,"Failed to open %s\n",fileName);
		if (errno) {
			perror(fileName);
		}
		return NULL;
	}
	return fileHandler;
}

void * getDatas ( void * var, FILE * file, long offset, size_t size, size_t nmemb)
{
  void * mvar;
  if (size == 0 || nmemb == 0)
    return NULL;
  if (fseek (file, offset, SEEK_SET))
  {
    printf("Cannot seek\n");
    return NULL;
  }
  mvar = var;
  if (mvar == NULL)
  {
  /* Check for overflow.  */
    if (nmemb < (~(size_t) 0 - 1) / size)
      /* + 1 so that we can '\0' terminate invalid string table sections.  */
      mvar = malloc (size * nmemb + 1);
    if (mvar == NULL)
    {
      printf("Out of memory\n");
      return NULL;
    }
    ((char *) mvar)[size * nmemb] = '\0';
  }
  if (fread (mvar, size, nmemb, file) != nmemb)
  {
    printf("Unable to read in 0x%lx bytes", (unsigned long)(size * nmemb));
    return NULL;
  }
  return mvar;
}

string trimFileName(const string& filename)
{
  string search("//");
  string result = filename;

  for (string::size_type pos = result.find(search); pos != string::npos; pos = result.find(search))
    result.replace(pos, search.size(), "/");

  return result;
}
string modifyTimeFile(const string& filename)
{
	string sTimeFile = "";
	char * c_time_s;
	struct stat buf;
	if (lstat(filename.c_str(), &buf) == -1) {
		return sTimeFile;
	}
	c_time_s = ctime(&buf.st_mtime);
	sTimeFile = c_time_s;
	return sTimeFile;
}
bool checkFileExist(const string& filename)
{
  struct stat buf;
  return !lstat(filename.c_str(), &buf);
}

bool checkFileEmpty(const string& filename)
{
  struct stat buf;

  if (lstat(filename.c_str(), &buf) == -1)
    return false;

  return (S_ISREG(buf.st_mode) && buf.st_size == 0);
}
bool checkFilesEqual(const string& file1, const string& file2)
{
  struct stat buf1, buf2;

  if (lstat(file1.c_str(), &buf1) == -1)
    return false;

  if (lstat(file2.c_str(), &buf2) == -1)
    return false;

  // Regular files
  if (S_ISREG(buf1.st_mode) && S_ISREG(buf2.st_mode)) {
    ifstream f1(file1.c_str());
    ifstream f2(file2.c_str());

    if (!f1 || !f2)
      return false;

    while (!f1.eof()) {
      char buffer1[4096];
      char buffer2[4096];
      f1.read(buffer1, 4096);
      f2.read(buffer2, 4096);
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
bool checkPermissionsEqual(const string& file1, const string& file2)
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
void cleanupMetaFiles(const string& basedir)
{
	if ( checkFileExist( basedir + "/.INFO") ) {
#ifndef NDEBUG
		cerr << basedir << "/.INFO removed" << endl;
#endif
		removeFile ( basedir, "/.INFO");
	}
	if ( checkFileExist( basedir + "/.META") ) {
#ifndef NDEBUG
		cerr << basedir << "/.META removed" << endl;
#endif
		removeFile ( basedir, "/.META");
	}
	if ( checkFileExist ( basedir + "/.PRE" ) ) {
#ifndef NDEBUG
		cerr << basedir << "/.PRE removed" << endl;
#endif
		removeFile ( basedir, "/.PRE");
	}
	if ( checkFileExist ( basedir + "/.POST" ) ) {
#ifndef NDEBUG
		cerr << basedir << "/.POST removed" << endl;
#endif
		removeFile ( basedir , "/.POST");
	}
	if ( checkFileExist ( basedir + "/.MTREE" ) ) {
#ifndef NDEBUG
		cerr << basedir << "/.MTREE removed" << endl;
#endif
		removeFile ( basedir, "/.MTREE");
	}
	if ( checkFileExist ( basedir + "/.README" ) ) {
#ifndef NDEBUG
		cerr << basedir << "/.README" << endl;
#endif
		removeFile ( basedir, "/.README");
	}
}
void removeFile(const string& basedir, const string& filename)
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

	const int size = 16384;
	char buffer[size];

	while (!feof(infile)) {
		int n = fread(buffer, 1, size, infile);
		fwrite(buffer, 1, n, outfile);
	}

	fflush(outfile);

	fclose(infile);
	fclose(outfile);
	return 0;
}
int findDir(itemList* filesList, const char* path)
{
	DIR *d;
	DIR *sd;
	struct dirent *dir;
	d = opendir(path);
	char fullPath[255];
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if ( dir->d_name[0] != '.' ) { // ignore any .directories
				sprintf(fullPath,"%s/%s",path,dir->d_name);
				sd = opendir(fullPath);
				/* want to make shure its not a regular file
					maybe a beter way ?
				*/
				if (sd) {
					addItemToItemList(filesList,fullPath);
					closedir(sd);
				} else {
					cerr << "WARNING "<< fullPath << " is NOT a directory..."<< endl;
				}
			}
		}
		closedir(d);
	} else { 
		cerr << "Cannot open " << path << endl;
		return -1;
	}
	return 0;
}
	
int findFile(set<string>& filesList, const string& path)
{
  DIR *d;
  struct dirent *dir;
  d = opendir(path.c_str());
  if (d)
  {
    while ((dir = readdir(d)) != NULL)
    {
			if ( dir->d_name[0] != '.' ) { // ignore any .directories
        	filesList.insert(dir->d_name);
      }
    }
    closedir(d);
  } else {
		cerr << YELLOW << path << " not exist." << NORMAL << endl;
		return -1;
	}
  return 0;
}
bool createRecursiveDirs(const string& path)
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
int findRecursiveFile(set<string>& filenameList, char *filename, regex_t *reg, int spec)
{
	struct dirent *dent;
	DIR *dir;
	struct stat st;
	char fn[FILENAME_MAX];
	int res = WALK_OK;
	int len = strlen(filename);
	string sfilename;
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
				findRecursiveFile(filenameList, fn, reg, spec);

			if (!(spec & WS_MATCHDIRS))
				continue;
		}

		/* pattern match */
		if (!regexec(reg, fn, 0, 0, 0))
		{
			sfilename=fn;
			filenameList.insert(sfilename);
		}				
	}

	if (dir) closedir(dir);
	return res ? res : errno ? WALK_BADIO : WALK_OK;
}
int readFileStripSpace(itemList* fileContent, const char* fileName)
{
	FILE* fp = NULL;
	if ((fp = openFile (fileName)) == NULL ) {
		return -1;
	}
	char input[1024];
	while (fgets(input, 1024, fp)) {
		input[strlen(input)-1] = '\0';
		string inputS = input;
		string stripString = stripWhiteSpace(inputS);
		if (stripString.size() > 0)
			addItemToItemList(fileContent,stripString.c_str());
		}
		fclose(fp);
		return 0;	
}
int readFile(itemList* fileContent, const char* fileName)
{
	FILE* fp = NULL;
	if ((fp = openFile (fileName)) == NULL ) {
		return -1;
	}
	char input[1024];
	while (fgets(input, 1024, fp)) {
		input[strlen(input)-1] = '\0';
		addItemToItemList(fileContent,input);
	}
	fclose(fp);
	return 0;
}
int parseFile(set<string>& fileContent, const char* fileName)
{
	FILE* fp = NULL;
	if ((fp = openFile (fileName)) == NULL ) {
		return -1;
	}
	const int length = BUFSIZ;
  char input[length];
	string line;
  while (fgets(input, length, fp)) {
		input[strlen(input)-1] = '\0';
		line = input;
    fileContent.insert(line);
  }
  fclose(fp);
  return 0;
}

int parseFile(vector<string>& fileContent, const char* fileName)
{
	FILE* fp = NULL;
	if ((fp = openFile (fileName)) == NULL ) {
		return -1;
	}
	const int length = BUFSIZ;
	char input[length];
	string line;
	while (fgets(input, length, fp)) {
		input[strlen(input)-1] = '\0';
		line = input;
		fileContent.push_back(line);
	}
	fclose(fp);
	return 0;
}
bool findMD5sum(const string& fileName, unsigned char* result)
{
	struct md5_context ctx;
	unsigned char buffer[1000];

	FILE* f = fopen(fileName.c_str(), "rb");
	if (!f)
		return false;
	md5_starts( &ctx );
	int i = 0;
	while( ( i = fread( buffer, 1, sizeof( buffer ), f ) ) > 0 )
	{
		md5_update( &ctx, buffer, i );
	}
	fclose(f);

	md5_finish( &ctx, result );
	return true;
}
bool checkMD5sum(const char * fileName, const char * MD5Sum)
{
  bool same = true;
  unsigned char md5sum[16];

  if ( findMD5sum(fileName,md5sum) ) {
    static char hexNumbers[] = {'0','1','2','3','4','5','6','7',
                                '8','9','a','b','c','d','e','f'};

    unsigned char high, low;
    for (int i = 0; i < 16; ++i) {
      high = (md5sum[i] & 0xF0) >> 4;
      low = md5sum[i] & 0xF;
      if ( *(MD5Sum+2*i) - hexNumbers[high] ||
           *(MD5Sum+2*i+1) - hexNumbers[low]) {
        same = false;
        break;
      }
    }
  } else {
    same = false;
  }
  return same;
}
// vim:set ts=2 :
