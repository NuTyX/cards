#! /usr/bin/gawk -f
#
#  cards
#
#  Copyright (c) 2007-2013 by NuTyX team (http://nutyx.org)

#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, 
#  USA.
#

function help()
{
   print "Usage: deps [list [TGT]]"
   print "            [scan [TGT]]"
   print "TGT: pakxe | attapu"
}


function init()
{
   
#   if (ARGV[1]=="-d") {
#      TRC = 1
#      OPR = (ARGC>2) ? ARGV[2] : "scan"
#      TGT = (ARGC>3) ? ARGV[3] : "pakxe"
#   } else {
#      OPR = (ARGC>1) ? ARGV[1] : "scan"
#      TGT = (ARGC>2) ? ARGV[2] : "pakxe"
#   }
   OPR = ARGV[1]
   TGT = "pakxe"
   if (ARGV[2] =="depend") { DepTyp = "Depends on:" }
   if (ARGV[2] =="run")  { DepTyp = "Run on:" }
   "whoami" | getline WHO; close("whoami")
   REPO   = "/usr/ports/"
   ERR    = "ERR." TGT
   PDEPS  = "PKGDEPS"
   PNIV   = "NIV." TGT
}


function list_DEPS(   Deps, tb_sD, i, n)
{
   printf "" > PDEPS
   load_DEPS("list")
   for (Deps in tb_DEPS)
      tb_sD[++i]=sprintf("%-30s : %s", Deps, tb_DEPS[Deps])
   n=asort(tb_sD)
   for (i=1;i<=n;i++)
      print tb_sD[i] >> PDEPS
   close(PDEPS)
}


function list_NIV(Niv,    s, Pkg, nb_Pkg, n, i , tb_NIV_S)
{
   printf "%d", Niv >> PNIV
#  if (TRC) {
#     if (length(tb_NIV)==nbtot_Pkg)
#        printf "(%d)", length(tb_DEPS) >> PNIV
#     else
#        printf "(%d)", length(tb_NIV)-nbtot_Pkg >> PNIV
#  }
   for (Pkg in tb_NIV)
      tb_NIV_S[Pkg] = Pkg
   n = asort(tb_NIV_S)
   s=","
   for (i=1;i<=n;i++) {
      Pkg=tb_NIV_S[i]
      if (tb_NIV[Pkg]==Niv) {
         nb_Pkg++
         printf "%c%s", s, Pkg >> PNIV
         s=","
      }
   }
   printf ",%d", nb_Pkg >> PNIV
   nbtot_Pkg += nb_Pkg
   printf "\n\n" >> PNIV
   return nb_Pkg
}


function load_DEPS(Oper,   j, q, cmd, PkgF, pk, found_DependsOn, d, n)
{
   j=split(REPO,q,"/")+1
   cmd="find " REPO " -name Pkgfile"
   while (cmd | getline PkgF) {
      split(PkgF,pk,"/")
      if (Oper=="scan")
         tb_PKG[pk[j]]=pk[j-1]
      found_DependsOn="n"
      while (getline < PkgF) {
         if ($0 ~ DepTyp) {
            found_DependsOn="y"
            split($0,d,":")
            gsub("\t"," ",d[2])
            gsub("  "," ",d[2])
            gsub(", ",",",d[2])
            gsub(" ",",",d[2])
            gsub(",,",",",d[2])
            gsub("^,","",d[2])
            gsub(",$","",d[2])
            n=split(d[2],q,",")
            if (n==0)
               tb_NIV[pk[j]]=0
            else 
               tb_DEPS[pk[j]]=d[2]
         }
      }
      if (found_DependsOn=="n")
         tb_NIV[pk[j]]=0
      close(PkgF)
   }
   close (cmd)
}


function scan_DEPS(   k, Niv, Pkg, thisLevel, n, a, i, z, tb_Add2ThisLvl)
{
   if ((getline < PDEPS) <0) 
      list_DEPS()
   close(PDEPS)
   load_DEPS("scan")
   printf "" > PNIV 
   printf("%-24s %s\n", "unknown deps", "Package")  > ERR
   printf("%-24s %s\n", "------------", "-------") >> ERR
   k=list_NIV(0)
   for (Niv=1;;Niv++) {
      for (Pkg in tb_DEPS) {
         if (TRC)
            print Niv, "\033[1;33m" Pkg "\033[1;0m"
         thisLevel=1
         n=split(tb_DEPS[Pkg],a,",")
         for (i=1; i<=n; i++) {
            if (!(a[i] in tb_PKG))
               tb_ERR[++z]=sprintf("%-24s %s", a[i], Pkg)
            else 
               if (!(a[i] in tb_NIV)) {
                  thisLevel=0
                  if (TRC)
                     print "     \033[1;31m" a[i] "\033[1;0m"
                  else
                     break
               } else 
                  if (TRC)
                     print "  ", tb_NIV[a[i]], "\033[1;32m" a[i] "\033[1;0m "
         }
         if (thisLevel==1)
            tb_Add2ThisLvl[Pkg]=Niv
      }
      for (Pkg in tb_Add2ThisLvl) {
         tb_NIV[Pkg]=Niv
         delete tb_DEPS[Pkg]
      }
      delete tb_Add2ThisLvl
      k=list_NIV(Niv)
      if (k==0)
         break
   }
   n=asort(tb_ERR)
   for (i=1; i<=n; i++)
      if (tb_ERR[i]!=tb_ERR[i-1])
         print tb_ERR[i] >> ERR
   close(PNIV)
   close(ERR)
}


BEGIN {
   init()
   if      (OPR=="list") list_DEPS()
   else if (OPR=="scan") scan_DEPS()
   else                  help()
}
# End of file
