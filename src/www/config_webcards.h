// config_webcards.h
//
//  Copyright (c) 2016 - 2020 by NuTyX team (http://nutyx.org)
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
#define CSSDATA cout << " <style type=\"text/css\">" << endl \
 << " body{" << endl \
 << "   text-align: left;" << endl \
 << "   color: #444;" << endl \
 << "   background: #eee url(data:image/png;base64,iVBORw0KGgo\
AAAANSUhEUgAAAAUAAAAFCAMAAAC6sdbXAAAAGXRFWHRTb2Z0d2FyZQBBZG9iZS\
BJbWFnZVJlYWR5ccllPAAAAAZQTFRF3d3d////riJKgAAAAAJ0Uk5T/wDltzBK\
AAAAFUlEQVR42mJgBAEGGMmAxAYCgAADAAGGABmnk/7aAAAAAElFTkSuQmCC); " << endl \
 << "   font: 14px;" << endl \
 << "}" << endl \
 << "header {" << endl \
 << "   float: left;" << endl \
 << "   padding-top: 6px;" << endl  \
 << "   padding-left: 2%;" << endl \
 << "   margin-right: 2%;" << endl \
 << "   background-color: #606060;" << endl \
 << "}" << endl \
 << "nav { padding-top: 1px; }" << endl \
 << "/* horizontally center ul element */" << endl \
 << "nav ul { text-align:center; }" << endl \
 << "nav li {" << endl \
 << "    /* make list elements fall inline as block elements */" << endl \
 << "    position: relative;" << endl \
 << "    display: inline-block;" << endl \
 << "    /* next two lines only for display purposes */" << endl \
 << "    text-align: center;" << endl \
 << "    border:1px solid black;" << endl \
 << "}" << endl \
 << "nav li a {" << endl \
 << "    font-size:12px;" << endl \
 << "    text-decoration: none;" << endl \
 << "    float:left;" << endl \
 << "    padding:10px;" << endl \
 << "    background-color: #333333;" << endl \
 << "    color:#ffffff;" << endl \
 << "    border-bottom:1px;" << endl \
 << "    border-bottom-color:#000000;" << endl \
 << "    border-bottom-style:solid;" << endl \
 << "}" << endl \
 << "nav li a:hover {" << endl \
 << "    background-color:#9B1C26;" << endl \
 << "    border-bottom-color:#000000;" << endl \
 << "    border-bottom-style:solid;" << endl \
 << "}" << endl \
 << "img { border: 0; }" << endl \
 << "nav.img { border: 0; }" << endl \
 << "h1 { text-decoration: underline; }" << endl \
 << endl \
 << "p1 {" << endl \
 << "    display: inline-block;" << endl \
 << "    vertical-align: top;" << endl \
 << "    text-align: left;" << endl \
 << "    color:#585858;" << endl \
 << "    font-size:1em;" << endl \
 << "}" << endl \
 << "pre {" << endl \
 << "   padding: 0.5em 1em;" << endl \
 << "   margin: 1em 0em 1em 2em;" << endl \
 << "   font-weight:bold;" << endl \
 << "   color: black;" << endl \
 << "   font-family: monospace;" << endl \
 << "   background-color: #D0D0D0;" << endl \
 << "}" << endl \
 << "td { padding: 0.5em 0.5em 0.5em 0.5em; }" << endl \
 << "tr.header { color: white; background-color: #606060; }" << endl \
 << "tr.odd { background-color: #D0D0D0; }" << endl \
 << ".sidebar { background-color: #E0E0E0; }" << endl \
 << "pre.command { border-left: 0.2em solid #C70000; }" << endl \
 << "pre.command_user { border-left: 0.2em solid #00C700; }" << endl \
 << "pre.output { font-weight:normal; }" << endl \
 << "div.important {" << endl \
 << "   margin: 1em 2em 1em 2em;" << endl \
 << "   font-size: 1.2em;" << endl \
 << "   color:#883333;" << endl \
 << "   padding: 0.5em 0.5em 0.5em 0.5em;" << endl \
 << "   border: 2px solid #866;" << endl \
 << "}" << endl \
 << "div.note {" << endl \
 << "   margin: 1em 2em 1em 0em;" << endl \
 << "   font-size: 1em;" << endl \
 << "   color:#333388;" << endl \
 << "   border: 2px solid #dbddec;" << endl \
 << "   padding: 0.5em 0.5em 0.5em 0.5em;" << endl \
 << "}" << endl \
 << "div.toc {" << endl \
 << "   margin: 0em 0em 0em 0em;" << endl \
 << "   font-size: 0.9em;" << endl \
 << "   border: 1px solid #999999;" << endl \
 << "   padding: 0em 2em 0em 1em;" << endl \
 << "   width: -moz-max-content;" << endl \
 << "   background-color: #e9e9ef;" << endl \
 << "}" << endl \
 << "div.important img { padding: 0.4em; }" << endl \
 << "div.note img { padding: 0em 0.4em; }" << endl \
 << "div.pied_page { text-align: center; }" << endl \
 << "a { color: #F64825; }" << endl \
 << "#foot {" << endl \
 << "    text-align: center;" << endl \
 << "    text-decoration-color: #000000;" << endl \
 << "    font-size: 10px;" << endl \
 << "}" << endl \
 << "/* Last edited info */" << endl \
 << "p.updated {" << endl \
 << "    text-align: left;" << endl \
 << "    font-size: small;" << endl \
 << "    font-weight: bold;" << endl \
 << "    font-style: italic;" << endl \
 << "}" << endl \
 << "a:link {" << endl \
 << "    color: #703030;" << endl \
 << "    text-decoration: none;" << endl \
 << "}" << endl \
 << "a:visited {" << endl \
 << "    color: #0b0080;" << endl \
 << "}" << endl \
 << "a:hover {" << endl \
 << "    color: #403030;" << endl \
 << "    text-decoration: underline;" << endl \
 << "}" << endl \
 << "table {" << endl \
 << "    width: 100%;" << endl \
 << "}" << endl \
 << "td.menu {" << endl \
 << "    font: courrier;" << endl \
 << "    background-color: #959595;" << endl \
 << "    text-decoration: none;" << endl \
 << "    vertical-align:middle;" << endl \
 << "    text-align:center;" << endl \
 << "    height:60px;" << endl \
 << "}" << endl \
 << "td.menu:hover {" << endl \
 << "    text-decoration: none;" << endl \
 << "    background-color: #AAAAAA;" << endl \
 << "    text-transform: uppercase;" << endl \
 << "}" << endl \
 << "td.menu a:link {" << endl \
 << "    font-weight:bold;" << endl \
 << "    color: black;" << endl \
 << "    text-decoration: none;" << endl \
 << "    font-family: arial;" << endl \
 << "}" << endl \
 << "td.menu a:visited {" << endl \
 << "    font-weight:bold;" << endl \
 << "    color: black;" << endl \
 << "    text-decoration: none;" << endl \
 << "    font-family: arial;" << endl \
 << "}" << endl \
 << "td.menu a:hover {" << endl \
 << "    color: black;" << endl \
 << "    text-decoration: none;" << endl \
 << "    font-family: arial;" << endl \
 << "}" << endl \
 << " @media print { header, footer {display:none } }" << endl \
 << " </style>" << endl \
 << "</head>" << endl

#define FOOTERTEXT   cout << "<hr>" << endl \
  << "    <footer >" << endl \
  << "     <p id=\"foot\"> Powered by cards " <<  VERSION \
  << "     &copy; 2007 - 2017 " \
  << "<a href=\".\">NuTyX</a>.<br><br>" << endl \
  << "Hosted by <a href=\"http://tuxfamily.org\"><img src=\"../graphics/logo_tuxfamily_50.png\"/></a><br><br>" << endl \
  << "     <a href=\"http://www.wtfpl.net/\">" << endl \
  << "     <img src=\"http://www.wtfpl.net/wp-content/uploads/2012/12/wtfpl-badge-4.png\"" \
  << endl \
  << "     width=\"80\" height=\"15\" alt=\"WTFPL\" /></a> " << endl \
  << "     <a href=\"http://www.anybrowser.org/campaign/\">" << endl \
  << "       <img src=\"../graphics/abblue.gif\" width=\"80\" height=\"15\"" << endl \
  << "       alt=\"anybrowser\"></a>" << endl \
  << "    </footer>" << endl

#define HEADERTEXT  	cout << "Content-type:text/html\r\n\r\n" \
	<< "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\"" << endl \
	<< "     \"http://www.w3.org/TR/html4/strict.dtd\">" << endl \
	<< "<HTML>" << endl \
	<< "   <HEAD>" << endl \
	<< "      <TITLE>NuTyX GNU/Linux</TITLE>" << endl \
	<< "      <META charset=\"utf-8\" />" << endl \
	<< "      <LINK rel=\"shortcut icon\" href=\"../favicon.icon\">" \
	<< endl

#define SEARCH cout << "   <div>" << endl \
  << "  <form method=\"get\" action=\".\">" << endl \
  << "     <fieldset>" << endl \
  << "          <label for=\"search-field\">" \
	<< search << "</label>" << endl \
  << "          <input id=\"search-field\" type=\"text\" name=\"search\" \
size=\"18\" maxlength=\"200\" />" << endl \
	<< "     </fieldset>" << endl \
  << "  </form>" << endl \
  << "   </div>" << endl

#endif /* CONFIGWEBCARDS_H */
// vim:set ts=2 :
