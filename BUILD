This project aims to be compilable and runnable on any POSIX-compatible operating
system, including GNU/Linux (and any other GNU variants), Darwin (OS X), BSD and
Windows (using a GNU/POSIX-compatibility layer like cygwin or MinGW).

Prerequisites:
CMake version 3.2, g++ version 4.8 or clang version 3.6.0, bison version 3.0.3, flex
(note: sometimes the newer bison is included in the installs of older versions)

Instructions:
- start a command line in the root directory of the git project
- navigate to code/TweeZcodeCompiler
- execute "cmake CMakeLists.txt"
- run "make all"
- navigate to main/. The program can be found there.

You can use one of the provided example story files:
./TweeZcodeCompiler -d ../../../examples/teststories/completeStories/helloworld.twee

Usage: TweeZcodeCompiler [-d] [-o outputfile] [-a] <inputfile>

  -d				print debug logs to console
  -o <outputfile>	use specified output instead of default (name of input file with .z8 extension)
  -a				treat input file as Z-machine assembly instead of Twee source (cannot be used in conjunction with -s)
  -s				generate readable Z-machine assembly instead of Z-program (cannot be used in conjunction with -a)
