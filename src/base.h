/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "pkgdbh.h"
#include "cards_argument_parser.h"
#include "file_utils.h"
#include "file_download.h"
#include "pkgrepo.h"

namespace cards {
class base : public pkgdbh
{
	const CardsArgumentParser& m_argParser;

public:
	base(const CardsArgumentParser& argParser);
	void run(int argc, char** argv);
	void printHelp() const;
};
}
