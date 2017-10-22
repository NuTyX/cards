#include "cards_event_handler.h"

const string Cards_event_handler::getReasonCodeString(const CEH_RC rc)
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
	default:
		{
			return "Unknown reason... Please contact Nutyx team to signal this case";
			break;
		}
    }
}
