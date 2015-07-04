Tested these instructions on a xubuntu 14.04

Prerequisites:
Cmake version 3.2, g++ version 4.8, bison version 3.0.3, flex
(note: sometimes the newer bison is included in the installs of older versions)

Instructions:
start a command line in the root directory of the git project
navigate to code/TweeZcodeCompiler
after that execute:
cmake CMakeLists.txt
after that, make is ready to be executed:
make all
now navigate to main/
in there TweeZcodeCompiler is the finished product

Usage: TweeZcodeCompiler [-d] [-o outputfile] [-a] <inputfile>

  -d				print debug logs to console
  -o <outputfile>	use specified output instead of default (name of input file with .z8 extension)
  -a				treat input file as Z-machine assembly instead of Twee source (cannot be used in conjunction with -s)
  -s				generate readable Z-machine assembly instead of Z-program (cannot be used in conjunction with -a)