/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#define HEADERTEXT  	cout << "Content-type:text/html\r\n\r\n" \
	<< "<!DOCTYPE html>\n" \
	<< "<html>\n" \
	<< "   <head>\n" \
	<< "      <title>NuTyX GNU/Linux</title>\n" \
	<< "      <meta charset=\"utf-8\" />\n" \
	<< "      <link rel=\"shortcut icon\" href=\"../favicon.ico\">\n" \
	<< "      <link href=\"../style.css\" rel=\"stylesheet\">\n" \
	<< "   </head>" \
	<< endl

#define SEARCH cout << "   <div>\n" \
  << "  <form method=\"get\" action=\".\">\n" \
  << "     <fieldset>\n" \
  << "          <label for=\"search-field\">" \
	<< search << "</label>\n" \
  << "          <input id=\"search-field\" type=\"text\" name=\"search\" \
size=\"18\" maxlength=\"200\" />\n" \
	<< "     </fieldset>\n" \
  << "  </form>\n" \
  << "   </div>\n"

