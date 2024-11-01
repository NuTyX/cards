/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "argument_parser.h"

class CardsArgumentParser : public ArgParser
{
	public:
		CardsArgumentParser();
		virtual ~CardsArgumentParser() {};

		static ArgParser::APOpt OPT_DOWNLOAD_ONLY;
		static ArgParser::APOpt OPT_DOWNLOAD_READY;
		static ArgParser::APOpt OPT_NO_SYNC;

		static ArgParser::APOpt OPT_CHECK;
		static ArgParser::APOpt OPT_SIZE;

		static ArgParser::APOpt OPT_FULL;
		static ArgParser::APOpt OPT_FORCE;
		static ArgParser::APOpt OPT_UPDATE;

		static ArgParser::APOpt OPT_INSTALLED;
		static ArgParser::APOpt OPT_BINARIES;
		static ArgParser::APOpt OPT_PORTS;
		static ArgParser::APOpt OPT_SETS;
		static ArgParser::APOpt OPT_VERSION;

		static ArgParser::APOpt OPT_IGNORE;
		static ArgParser::APOpt OPT_NOLOGENTRY;


		static ArgParser::APOpt OPT_DISABLE;

		static ArgParser::APOpt OPT_DRY;
		static ArgParser::APOpt OPT_REMOVE;
		static ArgParser::APOpt OPT_ALL;
		static ArgParser::APOpt OPT_LEVEL_SILENCE;

		static ArgParser::APOpt OPT_ROOT;
		static ArgParser::APOpt OPT_CONFIG_FILE;

		static ArgParser::APOpt OPT_NAMES;

		std::string getAppIdentification() const
		{
			return std::string("cards ")  + PACKAGE_VERSION + "\n";
		}
};
