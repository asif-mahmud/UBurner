This file contains detailed instructions about Compiling, Building, Installing and about Making installers.
These are platform specific, but to spare the pain CMake comes in handy.

#Prerequisites : #

-------------------------

*	Windows(XP, Vista, 7, 8) (If you change the versions change the next steps accordingly)
	*	MinGW 4.4 or later
	*	Qt4 Open source Libraries(4.8.5)
	*	CMake 2.8 or later
	*	NSIS (Only if you intend to make installer)

*	Linux
	*	gcc and g++
	*	libqt4-dev
	*	libudev-dev
	*	cmake(2.8 or later)

*	Apple
	*	Yet undocumented ***FIX ME***

#Environment Setup : #

-----------------------

##Windows(XP, Vista, 7, 8) :##

Install the prerequisites under C:\QT directory and do the followings -

* 	Add the following variables in user variables of environmental variable -

	1.	QTDIR --> C:\QT\4.8.5
	2.	GCCDIR --> C:\QT\MinGW
	3. 	QMAKESPEC ---> win32-g++

*	Add the following line in the user level PATH variable in environmental variable -

```
;%QTDIR%\bin;%GCCDIR%\bin;%GCCDIR%\lib;C:\QT\CMake 2.8\bin;C:QT\NSIS;
```

Now you are ready to Compile or Build or Make and Installer for your Windows PC.

##Linux  :##

Install the prerequisites according to your distro. 

i.e for Ubuntu users -

```
sudo apt-get install libudev-dev g++ gcc libqt4-dev cmake
```

##Apple :##

Yet undocumented.***FIX ME***


#Compile and Build :#

---------------------------

Compiling or building is devided into two types -

1.	***DEVEL :*** If you intend to develop the software use this type
2.	***RELEASE :*** if you intend to install or make installers use this type

You ***MUST*** define this when you run cmake for the first time. Use 

```
cmake -D OP_BUILD_TYPE=RELEASE
``` 

to use ***RELEASE*** type or 

```
cmake -D OP_BUILD_TYPE=DEVEL
```

to use ***DEVEL*** type.

Compiling or building procedure is the same for all platforms. Here it is -

1.	Extract the source in a directory. this will create a directory named like opprog-0.0.2
2.	Open a Terminal or Command prompt inside that directory
3.	type -

```
mkdir build && cd build
```

4.	Now type -

```
cmake -D OP_BUILD_TYPE=DEVEL ..
```

or

```
cmake -D OP_BUILD_TYPE=RELEASE ..
```

5.	Now run make -

```
make
```

That's all !!!


#Install : #

-------------------

For Windows users there should an Installer in the Download section. 

For Linux users do the following -

1.	Follow the steps described in ***Compile and Build*** Section and keep that terminal open.
2.	Type -

```
sudo make install
```

or 

```
su 
make install
```

according to your distro.


#Uninstall :#

-------------------

Windows user can find uninstallers both in Start Menu and in Control panel --> Add or Remove Programs.

Linux users ***SHOULD*** keep the source directory where it was built, the build directory as well.
Open a Terminal in the build directory and type -

```
sudo make uninstall
```

or 

```
su
make uninstall
```

This should uninstall the program and the gnome menu.

#Run :#

----------------

Windows users will find the short cut both in desktop and in start menu.

Linux user will find a gnome menu named ***Open Programmer GUI*** under the following submenus -

*	Qt
*	Developer
*	Development
*	Programming
*	Embedded

#Make Installer :#

------------------

##Windows :##

After building the software in ***RELEASE*** build type run (YOU ***MUST*** have ***NSIS*** installed)-

```
cpack -C Release
```

This will make an installer in _CPack_Packages folder structure.

##Linux :##

CPack builds wrong DEB file. So unfortunately no installers for Linux users right now.
