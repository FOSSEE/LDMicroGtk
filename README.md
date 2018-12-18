# About LDMicro
LDmicro is a ladder logic editor, simulator and compiler for 8-bit microcontrollers. It can generate native code for Atmel AVR and Microchip PIC16 CPUs from a ladder diagram.

# LDMicro for Linux
This is a Linux compatible version of the original LDMicro program for windows created by Jonathan Westhues, and ported to Linux under [FOSSEE](https://github.com/FOSSEE) fellowship program 2018 by [Ramana R](https://github.com/Rr42) and [Rohit I](https://github.com/NatsuDrag9), under the mentorship of [Akshay C](https://github.com/akshay-c).

GitHub repository of LDMicro for windows can be found [here](https://github.com/akshay-c/LDmicro).

# Instructions
Before modifying contents of this git account, kindly make sure that you do not add unnecessary files that are created during the make process. In order to do so ensure that you add appropriate command to the makefile and execute "make clean" script before uploading your changes to git.

# External package dependencies
The install commands for all the packages required to compile LDMicro for Linux are given below:

_Note_: Be sure to run `sudo apt-get update` and `sudo apt-get upgrade` before executing the following commands

* Cmake: `sudo apt-get install cmake`
* GTK3: `sudo apt-get install libgtk-3-dev`
* MinGW: 
  ```
  sudo apt-get install gcc-mingw-w64
  sudo apt-get install g++-mingw-w64
  ```
* Perl: `sudo apt-get install perl`

# Building LDMicro for Linux
LDmicro for Linux is built using MinGW C++ compiler. 

### Generating the Makefile
The program is compiled using the 'CMake' utility. In order to generate the Makefile:
* Create a 'build' directory under 'ldmicro' subfolder at your clone location. The purpose is to keep the compilation mess separate from the original code.
* Now run 'cmake' command with path to the CMakeList file. In our case, the 'CmakeLists.txt' file resides under the parent directory of 'build' subdirectory
eg:
```
cmake ..
```

### Compiling and creating executable
Simply run `make` in the build directory of the project to compile.

_Note_: In order to compile the Makefile must first be generated (see [Building the Makefile](#building-the-makefile)).

Multiple Perl Scripts are executed during the build phase. In order to execute these scripts, to install the perl packages from the terminal (see [External package dependencies](#external-package-dependencies)).



## Running and testing LDMicro for Linux
### To run program in shell mode use the below command
`./LDmicro /c <.ld file to compile> <.hex destination file>`

### To run program in GUI mode use the below command
`sudo ./LDMicro`
