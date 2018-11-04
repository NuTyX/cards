/*
 * cards_event_handler.cxx
 *
 * Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#include "cards_event_handler.h"

namespace cards
{
    /// Convert Reason Code Enum to human readable text
    const string CEventHandler::getReasonCodeString(const CEH_RC rc)
    {
        switch (rc)
        {
            case CEH_RC::OK:
            {
                return "Finished";
                break;
            }
            case CEH_RC::NO_ROOT:
            {
                return "This application need root privileges, please restart it with super user rights!";
                break;
            }
            case CEH_RC::EXCEPTION:
            {
                return "An exception occured during cardslib operation. See messages above to know the reason";
                break;
            }
            default:
            {
                return "Unknown reason... Please contact Nutyx team to signal this case";
                break;
            }
        }
    }
}
