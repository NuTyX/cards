## TODO

### extract_file() - clarify permissions

Clarify permissions set for extract_file(): [see issue](https://github.com/NuTyX/cards/issues/34)


### Add documentation / man page info for PKGMK variables

Add documentation / man page info for ALL PKGMK options.

Split them into 2 categories: 

* PKGMK.CONF VARIABLES: which can be set in the `pkgmk.conf`
* PKGMK INTERNAL VARIABLES: which should not be set in the `pkgmk.conf` but might be useful for developers.
    e.g. Pkgfiles


###  build_package(): find a better solution for the hardcoded folders

build_package(): find a better solution for the hardcoded folders [see issue](https://github.com/NuTyX/cards/issues/36)


### Add for VCS (version control source) code to auomaticaly update the Pkgfile version.


### Unify code style in `pkgmk.in`: backticks to parentheses style

Change all backticks to parentheses style [see](http://mywiki.wooledge.org/BashFAQ/082) 


### Unify code style in `pkgmk.in`: quotation

Prefer quoted ariables over unquoted.


### Unify code style in `pkgmk.in`: Quoting Variables

[see Quoting Variables](http://www.tldp.org/LDP/abs/html/quotingvar.html)

```
When referencing a variable, it is generally advisable to enclose its name in double quotes. This prevents 
reinterpretation of all special characters within the quoted string -- except $, ` (backquote), and \ (escape). 
```

### Remove unneeded or brocken options

e.g. pkgmk -cm, --check-md5sum [see issue](https://github.com/NuTyX/cards/issues/33)

but decide also on others.


### Re-write Documentation


### DIVERSE TODO

* Maybe: add Pkgfile syntax check for source=() array

* improve the pkginfo --runtimedepfiles functionnality

