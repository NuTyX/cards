/*
 * pack_list.cxx
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

#include "pack_list.h"

PackList::PackList (int x, int y, int w, int h)
    : Fl_Pack(x,y,w,h)
{
    this->box(FL_DOWN_FRAME);
    this->begin();
    //lxde
    Fl_Button* lxde = new Fl_Button(0,0,52,52,"");
    Fl_Pixmap* lxde_Icon = new Fl_Pixmap(lxde_xpm);
    lxde->tooltip("Lxde : a lightweight desktop environment based on X11");
    lxde->image(lxde_Icon);
    lxde->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT);
    lxde->callback(&OnClickButton,Collections::LXDE);

    //Xfce
    Fl_Button* xfce = new Fl_Button(0,0,52,52,"");
    Fl_Pixmap* xfce_Icon = new Fl_Pixmap(xfce_xpm);
    xfce->tooltip("xfce : a lightweight desktop environment based on GTK+");
    xfce->image(xfce_Icon);
    xfce->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT);
    xfce->callback(&OnClickButton,Collections::XFCE);

    //lxqt
    Fl_Button* lxqt = new Fl_Button(0,0,52,52,"");
    Fl_Pixmap* lxqt_icon = new Fl_Pixmap(lxqt_xpm);
    lxqt->tooltip("lxqt : a lightweight desktop environment based on Qt");
    lxqt->image(lxqt_icon);
    lxqt->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT);
    lxqt->callback(&OnClickButton,Collections::LXQT);

    //Mate
    Fl_Button* Mate = new Fl_Button(0,0,52,52,"");
    Fl_Pixmap* Mate_Icon = new Fl_Pixmap(mate_xpm);
    Mate->tooltip("Mate : a Gnome 2 desktop environment fork, light and functional");
    Mate->image(Mate_Icon);
    Mate->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT);
    Mate->callback(&OnClickButton,Collections::MATE);

    //KDE
    Fl_Button* KDE = new Fl_Button(0,0,52,52,"");
    Fl_Pixmap* KDE_Icon = new Fl_Pixmap(kde_xpm);
    KDE->tooltip("KDE : a heavy powerfull Desktop Environment");
    KDE->image(KDE_Icon);
    KDE->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT);
    KDE->callback(&OnClickButton,Collections::KDE);
    this->end();
    resizable(0);
}

void PackList::OnClickButton(Fl_Widget* w, long p)
{
    set<string> Collec_List;
    Collec_List.insert("(xorg)");
    Collec_List.insert("(lxdm)");
    CWrapper* Cards = CWrapper::instance();
    switch((Collections)p)
    {
        case LXDE:
        {
            Collec_List.insert("(lxde)");
            Collec_List.insert("(lxde-extra)");
            break;
        }
        case XFCE:
        {
            Collec_List.insert("(xfce4)");
            Collec_List.insert("(xfce4-extra)");
            break;
        }
        case LXQT:
        {
            Collec_List.insert("(lxqt)");
            Collec_List.insert("(lxqt-extra)");
            break;
        }
        case MATE:
        {
            Collec_List.insert("(mate)");
            Collec_List.insert("(mate-extra)");
            break;
        }
        case GNOME:
        {
            Collec_List.insert("(gnome)");
            Collec_List.insert("(gnome-extra)");
            break;
        }
        case KDE:
        {
            Collec_List.insert("(kde5)");
            Collec_List.insert("(kde5-extra)");
            break;
        }
        default:
        {
            break;
        }
    }
    vector<CPackage*> Packages = Cards->getPackageList();
    for (CPackage* Package : Packages)
    {
        if ((!Package->isInstalled()) && (Collec_List.count(Package->getCollection())))
        {
            Package->setStatus(TO_INSTALL);
        }
    }
    Cards->refreshJobList();
}
