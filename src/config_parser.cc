//  config_parser.cc
//
//  Copyright (c) 2002-2005 by Johannes Winkelmann jw at tks6 dot net
//  Copyright (c) 2014 by NuTyX team (http://nutyx.org)
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

#include <iostream>
#include <cstdio>
#include <cstring>

#include "config_parser.h"
#include "error_treat.h"

using namespace std;
ConfigParser::ConfigParser(const std::string& fileName)
	: m_configFileName(fileName)
{
}
int ConfigParser::parseMD5sumCategoryDirectory()
{
	parseConfig(m_configFileName);
	for (vector<DirUrl>::iterator i = m_config.dirUrl.begin();i != m_config.dirUrl.end(); ++i) {
		if ( i->Url.size() == 0 )
			continue;
		portsDirectory pD;
		fileList fL;
		pD.Dir = i->Dir;
		pD.Url = i->Url;
		
		string categoryMD5sumFile = i->Dir + "/MD5SUM" ;
		FileDownload MD5Sum(i->Url + "/MD5SUM",
			i->Dir,
			"MD5SUM", false);
		MD5Sum.downloadFile();
		
		vector<string> MD5SUMFileContent;

		if ( parseFile(MD5SUMFileContent,categoryMD5sumFile.c_str()) != 0) {
			cerr << "cannot read file MD5SUM" << endl;
			return -1;
		}
		remove(categoryMD5sumFile.c_str());
		for ( vector<string>::iterator i = MD5SUMFileContent.begin();i!=MD5SUMFileContent.end();++i) {
			string val = *i;
			if ( i->size() <  34 ) {
				cerr << "missing info" << endl;
				continue;
			}
			if ( val[32] != ':' ) {
				cerr << "wrong  info" << endl;
				continue;
			}
			string::size_type pos = val.find(':');
			if (pos != string::npos) {
				fL.md5SUM = stripWhiteSpace(val.substr(0,pos));
			}
			string tmpName = val.substr(pos+1);
			pos = tmpName.find('@');
			if (pos != string::npos) {
				fL.basePackageName = stripWhiteSpace(tmpName.substr(0,pos));
				fL.version = stripWhiteSpace(tmpName.substr(pos+1));
				pD.basePackageList.push_back(fL);
			}
		}
		m_packageList.push_back(pD);
	}
/*	for (std::vector<portsDirectory>::iterator i = m_packageList.begin();i !=  m_packageList.end();++i) {
		portsDirectory pD = *i;
		for (std::vector<fileList>::iterator j = pD.basePackageList.begin(); j != pD.basePackageList.end();++j) {
			cout << i->Url << " "<< i->Dir << " " << j->basePackageName  << " " << j->version << " "  << j->md5SUM << endl;
		}
	} */
	return 0;
}
int ConfigParser::parseBasePackageList()
{
	InfoFile downloadFile;
	vector<InfoFile> downloadFilesList;
	for (std::vector<portsDirectory>::iterator i = m_packageList.begin();i !=  m_packageList.end();++i) {
		portsDirectory pD = *i;
		for (std::vector<fileList>::iterator j = pD.basePackageList.begin(); j != pD.basePackageList.end();++j) {
			if ( ! checkFileExist(i->Dir + "/" + j->basePackageName  + "@" + j->version + "/MD5SUM") ) {
				downloadFile.url = i->Url + "/" + j->basePackageName  + "@" + j->version + "/MD5SUM";
				downloadFile.dirname = i->Dir + "/" + j->basePackageName  + "@" + j->version;
				downloadFile.filename = "/MD5SUM";
				downloadFile.md5sum = j-> md5SUM;
				downloadFilesList.push_back(downloadFile);
				cout << i->Dir + "/" + j->basePackageName  + "@" + j->version << endl;
			}
		}
	}
	if ( downloadFilesList.size() > 0 ) {
		FileDownload FD(downloadFilesList,false);
	}
	// From here we can parse the binaries

	return 0;
}
int ConfigParser::parseConfig(const string& fileName)
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
				} else{
					DU.Dir = stripWhiteSpace(val);
					DU.Url = "";
				}
				m_config.dirUrl.push_back(DU) ;
			}
			if (key == "arch") {
				if (val != "i686") {
					m_config.arch="x86_64";
				} else {
					m_config.arch="i686";
				}
			}
			if (key == "locale") {
				m_config.locale.push_back(val);
			}
			if (key == "base") {
				m_config.baseDir.push_back(val);
			}
		}
	}
	fclose(fp);
	return 0;
}
int ConfigParser::parseConfig(const string& fileName, Config& config)
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
					config.dirUrl.push_back(DU) ;
			}
			if (key == "arch") {
				if (val != "i686") {
					config.arch="x86_64";
				} else {
					config.arch="i686";
				}
			}
			if (key == "locale") {
				config.locale.push_back(val);
			}
			if (key == "base") {
				config.baseDir.push_back(val);
			}
		}
	}
	fclose(fp);
	return 0;
}
string ConfigParser::stripWhiteSpace(const string& input)
{
	string output = input;
	while (isspace(output[0])) {
		output = output.substr(1);
	}
	while (isspace(output[output.length()-1])) {
		output = output.substr(0, output.length()-1);
	}

	return output;
}
// vim:set ts=2 :
