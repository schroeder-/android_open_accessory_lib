# Android Open Accessory Lib

## Intro
To use this copy "openaccessory.cpp" and "openaccessory.hpp" into your project.
For an example for the Nexus S just type "make".
## Android
For more information about the Android Software look at the android dir.
## Linux
you need to install libusb-1.0 or libusbx from your package manager.
now you can compile, take a look in the Makefile how to compile this files.
to use the example you might change the device pid and vid to your phone.
## Windows MinGW
to use this libary you need libusbx. 
* Download the latest windows binary from [libusbx](http://libusbx.org/). From MinGW32/dll or MinGW64/dll copy libusb-1.0.dll and libusb-1.0.dll.a to your project folder. 
* Also copy the libusb.h header to your source code.

to use the example you might change the device pid and vid to your phone.

at the first time time your device is changed into open accessory windows won't find a driver.
go into the hardware manager and select the "windows driver" folder from this project. 

now you can enjoy your open accessory.
## Windows VS C++
works.
* Make a new Project. 
* Get usblibx working in this project.
* Add the libary.
## Limitations
* At the moment you can only use blocking functions. Feel free to contribute such functionality.
* Not well tested. 
* Add a guide to use under linux without root privileges.
* Only tested with Nexsus S

## Why C++
I have to do some c++ in university, so I choose this project to refresh some c++ and use some of the new
c++11 features like array, auto usw. I think the project isn't comples, feel free to build a c version or 
copy it to python via pyusb. Remember the license ;)

## Author
Alexander Schrode (midix01[at]googlemail.com)
