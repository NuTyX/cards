/* SPDX-License-Identifier: LGPL-2.1-or-later  */
#pragma once
#include "cards_argument_parser.h"
#include "compile_dependencies_utils.h"
#include "pkgfile.h"

namespace cards {
class cards_level : public pkgfile
{
    void level();
    void generate_level(unsigned int& l);

public:
    cards_level();
    ~cards_level();
    void printout();
};

}  // endof cards namespace
