## cards - Package Management Utilities for NuTyX-Linux


### Description

**cards** which stands for *Create, Add, Remove and Download System* is a set of utilities
(pkgadd, pkgrm, pkginfo, pkgmk and cards), which are used for managing software packages in NuTyX-Linux.

*cards* is based on a fork of the *[CRUX-Linux](http://crux.nu) pkgutils and prt-get tools and is under active
development. [NuTyX-Linux Team](http://nutyx.org)


#### The most notable additions and improvements

* Full support for *binary packages*
* Support for sources from *version control system (VCS)* (Bazaar, Git, Subversion and Mercurial)


### Building and installing


#### To build the lib

```bash
$ make libs
```

#### To install the lib

```bash
$ make install-libs
```

#### To build all

```bash
$ make all
```

#### To build the pkgadd binary

```bash
$ make pkgadd
```

#### To install the binaries

```bash
$ make install
```

or

```bash
$ make DESTDIR=/some/other/path install
```

#### To get a quick overview

```bash
$ cards help
```


### Copyright

**cards** Copyright (c) 2013-2016 [NuTyX-Linux Team](http://nutyx.org) licensed under the GNU General Public License 2
or later.

Read the COPYING file for the complete license.


#### Licenses for incorporated software

* `pkgutils`: Copyright (c) 2000-2005 Per Liden and Copyright (c) 2006-2013 CRUX team <http://crux.nu> [GPL2/later]
* `prt-get`:  Copyright (c) 20002, 2004, 2005 Johannes Winkelmann (jw@tks6.net) [GPL2/later]
* `pacman`:   Copyright (c) 2015-2016 Pacman Development Team <https://projects.archlinux.org/pacman.git/> [GPL2/later]

