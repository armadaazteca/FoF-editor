Project description
===================

The new editor for .dat / .spr files based on Tibia format.

Building
========

### Windows
===========

Run 'configure.bat' to create directory structure for temporary files and executables.
Use Visual Studio 2013 to open project and build it.

### Linux
=========
Run './configure && make' as usual, there's no 'make install' yet, binary will be located at 'bin/release'.
Alternatively you may use CMake. Go to 'cmake' subdir and run 'cmake ..' from terminal, binary will be located at 'cmake/release'.
