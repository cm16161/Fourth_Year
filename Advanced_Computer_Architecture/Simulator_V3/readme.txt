Compilation:

If cmake version 3.15 is available then:

Enter the build directory
mkdir build && cd build/

Generate the Makefile

cmake ..

Make the program

make

If cmake is not available then:

In the project root directory:

mkdir bin && cd bin

Compile the program:

g++ -std=c++11 ../src/*.cpp -o simulator

Usage:

Find the binary in the bin/ directory

cd bin/

Run the simulator by passing in the path to test program files
./simulator /path/to/assembly/text/files

NB. Program files can be found in the apps/ directory

Default execution (with no arguments) will execute "apps/all_ops.txt"
