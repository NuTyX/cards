// argument_parser.cxx
//
//  Copyright (c) 2004 by Johannes Winkelmann (jw at tks6 dot net)
//  Copyright (c) 2013-2022 by NuTyX team (http://nutyx.org)
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
#include <sstream>
#include <cassert>
#include <libgen.h>
#include <cstring>
#include <cstdlib>

#include "argument_parser.h"

ArgParser::ArgParser()
    : m_cmdIdCounter(0),
      m_optIdCounter(0)
{
}

ArgParser::~ArgParser()
{
    std::map<int, Option*>::iterator oit = m_options.begin();
    for (; oit != m_options.end(); ++oit) {
        delete oit->second;
    }

    std::map<std::string, Command*>::iterator cit = m_commands.begin();
    for (; cit != m_commands.end(); ++cit) {
        delete cit->second;
    }
}

int ArgParser::addCommand(CMD cmd,
                          const std::string& name,
                          const std::string& shortInfo,
                          const std::string& description,
                          ArgNumberCheck argNumberCheck,
                          int argNumber,
                          const std::string& otherArguments)
{
    Command* command = new Command;

    ++m_cmdIdCounter;

    command->cmd = cmd;
    command->id = m_cmdIdCounter;
    command->name = name;
    command->argNumber = argNumber;
    command->argNumberCheck = argNumberCheck;

    command->shortInfo = shortInfo;
    command->description = description;
    command->otherArguments = otherArguments;

    m_commands[name] = command;
    m_commandIdMap[cmd] = command;

    PREDEFINED_CMD_HELP.init("help", 'h', _("\t   Print this help message"));

    // add predefined option
    addOption(cmd, PREDEFINED_CMD_HELP, false);

    return 0;
}

int ArgParser::addOption(CMD commandKey,
                         APOpt& key,
                         bool required)
{
    // TODO: check for null cmd
    if (m_commandIdMap.find(commandKey) == m_commandIdMap.end()) {
        return -1;
    }

    Option* o = 0;
    if (key.id != -1 && m_options.find(key.id) != m_options.end()) {
        o = m_options.find(key.id)->second;
    }

    if (!o) {

        assert(key.m_initialized == true);

        o = new Option();
        ++m_optIdCounter;
        key.id = m_optIdCounter;

        o->id = key.id;
        o->description = key.m_description;
        o->requiresValue = key.m_valueRequired;
        o->shortName = key.m_shortName;
        o->longName = key.m_longName;
        o->valueName = key.m_valueName;

        if (key.m_shortName != 0) {
            m_optionsByShortName[key.m_shortName] = o;
        }
        if (key.m_longName != "") {
            m_optionsByLongName[key.m_longName] = o;
        }

        m_options[key.id] = o;

    }


    Command* cmd = m_commandIdMap[commandKey];
    if (required) {
        cmd->mandatoryOptions[key.id] = o;
    } else {
        cmd->options[key.id] = o;
    }


    return 0;
}


void ArgParser::parse(int argc, char** argv)
{
    bool commandFound = false;
    std::string command = "";
    Command* cmd = 0;
    int cmdPos = 0;

    m_appName = basename(argv[0]);

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            if (!commandFound) {
                if (m_commands.find(argv[i]) == m_commands.end()) {
                    parseError("Non option / Non command argument \033[1;31m" +
                               std::string(argv[i]) + NORMAL);
                }
                cmd = m_commands[argv[i]];
                m_command.id = cmd->id;
                m_cmd = cmd->cmd;
                commandFound = true;
                cmdPos = i;
                break;
            }
        } else {
            // TODO: add proper handling for global options

            std::string arg = argv[i];
            if (arg == "-h" || arg == "--help") {
                std::cout << generateUsage() << std::endl;
                exit(0);
            }
        }

    }

    if (!commandFound) {
        parseError(_("No specify command"));
        exit(-1);
    }

    for (int i = 1; i < argc; ++i) {

        if (i == cmdPos) {
            continue;
        }

        if (argv[i][0] == '-') {
            if (argv[i][1] == '\0') {
                parseError(_("Illegal token: '-'"), cmd->name);
            } else if (argv[i][1] == '-') {

                char* valPtr = strchr(argv[i]+2, '=');
                if (valPtr) {
                    *valPtr = '\0';
                    ++valPtr;
                }

                if (m_optionsByLongName.find(argv[i]+2) ==
                    m_optionsByLongName.end()) {
                    parseError(_("unknown option: ") + std::string(argv[i]+2),
                               cmd->name);
                }

                Option* o = m_optionsByLongName[argv[i]+2];
                std::string val = "";
                if (o->requiresValue) {
                    if (valPtr == NULL || *valPtr == 0) {
                        parseError(_("Value required for option '") +
                                   std::string(argv[i]+2), cmd->name);
                    } else {
                        val = valPtr;
                    }
                }
                m_setOptions[o->id] = val;
            } else {
                if (argv[i][2] != '\0') {
                    parseError(_("invalid short option '") +
                               std::string(argv[i]+1) + "'", cmd->name);
                }

                if (m_optionsByShortName.find(argv[i][1]) ==
                    m_optionsByShortName.end()) {
                    parseError(_("unknown short option: ") + std::string(argv[i]+1),
                               cmd->name);
                }

                Option* o = m_optionsByShortName[argv[i][1]];
                std::string val = "";
                if (o->requiresValue) {
                    if (i+1 == argc) {
                        parseError(_("Option required for option '") +
                                   std::string(argv[i]+1), cmd->name);
                    } else {
                        val = argv[i+1];
                        ++i;
                    }
                }
                m_setOptions[o->id] = val;
            }
        } else {
            m_otherArguments.push_back(std::string(argv[i]));
        }
    }

    if (isSet(PREDEFINED_CMD_HELP)) {
        std::cout << generateHelpForCommand(cmd->name)
					<< std::endl;
        exit(0);
    } else {

        // make sure all required options of a command are set

        std::map<int, Option*>::iterator it;
        it = cmd->mandatoryOptions.begin();
        for (; it != cmd->mandatoryOptions.end(); ++it) {
            if (!isSet(it->second->id)) {
                parseError(_("Command ") + cmd->name +
                           _(" requires option ") +
                           std::string("-") + it->second->shortName +
                           std::string(",  ") +
                           std::string("--") + it->second->longName + _(" not found"),
                           cmd->name);
            }
        }
    }

    switch (cmd->argNumberCheck)
    {
        case EQ:
            if (m_otherArguments.size() != cmd->argNumber) {
                std::ostringstream ostr;
                ostr << BLUE << cmd->name << NORMAL
                     << _(" takes exactly ")
                     << cmd->argNumber
                     << (cmd->argNumber == 1 ? " argument." : " arguments.");

                parseError(ostr.str(), cmd->name);
            }
            break;
        case MIN:
            if (m_otherArguments.size() < cmd->argNumber) {
                std::ostringstream ostr;
                ostr << BLUE << cmd->name << NORMAL
                     << _(" takes at least ")
                     << cmd->argNumber
                     << (cmd->argNumber == 1 ? _(" argument.") : _(" arguments."));

                parseError(ostr.str(), cmd->name);
            }
            break;
        case MAX:
            if (m_otherArguments.size() > cmd->argNumber) {
                std::ostringstream ostr;
                ostr << BLUE << cmd->name << NORMAL
                     << _(" takes at most ")
                     << cmd->argNumber
                     << (cmd->argNumber == 1 ? _(" argument.") : _(" arguments."));

                parseError(ostr.str(), cmd->name);
            }
            break;
        case NONE:
        default:
            break;
    }
}

void ArgParser::parseError(const std::string& error, const std::string& cmd) const
{
//    cerr << "Parse error: " << error << endl;
		std::cerr << error << std::endl;
    if (cmd != "") {
        std::cerr << generateHelpForCommand(cmd) << std::endl;
    } else {
        std::cerr << generateUsage() << std::endl;
    }
    exit(-1);
}
ArgParser::CMD ArgParser::getCmdValue() const
{
	return m_cmd;
}
int ArgParser::getIdValue( const APCmd& command)
{
	return command.id;
}
int ArgParser::getIdValue()
{
	return m_command.id;
}
ArgParser::APCmd ArgParser::command() const
{
    return m_command;
}

bool ArgParser::isSet(const APOpt& key) const
{
    return isSet(key.id);
}

bool ArgParser::isSet(int key) const
{
    return m_setOptions.find(key) != m_setOptions.end();
}

void ArgParser::printHelp(const std::string& cmd)
{
	std::cerr << generateHelpForCommand(cmd) << std::endl;	
}
std::string ArgParser::getOptionValue(const APOpt& key) const
{
    return m_setOptions.find(key.id)->second;
}

std::string ArgParser::appName() const
{
    return m_appName;
}

std::string ArgParser::generateHelpForCommand(const std::string& command) const
{
	std::map<std::string, Command*>::const_iterator cit =
		m_commands.find(command);

	if (cit == m_commands.end()) {
		return "";
	}

	const Command * const  cmd = cit->second;
	std::string help = "";
	help += ACTION;
	help += "\n  " + cmd->shortInfo;
	help += "\n\n";
	help += USAGE;
	help += "\n  " ;
	help += BLUE + m_appName + " " + cmd->name + " ";
	help += OPTIONS;
	help += " " + cmd->otherArguments + "\n\n";
	help += DESCRIPTION;
	help += "\n  " + cmd->description;
	help += "\n\n";


	std::map<int, Option*>::const_iterator it = cmd->mandatoryOptions.begin();
	if (it != cmd->mandatoryOptions.end()) {
		help += REQUIRED;
		help += ":\n";
		for (; it != cmd->mandatoryOptions.end(); ++it) {
			help += generateOptionString(it->second);
		}
	}

	it = cmd->options.begin();
	if (it != cmd->options.end()) {
		help += OPTIONS;
		help += ":\n";
		for (; it != cmd->options.end(); ++it) {
			help += generateOptionString(it->second);
		}
	}

	return help;
}

std::string ArgParser::generateOptionString(Option* o) const
{
	std::string help = "  ";

	if (o->shortName) {
		help += "-";
		help += o->shortName;

		if (o->requiresValue && o->valueName != "") {
			help += " " + o->valueName;
		}

		help += ", ";
	}

	if (o->longName != "") {
		help += "--";

		help += o->longName;
		if (o->requiresValue && o->valueName != "") {
			help += "=" + o->valueName;
		}

		help += "  ";
		help += o->description;
		help += "\n";
	}

	return help;
}

std::string ArgParser::generateUsage() const
{
	std::string usage = getAppIdentification() +
  	USAGE;
	usage += " ";
	usage += BLUE + m_appName + " ";
	usage += COMMAND;
	usage += " ";
	usage += OPTIONS;
	usage += _(" <arguments>\n\n Where ");
	usage += COMMAND;
	usage += _(" is one of the following:\n\n");

	std::map<std::string, Command*>::const_iterator it;
	it = m_commands.begin();
	for (; it != m_commands.end(); ++it) {
        usage += BLUE + it->first + WHITE + "    \t" +
            it->second->shortInfo + "\n" + NORMAL;
	}

	return usage;
}

const std::vector<std::string>& ArgParser::otherArguments() const
{
	return m_otherArguments;
}
// vim:set ts=2 :
