Project description
===================

This is the tool to edit .dat, .spr and other files, used by "Frontiers of Fantasy" team for game development.

Building
========

The project uses two dependencies: wxWidgets (for the whole GUI) - http://www.wxwidgets.org/,
and Magick++ by ImageMagick (for some image operations) - http://www.imagemagick.org/Magick++/.

You will need to install these libraries to properly build the app.

### Windows

1.	Download latest stable wxWidgets release from here http://www.wxwidgets.org/downloads/ (originally v3.0.0 was used).  
		As far as I remember, you will still need to build it using Visual Studio to produce .lib files.  
		The solution should be located in build/msw subdirectory.

2.	Define environment variable WXHOME, pointing to wxWidgets root directory.

3.	Download ImageMagick from here http://www.imagemagick.org/script/binary-releases.php#windows.  
		Preferred download link is the first one.  
		Then use Visual Studio to build the solution VisualDynamicMT.sln (I preferred dynamic version), located at 'VisualMagick' subdirectory.
		
4.	Define environment variable IMAGE_MAGICK_HOME, pointing to ImageMagick root directory.

5. Run configure.bat, it will just create directories for output files.

6. Use Visual Studio (orinally VS 2013 was used) to open the project solution and build it.

7. You will need to manually copy 'res' from root directory into build directory. Later I should have build step for that.

8.	Also, manually copy some *.dll files from 'dlls' subdirectory into build directory.  
		You will need msvcp120.dll & msvcr120.dll only for release version; .dlls with '\_DB\_' in the filename are for debug version,
		while with '\_RL\_' are for release version.  
		Later I should have build step to do that automatically.

### Linux

1. Install wxWidgets. You may follow this guide for Ubuntu and Debian http://codelite.org/LiteEditor/WxWidgets30Binaries#toc2

2.	Install ImageMagick for development. For Debian-based distros like Ubuntu you may run
		```
		sudo apt-get install imagemagick libmagickcore-dev libmagickwand-dev
		```
		I'm not sure about other distros, but probably you'll have to download it from here http://www.imagemagick.org/script/binary-releases.php#unix
		and build from sources manually.
		
3.	Run './configure && make'. After build completes, executable will be located at 'bin/release'.  
		By default, release version is built. You may build debug version using 'make CONF=debug param'.  
		
4.	Alternatively you may use CMake. Run './configure' then go to 'cmake' subdir and run 'cmake ..' from terminal.
		After that run 'make'.  
		Executable will be located under 'cmake/release'.
