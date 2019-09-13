## TODO

### Add Option to define: no_unpack per source entry

[comment](https://github.com/NuTyX/cards/issues/48#issuecomment-191182092)

### pkgmk.in supports now gettext translation files

* Need to translate them
* Decide where final PO files should be installed and add: `TEXTDOMAIN`, `TEXTDOMAINDIR`

Maybe suggestion:

    * export TEXTDOMAIN='pkgmk'
    * export TEXTDOMAINDIR='/etc/cards/pkgmk/locale'

### Add documentation / man page info for PKGMK variables

Add documentation / man page info for ALL PKGMK options.

Split them into 2 categories:

* PKGMK.CONF VARIABLES: which can be set in the `pkgmk.conf`
* PKGMK INTERNAL VARIABLES: which should not be set in the `pkgmk.conf` but might be useful for developers.
    e.g. Pkgfiles

    * add info that PKGMK_ARCH variable exists: **currently I found 85 occurencies of `uname -m` in the official Pkgfiles.



### Unify code style in `pkgmk.in`: Quoting Variables

Prefer quoted variables over unquoted. [see Quoting Variables](http://www.tldp.org/LDP/abs/html/quotingvar.html)

```
When referencing a variable, it is generally advisable to enclose its name in double quotes.
This prevents  reinterpretation of all special characters within the
quoted string -- except $, ` (backquote), and \ (escape).
```


### Adjust documentation to include: ARCH i686

[see issue](https://github.com/NuTyX/cards/issues/35)


### Re-write Documentation


### CHECK: PKGMK.CONF VARIABLES: which can be set in the `pkgmk.conf`


### DIVERSE TODO

* Maybe: add Pkgfile syntax check for source=() array

* improve the pkginfo --runtimedepfiles functionnality

* Update Copyright to include: Johannes Winkelmann and Judd Vinet  man pages etc..

* PKGMK_VCS_HOLDVER: Decide maybe change it to hold the last set version

    Currently: When using VCS sources any currently checked out source will not be updated to the latest revision.

* `pkgmk.in` rewrite info *Pkgfile: source (array)* (the big block at the top of the file)
    in the documentation and remove it from the `pkgmk.in` file
    
