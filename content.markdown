City of the Condemned
=====================
_Seven Day Roguelike Challenge 2010 Entry._

[![CotC gameplay](pics/cotc_thumb.png "CotC Gameplay Screenshot")](pics/cotc.png)
&nbsp;&nbsp;
[![7DRL Medal](pics/Medal_7DRL_2010_s.png "7DRL 2010 Medal")](http://roguebasin.roguelikedevelopment.org/index.php?title=7DRL_Contest_2010)


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
You can download the original 7DRL version (with added 32 bit executables for Windows and Linux) in either
[zip](http://github.com/tapio/cotc/zipball/7drl "zip") or
[tar](http://github.com/tapio/cotc/tarball/7drl "tar") formats.

For latest version, use these:
[zip](http://github.com/tapio/cotc/zipball/master "zip") |
[tar](http://github.com/tapio/cotc/tarball/master "tar")

You can also clone the project with [Git](http://git-scm.com "Git") by running:

    $ git clone git://github.com/tapio/cotc

## Compilation ##
(Applies to latest version.)  
If you wish to compile the game yourself, you need curses (PDcurses on Windows, ncurses on Linux) and Boost headers. You can use CMake to build the game. Normal build procedure is as follows (Linux example):

    $ mkdir build && cd build && cmake .. && make && ./CotC

If you want to compile without CMake, execute generate-version.sh to create the version.h header. Then it's just something like this (Windows example):

    $ g++ *.cc -O2 -o CotC.exe -lpdcurses

Note that generate-version.sh needs git, wc and cmp in your path - if that is too much, you can just add VERSION_STRING define to the compiler.
