// menu_webcards.h
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
#ifndef MENUWEBCARDS_H
#define MENUWEBCARDS_H

#define MENUFR cout << "<body>" << endl \
  << "    <table class=\"page\">" << endl \
  << "     <tr>\n" << endl \
  << "      <td class=\"logo\">" << endl \
  << "       <a href=\".\">" << endl \
  << "        <img src=\"../graphics/logo_nutyx_120.png\" alt=\"NuTyX Reload\" height=\"85\" width=\"60\" class=\"center\">" << endl \
  << "       </a>" << endl \
  << "      </td>" << endl \
  << "      <td class=\"top-menu\">" << endl \
  << "       <table class=\"top-menu\">" << endl \
  << "        <tr>" << endl \
  << "            <td class=\"menu\"><a class=\"menu\" href=\"news\">Nouvelles</a>" << endl \
  << "            <td class=\"menu\"><a class=\"menu\" href=\"downloads\">Téléchargements</a>"  << endl \
  << "            <td class=\"menu\"><a class=\"menu\" href=\"documentation\">Documentation</a>" << endl \
  << "            <td class=\"menu\"><a class=\"menu\" href=\"packages\">Paquets</a>" << endl \
  << "        </tr>" << endl \
  << "        <tr>" << endl \
  << "            <td class=\"menu\"><a class=\"menu\" href=\"faq\">FAQ</a>" << endl \
  << "            <td class=\"menu\"><a class=\"menu\" href=\"information\">Information</a>" << endl \
  << "            <td class=\"menu\"><a class=\"menu\" href=\"screenshots\">Captures d'écrans</a>"  << endl \
  << "            <td class=\"menu\"><a href=\"../en\" title=\"Home\"> <img src=\"../graphics/gb.gif\" alt=\"Home\" height=\"25\" width=\"60\" class=\"center\">" << endl \
  << "           </a>" << endl \
  << "        </tr>" << endl \
  << "       </table>" << endl \
  << "      </td>" << endl \
  << "     </tr>" << endl \
  << "     <tr>" << endl

#define MENUEN cout << "<body>" << endl \
  << "    <table class=\"page\">" << endl \
  << "     <tr>\n" << endl \
  << "      <td class=\"logo\">" << endl \
  << "       <a href=\".\">" << endl \
  << "        <img src=\"../graphics/logo_nutyx_120.png\" alt=\"NuTyX Reload\" height=\"85\" width=\"60\" class=\"center\">" << endl \
  << "       </a>" << endl \
  << "      </td>" << endl \
  << "      <td class=\"top-menu\">" << endl \
  << "       <table class=\"top-menu\">" << endl \
  << "        <tr>" << endl \
  << "            <td class=\"menu\"><a class=\"menu\" href=\"news\">News</a>" << endl \
  << "            <td class=\"menu\"><a class=\"menu\" href=\"downloads\">Downloads</a>"  << endl \
  << "            <td class=\"menu\"><a class=\"menu\" href=\"documentation\">Documentation</a>" << endl \
  << "            <td class=\"menu\"><a class=\"menu\" href=\"packages\">Packages</a>" << endl \
  << "        </tr>" << endl \
  << "        <tr>" << endl \
  << "            <td class=\"menu\"><a class=\"menu\" href=\"faq\">FAQ</a>" << endl \
  << "            <td class=\"menu\"><a class=\"menu\" href=\"information\">Information</a>" << endl \
  << "            <td class=\"menu\"><a class=\"menu\" href=\"screenshots\">Screenshots</a>"  << endl \
  << "            <td class=\"menu\"><a href=\"../fr\" title=\"Home\"> <img src=\"../graphics/fr.gif\" alt=\"Home\" height=\"25\" width=\"60\" class=\"center\">" << endl \
  << "           </a>" << endl \
  << "        </tr>" << endl \
  << "       </table>" << endl \
  << "      </td>" << endl \
  << "     </tr>" << endl \
  << "     <tr>" << endl

#endif /* MENUWEBCARDS_H */
// vim:set ts=2 :
