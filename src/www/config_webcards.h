// config_webcards.h
//
//  Copyright (c) 2016 - 2022 by NuTyX team (http://nutyx.org)
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
#ifndef CONFIGWEBCARDS_H
#define CONFIGWEBCARDS_H

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

#define FOOTERTEXT   cout << "\n      </td>\n     </tr>\n    </table>\n" \
  << "    <footer>\n" \
  << "     <p> Powered by cards " <<  VERSION \
  << "     &copy; 2007 - 2024 " \
  << "<a href=\".\">NuTyX</a>.<br><br>\n" \
  << "Hosted by <a href=\"http://tuxfamily.org\"><img src=\"../graphics/logo_tuxfamily_50.png\"/></a><br><br>\n" \
  << "     <a href=\"http://www.wtfpl.net/\">\n" \
  << "     <img src=\"../graphics/logo_wtfpl_80.png\"\n" \
  << "     width=\"80\" height=\"15\" alt=\"WTFPL\" /></a> \n" \
  << "    </footer>\n"


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

#endif /* CONFIGWEBCARDS_H */
// vim:set ts=2 :
