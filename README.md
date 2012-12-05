[liburi](https://github.com/nevali/liburi) is a simple interface for parsing
URIs. Under the hood, the actual URI parsing is handled by
[uriparser](http://uriparser.sourceforge.net/) -- liburi aims to provide
an API which is easier to work with than uriparser's.

liburi provides:

* The ability to parse [IRIs](http://tools.ietf.org/html/rfc3987)
* Accessor methods for each parsed URI component and information about the URIs

liburi is licensed under the Apache License 2.0, while uriparser is licensed
under the [New BSD license](http://uriparser.git.sourceforge.net/git/gitweb.cgi?p=uriparser/uriparser;a=blob;f=COPYING).

### Building from git

You will need git, automake, autoconf and libtool. You'll also need a
DocBook 5 and DocBook-XSL toolchain (i.e., xsltproc and the stylesheets)
if you wish to rebuild the documentation. Both liburi and uriparser have
minimal external dependencies.

    $ git clone git://github.com/nevali/liburi.git
    $ cd liburi
    $ git submodule update --init --recursive
    $ autoreconf -i
    $ ./configure --prefix=/some/path
    $ make
    $ sudo make install
