/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "pkgdbh.h"
#include "cards_argument_parser.h"

class cards_base : public pkgdbh
{
private:
	const CardsArgumentParser& m_argParser;

public:
	cards_base(const CardsArgumentParser& argParser);
	void run(int argc, char** argv);
	void printHelp() const;
};
