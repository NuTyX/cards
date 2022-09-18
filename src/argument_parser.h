// argument_parser.h
//
//  Copyright (c) 2004 Johannes Winkelmann (jw at tks6 dot net)
//  Copyright (c) 2014 - 2022 by NuTyX team (http://nutyx.org)
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

#ifndef _ARGPARSER_H_
#define _ARGPARSER_H_


#include "error_treat.h"

#include <map>
#include <vector>
#include <string>

// TODO:
// -- important
// - allow multiple occurences of arguments:
//   prt-get --config-append="..." --config-append="..."
// - Allow global --help, --usage
// - check for duplicate entries

// -- nice to have
// 2. allow optional values for args, like --with-gtk[=DIR]
// 4. improve predefined commands: allow descriptions even for such
// 7. Add predefined --version, show in usage; add Contact text
// 8. Allow disabling of predefined options
// 9. make parseError more convenient (passing cmd->name all over...)

class APOpt;
class APCmd;


/**
 * \brief argument parser class
 *
 * Yet another argument parser, meant to speed up development of new
 * applications. Its focus is on being object oriented and safe to use
 */
class ArgParser
{
public:
	/**
	 * types of argument number checking
	 */
	enum ArgNumberCheck { NONE, MIN, EQ, MAX };

	/**
	 * Command passed
	 */
	enum CMD {
		CMD_HELP=1,
		CMD_CONFIG,
		CMD_INSTALL,
		CMD_REMOVE,
		CMD_FILES,
		CMD_LIST,
		CMD_SYNC,
		CMD_INFO,
		CMD_QUERY,
		CMD_SEARCH,
		CMD_PURGE,
		CMD_BASE,
		CMD_DIFF,
		CMD_UPGRADE,
		CMD_CREATE,
		CMD_DEPCREATE,
		CMD_DEPENDS,
		CMD_DEPTREE,
		CMD_LEVEL
	};

	/**
	 * Option passed
	 */
	enum OPT {
		OPT_NONE,
		OPT_ALL,
		OPT_BINARIES,
		OPT_PORTS,
		OPT_CHECK
	};
	/**
	 * APOpt and APCmd are the classes used in client code, to make
	 * efficient comparison of the selected command.
	 *
	 * In addition, it's currently mainly for programs which use a
	 * command syntax, much like CVS
	 */
	class APOpt
	{
	public:
		friend class ArgParser;
		APOpt() : id(-1), opt(OPT_NONE), m_initialized(false) {}
		bool operator ==(const APOpt& other) const { return other.id == id; }

		void init(const std::string& longName,
				  char shortName,
				  const std::string& description,
				  const bool valueRequired=false,
				  const std::string& valueName="") {

			m_initialized = true;
			m_longName = longName;
			m_shortName = shortName;
			m_description = description;
			m_valueRequired = valueRequired;
			m_valueName = valueName;
		}

	private:
		int id;
		OPT opt;
		std::string m_longName;
		char m_shortName;
		std::string m_shortInfo;
		std::string m_description;
		bool m_valueRequired;
		std::string m_valueName;

		bool m_initialized;
	};

	class APCmd
	{
	public:
		friend class ArgParser;
		APCmd() : id(-1) {}
		bool operator ==(const APCmd& other) const { return other.id == id; }

	private:
		int id;
	};

private:

	// internal representation of options and commands
	class Option
	{
	public:
		int id;
		std::string description;
		char shortName;
		std::string longName;


		bool requiresValue;
		std::string valueName;
	};

	class Command
	{
	public:
		int id;
		CMD cmd;
		std::string name;
		std::string description;
		std::string shortInfo;
		ArgNumberCheck argNumberCheck;
		unsigned int argNumber;
		std::string otherArguments;

		std::map<int, Option*> mandatoryOptions;
		std::map<int, Option*> options;

		ArgParser::APCmd* apCmd;
	};



public:
	ArgParser();
	virtual ~ArgParser();

	/**
	 * add a command
	 * 
	 * \param cmd a possible enumerate CMD
	 * \param name the name of the command to be parserd from the command line
	 * \param shortInfo a short description, used for general help screen
	 * \param description a description, used for the help screens
	 * \param argNumberCheck what kind of argument number checking
	 * \param argNumber optional number of arguments
	 * \param otherArguments value to display in the help screen for following (non option) arguments
	 */
	int addCommand(CMD cmd,
			const std::string& name,
			const std::string& shortInfo,
			const std::string& description,
			ArgNumberCheck argNumberCheck,
			const int argNumber=-1,
			const std::string& otherArguments="");

  
	/**
	 * add a command
	 *
	 * \param cmd a reference to the command; use it to compare the actually selected command against this one after parsing
	 * \param name the name of the command to be parsed from the command line
	 * \param shortInfo a short description, used for general help screen
	 * \param description a description, used for the help screens
	 * \param argNumberCheck what kind of argument number checking
	 * \param argNumber optional number of arguments
	 * \param otherArguments value to display in the help screen for following (non option) arguments
	 */
	int addCommand(APCmd& cmd,
			const std::string& name,
			const std::string& shortInfo,
			const std::string& description,
			ArgNumberCheck argNumberCheck,
			const int argNumber=-1,
			const std::string& otherArguments="");

	/**
	 * add an option to a command - this will fail with an assertion
	 * of \a key has not been initialized using init()
	 *
	 * \param cmd a possible enumerate CMD, the command to add an option to
	 * \param key the option reference; use it to check for certain options after parsing
	 * \param required whether this option is required
	 */
	int addOption(const CMD commandKey,
                         APOpt& key,
                         bool required);
	/**
	 * add an option to a command - this will fail with an assertion
	 * of \a key has not been initialized using init()
	 *
	 * \param cmd the command to add an option to
	 * \param key the option reference; use it to check for certain options after parsing
	 * \param required whether this option is required
	 */
	int addOption(const APCmd& cmd,
				  APOpt& key,
				  bool required);

	/**
	 * the actual parsing. Highly recommended :-)
	 */
	void parse(int argc, char** argv);

	/**
	 * the command which was parsed, to be used to compare against
	 * actual APCmd obtained from addCommand calls
	 */
	APCmd command() const;

	/**
	 * the name of the application, from argv[0]
	 */
	std::string appName() const;

	/**
	 * \return true if \param key is set, false otherwise
	 */
	bool isSet(const APOpt& key) const;

	/**
	 * \return the value attached to the option \param key if any
	 */
	std::string getOptionValue(const APOpt& key) const;

	/**
	 * \return the CMD value of the command
	 */
	CMD getCmdValue() const;

	/**
	 * \return the id value of the command
	 */
	int getIdValue();

	/**
	 * \return the id value of the command
	 */
	int getIdValue( const APCmd& command);

	/**
	 * the remaining arguments
	 */
	const std::vector<std::string>& otherArguments() const;

	/**
	 * \return an application identification to be used in the usage
	 */
	virtual std::string getAppIdentification() const { return ""; }


	/**
	 * \return the help of the command
	 */
	void printHelp(const std::string& cmd);

private:

	std::string generateHelpForCommand(const std::string& command) const;
	std::string generateUsage() const;

	bool isSet(int key) const;

	std::string generateOptionString(Option* o) const;

	void parseError(const std::string& error, const std::string& cmd="") const;

	std::map<std::string, Command*> m_commands;
	std::map<int, Command*> m_commandIdMap;
	std::map<int, Option*> m_options;

	std::map<char, Option*> m_optionsByShortName;
	std::map<std::string, Option*> m_optionsByLongName;
	std::map<int, std::string> m_setOptions;

	std::vector<std::string> m_otherArguments;
	APCmd m_command;
	CMD m_cmd;
	std::string m_appName;


	int m_cmdIdCounter;
	int m_optIdCounter;
	APOpt PREDEFINED_CMD_HELP;
};


#endif /* _ARGPARSER_H_ */
// vim:set ts=2 :
