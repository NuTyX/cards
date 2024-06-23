/* SPDX-License-Identifier: LGPL-2.1-or-later */
// Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
// Copyright 2017 - .... Thierry Nuttens <tnut@nutyx.org>

#include "cards_event_handler.h"


namespace cards
{
    /// Convert Reason Code Enum to human readable text
    const std::string cards_event_handler::getReasonCodeString(const CEH_RC rc)
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
                fl_alert("This application need root privileges.\nPlease restart it with super user rights !");
                return "This application need root privileges, please restart it with super user rights !";
                break;
            }
            case CEH_RC::EXCEPTION:
            {
                fl_alert("An exception occured during cardslib operation.\nSee messages above to know the reason.");
                return "An exception occured during cardslib operation. See messages above to know the reason.";
                break;
            }
            default:
            {
                fl_alert("Unknown reason...\nPlease report this case to https://bugs.nutyx.org.");
                return "Unknown reason... Please report this case to https://bugs.nutyx.org.";
                break;
            }
        }
    }
}
