//  files_utils.cc
//
//  Copyright (c) 2000-2005 Per Liden
//  Copyright (c) 2006-2013 by CRUX team (http://crux.nu)
//  Copyright (c) 2013 by NuTyX team (http://nutyx.org)
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
string trim_filename(const string& filename)
{
  string search("//");
  string result = filename;

  for (string::size_type pos = result.find(search); pos != string::npos; pos = result.find(search))
    result.replace(pos, search.size(), "/");

  return result;
}
bool file_exists(const string& filename)
{
  struct stat buf;
  return !lstat(filename.c_str(), &buf);
}

bool file_empty(const string& filename)
{
  struct stat buf;

  if (lstat(filename.c_str(), &buf) == -1)
    return false;

  return (S_ISREG(buf.st_mode) && buf.st_size == 0);
}
bool file_equal(const string& file1, const string& file2)
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
bool permissions_equal(const string& file1, const string& file2)
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

void file_remove(const string& basedir, const string& filename)
{
  if (filename != basedir && !remove(filename.c_str())) {
    char* path = strdup(filename.c_str());
    file_remove(basedir, dirname(path));
    free(path);
  }
}
set<string> file_find(const string& path)
{
  set<string> files_list;
  DIR *d;
  struct dirent *dir;
  d = opendir(path.c_str());
  if (d)
  {
    while ((dir = readdir(d)) != NULL)
    {
      if ( strcmp (dir->d_name, ".") && strcmp (dir->d_name, "..") ) // ignore the directories dots
      {
        files_list.insert(dir->d_name);
      }
    }
    closedir(d);
  }
  return files_list;
}
bool create_recursive_dirs(const string& path)
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
          mkdir(opath,S_CARD_MODE);
        *p = '/';
      }
    }
    if(access(opath, 0))
      mkdir(opath,S_CARD_MODE);
    return true;
  }
  return false;
}


// vim:set ts=2 :
