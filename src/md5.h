/* SPDX-License-Identifier: LGPL-2.1-or-later */
//  Copyright (c) by Christophe <Devine devine at cr0 dot net>

#pragma once

#include "define.h"

#include <string.h>

struct md5_context {
    uint32 total[2];
    uint32 state[4];
    uint8 buffer[64];
};

void md5_starts(struct md5_context* ctx);
void md5_update(struct md5_context* ctx, uint8* input, uint32 length);
void md5_finish(struct md5_context* ctx, uint8 digest[16]);
