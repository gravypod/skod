Skod - FTP client for humans
=============================

Skod is a (File|Transfer|Protocol) client for humans, easy to use and hackable.
Written in pure C, without any external library.

Featuers
==========

* Highlight output.
* Full support for IPv6
* bash like commands

Platforms
==========

* linux - (any)
* Mac   - (Not tested)

Build
======

> Run `make` to compile skod.

Usage
=======

By default skod will try to login as anonymous user,
but you can use `-u, -p` to provide username, and password.

skod is user-freindly and easy to use, follow the screenshots:

##### skod -s localhost -u root -p root --list "/"

![list](https://github.com/Hypsurus/skod/wiki/screenshots/skod_list.png)

#####  skod -s localhost -u root -u pass --download "/file.ogv"

![download](https://github.com/Hypsurus/skod/wiki/screenshots/skod_download.png)

##### skod --help

![options](https://github.com/Hypsurus/skod/wiki/screenshots/skod_options.png)


Contributing
=============

Contributions are very welcome!

1. Read the 'HACKING' file!
2. clone the repository (git clone git@github.com:USERNAME/skod.git)
3. make your changes
5. add yourself to contributors.txt
4. push the repository
5. make a pull request

Thank you - and happy contributing!

Thanks
=======

* Thanks to Frank Denis (@jedisct1) on his code in FTP-map.

Copying
========

> Copyright 2015 (C) Hypsurus <hypsurus@mail.ru>.
> License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.
