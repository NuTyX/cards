// cards_argument_parser.cc
// 
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

#include "cards_argument_parser.h"

ArgParser::APCmd CardsArgumentParser::CMD_SYNC;

ArgParser::APOpt CardsArgumentParser::OPT_SIGNATURE;
ArgParser::APOpt CardsArgumentParser::OPT_DEPENDENCIES;
ArgParser::APOpt CardsArgumentParser::OPT_REZEPT;

CardsArgumentParser::CardsArgumentParser()
{
	addCommand(CMD_SYNC, "sync",
		"synchronize local info with remote repository",
		ArgParser::NONE, 0 , "");

	OPT_SIGNATURE.init("signature",
		's',
		"get the signature of the port");

	OPT_DEPENDENCIES.init("dependencies",
		'd',
		"get dependencies files");

	OPT_REZEPT.init("pkgfile",
		'p',
		"get the Pkgfile and info files");
	addOption(CMD_SYNC, OPT_SIGNATURE,false);
	addOption(CMD_SYNC, OPT_DEPENDENCIES, false);
	addOption(CMD_SYNC, OPT_REZEPT,false);
}
// vim:set ts=2 :
