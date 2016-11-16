// menu_webcards.h
//
//  Copyright (c) 2016 by NuTyX team (http://nutyx.org)
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
  << "    <table>" << endl \
  << "     <tr>" << endl \
  << "      <td>" << endl \
  << "       <a href=\".\"><img id=\"logo\" src=\"../graphics/logo_nutyx_120.png\" alt=\"logo nutyx\" /></a>" << endl \
  << "      </td>" << endl \
  << "      <td>" << endl \
  << "       <table class=\"sidebar\" >" << endl \
  << "        <tr>" << endl \
  << "            <td class=\"menu\"><br><a href=\"news\">Nouvelles</a><br><br>" << endl \
  << "            <td class=\"menu\"><br><a href=\"information\">Information</a><br><br>" << endl \
  << "            <td class=\"menu\"><br><a href=\"documentation\">Documentation</a><br><br>" << endl \
  << "            <td class=\"menu\"><br><a href=\"installation\">Installation</a><br><br>" << endl \
  << "            <td class=\"menu\"><br><a href=\"packages\">Paquets</a><br><br>" << endl \
  << "            <td class=\"menu\"><br><a href=\"faq\">FAQ</a><br><br>" << endl \
  << "        </tr>" << endl \
  << "        <tr>" << endl \
  << "            <td class=\"menu\"><br><a href=\"http://downloads.nutyx.org/?C=M;O=D\">Téléchargements</a><br><br>"  << endl \
  << "            <td class=\"menu\"><br><a href=\"http://forum.nutyx.org/index.php?action=recent\">Forum</a><br><br>" << endl \
  << "            <td class=\"menu\"><br><a href=\"http://github.com/NuTyX\">Code</a><br><br>" << endl \
  << "            <td class=\"menu\"><br><a href=\"screenshots\">Captures d'écrans</a><br><br>"  << endl \
  << "            <td class=\"menu\"><br><a href=\"wallpapers\">Fonds d'écrans</a><br><br>"  << endl \
  << "            <td class=\"menu\"><br><a href=\"../en\" title=\"Home\"> <img src=\"../graphics/gb.gif\" alt=\"Home\" height=\"12\" width=\"35\"></a><br><br>" << endl \
  << "        </tr>" << endl \
  << "       </table>" << endl \
  << "      </td>" << endl \
  << "     </tr>" << endl \
  << "    </table>" << endl \
  << "   <hr>" << endl

#define MENUTR cout << "<body>" << endl \
  << "    <table>" << endl \
  << "     <tr>" << endl \
  << "      <td>" << endl \
  << "       <a href=\".\"><img id=\"logo\" src=\"../graphics/logo_nutyx_120.png\" alt=\"logo nutyx\" /></a>" << endl \
  << "      </td>" << endl \
  << "      <td>" << endl \
  << "       <table>" << endl \
  << "        <tr>" << endl \
  << "            <td class=\"menu\"><br><a href=\"news\">Haberler</a><br><br>" << endl \
  << "            <td class=\"menu\"><br><a href=\"information\">Bilgilendirme</a><br><br>" << endl \
  << "            <td class=\"menu\"><br><a href=\"documentation\">Dökümantasyon</a><br><br>" << endl \
  << "            <td class=\"menu\"><br><a href=\"installation\">Kurulum</a><br><br>" << endl \
  << "            <td class=\"menu\"><br><a href=\"packages\">Paketler</a><br><br>" << endl \
  << "            <td class=\"menu\"><br><a href=\"faq\">Sık Sorulan Sorular</a><br><br>" << endl \
  << "        </tr>" << endl \
  << "        <tr>" << endl \
  << "            <td class=\"menu\"><br><a href=\"http://downloads.nutyx.org/?C=M;O=D\">Indirmeler</a><br><br>"  << endl \
  << "            <td class=\"menu\"><br><a href=\"http://forums.nutyx.org/index.php?action=recent\">Forum</a><br><br>" << endl \
  << "            <td class=\"menu\"><br><a href=\"http://github.com/NuTyX\">Kaynak Kod</a><br><br>" << endl \
  << "            <td class=\"menu\"><br><a href=\"screenshots\">Screenshots</a><br><br>"  << endl \
  << "            <td class=\"menu\"><br><a href=\"wallpapers\">Wallpapers</a><br><br>"  << endl \
  << "            <td class=\"menu\"><br><a href=\"../en\" title=\"Home\"> <img src=\"../graphics/gb.gif\" alt=\"Home\" height=\"12\" width=\"35\"></a><br><br>" << endl \
  << "        </tr>" << endl \
  << "       </table>" << endl \
  << "      </td>" << endl \
  << "     </tr>" << endl \
  << "    </table>" << endl \
  << "   <hr>" << endl


#define MENUEN cout << "<body>" << endl \
  << "    <table>" << endl \
  << "     <tr>" << endl \
  << "      <td>" << endl \
  << "       <a href=\".\"><img id=\"logo\" src=\"../graphics/logo_nutyx_120.png\" alt=\"logo nutyx\" /></a>" << endl \
  << "      </td>" << endl \
  << "      <td>" << endl \
  << "       <table>" << endl \
  << "        <tr>" << endl \
  << "            <td class=\"menu\"><br><a href=\"news\">News</a><br><br>" << endl \
  << "            <td class=\"menu\"><br><a href=\"information\">Information</a><br><br>" << endl \
  << "            <td class=\"menu\"><br><a href=\"documentation\">Documentation</a><br><br>" << endl \
  << "            <td class=\"menu\"><br><a href=\"installation\">Installation</a><br><br>" << endl \
  << "            <td class=\"menu\"><br><a href=\"packages\">Packages</a><br><br>" << endl \
  << "            <td class=\"menu\"><br><a href=\"faq\">FAQ</a><br><br>" << endl \
  << "        </tr>" << endl \
  << "        <tr>" << endl \
  << "            <td class=\"menu\"><br><a href=\"http://downloads.nutyx.org/?C=M;O=D\">Downloads</a><br><br>"  << endl \
  << "            <td class=\"menu\"><br><a href=\"http://forums.nutyx.org/index.php?action=recent\">Forum</a><br><br>" << endl \
  << "            <td class=\"menu\"><br><a href=\"http://github.com/NuTyX\">Code</a><br><br>" << endl \
  << "            <td class=\"menu\"><br><a href=\"screenshots\">Screenshots</a><br><br>"  << endl \
  << "            <td class=\"menu\"><br><a href=\"wallpapers\">Wallpapers</a><br><br>"  << endl \
  << "            <td class=\"menu\"><br><a href=\"../fr\" title=\"Acceuil\"> <img src=\"../graphics/fr.gif\" alt=\"Accueil\" height=\"12\" width=\"35\"></a><br><br>" << endl \
  << "        </tr>" << endl \
  << "       </table>" << endl \
  << "      </td>" << endl \
  << "     </tr>" << endl \
  << "    </table>" << endl \
  << "   <hr>" << endl

#endif /* MENUWEBCARDS_H */
// vim:set ts=2 :
