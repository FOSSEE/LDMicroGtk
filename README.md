# LDMicro for Linux
This is a linux compatible version of the original LDMicro program.

GitHub repository of LDMicro for windows can be found [here](https://github.com/akshay-c/LDmicro)

## External package dependencies
The install commands for all the packages required to compile LDMicro for Linux are given below:

_Note_: Be sure to run `sudo apt-get update` and `sudo apt-get upgrade` before running the following commands

* Cmake: `sudo apt-get install cmake`
* GTK3: `sudo apt-get install libgtk-3-dev`
* MinGW: 
  ```
  sudo apt-get install gcc-mingw-w64
  sudo apt-get install g++-mingw-w64
  ```
* Perl: `sudo apt-get install perl`

## LDMicro for Linux can be compiled and run using the following commands
### To cmpile using cmake use the following commands
```
cd /LDmicro/
mkdir build
cd build/
cmake ..
make
```
### To run program in linux use the following command

# To run program in shell mode use the below command
`./LDmicro /c <.ld file to compile> <.hex destination file>`

# To run program in GUI mode use the below command
`sudo ./LDMicro`
