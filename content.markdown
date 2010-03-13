City of the Condemned
=====================
_Seven Day Roguelike Challenge 2010 Entry._

[![CotC gameplay](pics/cotc_thumb.png "Optional title")](pics/cotc.png)


The game depicts a battle between the armies of
Heaven and Hell in a town where the souls of
innocent people are in danger.

Play as an angel, a divine warrior of God, with the
mission to vanquish all evil and protect the humans.

Or take your stand as a demon, sinister creation of the Devil
able to possess the people to create chaos.

## Play ##
Use your favourite ssh client and do following:

    $ ssh gamer@saluuna.dy.fi
    
The password is 'curse'.

## Download ##
You can download this project in either
[zip](http://github.com/tapio/cotc/zipball/master "zip") or
[tar](http://github.com/tapio/cotc/tarball/master "tar") formats.
The package includes 32bit binaries for Windows and Linux.

You can also clone the project with [Git](http://git-scm.com "Git") by running:

    $ git clone git://github.com/tapio/cotc

## Compilation ##
If you wish to compile the game yourself, you need curses (PDcurses on Windows, ncurses on Linux) and Boost headers. CMake can be used to build the game, but on Windows, you need to configure it to find PDcurses. Normal CMake build procedure is as follows (Linux example):

    $ mkdir build && cd build && cmake .. && make && ./CotC

If you want to compile without CMake, execute generate-version.sh to create the version.h header. Then it's just something like this (Windows example):

    $ g++ *.cc -O2 -o CotC.exe -lpdcurses

