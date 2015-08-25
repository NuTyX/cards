#include "pkgrepo.h"
#include "archive_utils.h"
#include <pwd.h>
#include <ck-connector.h>
#include <set>
#include <iostream>
#include <string>
#include <glib.h>
#include <glib/gstdio.h>


using namespace std;


int main (int argi, char * argv[])
{
	try {
		CkConnector *ckc = NULL;
		// initialisation
		if( ckc != NULL ) {
			DBusError error;
			dbus_error_init(&error);
			ck_connector_close_session(ckc, &error);
			if(dbus_error_is_set(&error)) {
				dbus_error_free(&error);
			}
			ck_connector_unref(ckc);
			ckc=NULL;
		}
		// free
		if (ckc) {
			ck_connector_unref(ckc);
			ckc=NULL;
		}
		// new connection
		if (!ckc) {
			ckc = ck_connector_new();
		}
		if (ckc != NULL) {
			struct passwd *pw = getpwnam("thierry");
			cout << pw->pw_uid << endl;
			cout << &pw->pw_uid << endl;
			DBusError error;
			char x[256], *d, *n;
			gboolean is_local=TRUE;
			sprintf(x, "/dev/tty%d", 2);
			dbus_error_init(&error);
			d = x; n = getenv("DISPLAY");
			cout << &d << endl;
			cout << &n << endl;
			cout << &is_local << endl;
			if (ck_connector_open_session_with_parameters(ckc, &error,
					"unix-user", &pw->pw_uid,
					"x11-display-device", &d,
					"x11-display", &n,
					"is-local",&is_local,
					NULL)) {
						cout << YELLOW << "ConsoleKit Session actif" << NORMAL << endl;
						setenv("XDG_SESSION_COOKIE", ck_connector_get_cookie(ckc), 1);
			} else {
				cerr << RED << "create ConsoleKit session fail" << NORMAL << endl;
			}
		} else {
			cout << YELLOW << "Cannot initialize ConsoleKit Session" << NORMAL << endl;
		}
	}  catch (runtime_error& e) {
			cerr << e.what() << endl;
			return -1;
	}
	return 0;
}
// vim:set ts=2 :
