# Feabhas CMake Project Notes

# Basic Usage

The Feabhas project build process uses [CMake](https://cmake.org/) as the underlying
build system. CMake is itself a build system generator and we have configured
it to generate the build files used by [GNU Make](https://www.gnu.org/software/make/).

Using CMake is a two step process: generate build files and then build. To simplify 
this and to allow you to add additional source and header files we have 
created a front end script to automate the build.

You can add additional C/C++ source and header files to the `src` directory. If 
you prefer you can place you header files in the `include` directory.

## To build the application

At the project root do:

```
$ ./build.sh
```

This will generate the file `build/debug/Application.elf` on an Arm target 
project or `build/debug/Application` on the host. Additional size and hex files
used by some flash memory software tools are generated for the Arm target.

You can add a `-v` option see the underlying build commands:

```
$ ./build.sh -v
```

## To clean the build

To delete all object files and recompile the complete project use
the `clean` option:

```
$ ./build.sh clean
```

To clean the entire build directory and regenerate a new build configuration use
the `reset` option:

```
$ ./build.sh reset
```
# Using VS Code

VS Code has been configured with tasks to build the code and run a gdb session.

## VS Build Tasks

Use the keyboard shortcut `Ctrl-Shift-B` to run the default build.

From within VS Code you can use the keyboard shortcut `Ctrl-Shift-B` 
to run one of the build tasks:
    * **Build** standard build
    * **Clean** to remove object and executable files
    * **Reset** to regenerate the CMake build files

To run the application use `Ctrl-Shift-P` shortcut key, enter test in 
the search field and then select `Tasks: Run Test Task` from the list of tasks shown. 
The next time you use `Ctrl-Shift-P` the `Tasks: Run Test Task` will be at the top of the list. 
    
Run tasks are project specific. For the target
    * **Run QEMU** to run the emulator using `./run-qemu.sh` 
    * **Run QEMU nographic** to run the emulator using `./run-qemu.sh --nographic` 
    * **Run QEMU serial** to run the emulator using `./run-qemu.sh serial` 
    * **Run QEMU serial nographic** to run the emulator using `./run-qemu.sh --nographic serial` 

For the host:
    * **Run Application** to run the built executable 
    
## VS Debug

To debug your code with the interactive (visual) debugger press the `<F5>` key or use the
**Run -> Start Debugging** menu.

The debug sessions with stop at the entry to the `main` function and *may* display 
a red error box saying:

```
Exception has occurred.
```

This is normal: just close the warning popup and use the debug icon commands at the top 
manage the debug system. The icons are (from left to right):
  **continue**, **stop over**, **step into**, **step return**, **restart** and **quit**

When working with QEMU additional debug launch tasks are available from the drop down list
at the top of the debug view. There are:

    * **QEMU debug** for a debug session with graphic WMS window
    * **QEMU nographic debug** for a debug session without the graphic WMS window
    * **QEMU serial debug** for a debug session using the serial port

Note that when using the debugger with a serial port you must use an external
Linux terminal to run Telnet and must have the graphic WMS window displayed.
  
# Building an exercise solution

To build any of the exercise solutions run the script:
```
$ ./build-one.sh N 
```
where `N` is the exercise number.

**NOTE:** this will copy all files in the `src` directory to the `src.bak` directory having
removed any files already present in `src.bak`.

Do not use the `build-all.sh` script as this will build each solution in turn and is used
as part of our Continuous Integration testing.

# Implementation Notes

The `build.sh` script and `CMakeLists.txt` files are configured to automate the
building projects as much as possible. The same configuration files can be used
for both C and C++ projects with both host and target environments. The
target environment also needs the Arm compiler settings file `toolchain-STM32F407.cmake`

The `build-one.sh` and `build-all.sh` scripts work with both scons and CMake (default): 
   1. CMake is used if there is a `CMakeFiles.txt` file present
   2. scons if `SConstruct` is present
   3. otherwise the build is rejected
 
## Host and Target Projects

The host and target projects use separate `build` and `CMakeList.txt` files which 
have a lot in common. This description is based on the more complex target 
configuration on the basis that the host version simply omits the functionality
required for cross compiling to an Arm embedded system.

## Build Shell Script

The `build.sh` script will generate simple `main.c` or `main.cpp` sources files
for the appropriate project type if these are not present.

Basic build command options:

```
Usage: build.sh [options...]
  Wrapper around cmake build system.
  Options:
    clean      -- remove object files
    reset      -- regenerate make files and do a clean build
    debug      -- build debug version (default)
    release    -- build release version
    test       -- run cmake with test target after a build
    --c        -- generate main.c if it doesnt exist
    --cpp      -- generate main.cpp if it doesnt exist
    --rtos     -- include RTOS middleware if not found automatically (target only)
    --verbose  -- add verbose option also -v
    --help     -- this help information also -h -?
    -Werror    -- adds -Werror to build
    -Dvar=val  -- define a CMake variable which must have a value
  
  Output written to build/debug (or build/release), executables:
      host:       build/debug/Application
      arm target: build/debug/Application.elf
  Generates compile_commands.json used by tools like clang-tidy.
  Script will generate a missing main.c/main.cpp based on the
  hostname: those staring with c- or ac- are C otherwise C++.
```

Neither template has a `src` folder, just the build files: a suitable
starter `main.c` or `main.cpp` file is generated when the build script is first run.

In the case of the target the following support folders:
   * drivers-c     // supporting files for C courses
   * drivers-cpp   // supporting files for C++ courses
   * middleware    // RTOS files for all courses
   * system        // Cortex-M files for all courses

## Creating source files main.c or main.cpp

The first time the `build.sh` script is run it will create any missing
`src` and `include` folders and generate the appropriate `main.c/main.cpp`
file. The VM hostnames are set to the course code so that on a VM the correct 
C or C++ file will be created on the assumption that host names starting
with `c-` or `ac-` will be C and all other hosts will be C++. The 
presence of the `system` folder is used to identify an Arm target project.

The course exercise build instructions could specify that the first time the script
is run the options `-c` or `-cpp` can be added to the command line to generate
the correct file for the course when not using a VM:

```
$ ./build.sh -c        # generate main.c
$ ./build.sh -cpp      # generate main.cpp
```

## Handling user added or renamed source files

The CMake configuration uses wildcards for the source files and headers 
in `src` and `include`. Once the make build files have been generated any 
additional files added to these directories will not be included in the build. 

To ensure user created files are automatically included in the build the 
`build.sh` script will detect changes to the source file list and touch 
the `CMakeLists.txt` file forcing CMake to generate new build files for the 
main project (but not the libraries). A `build.sh clean` will remove all
object files forcing a recompilation of all sources files (but not a regeneration
of the make build files).

The build script option `reset` deletes the build directory and asks CMake to
regenerate the make build files.

# Testing support

Create a sub-directory called `tests` with it's own `CMakeList.txt` and define
yoru test suite (you don't need to include `enable_testing()` as this is done
in the project root config).

Invoke the tests by adding the `test` option to the build command:

```
./build.sh test
```
Tests are only run on a successful build of the application and all tests.

You can also use `cmake` or `ctest` directly.

If a test won't compile the main application will still have been built. You can
temporarily rename the `tests` directory to stop CMake building the tests, but make
sure you run a `./build.sh reset` to regenerate the build scripts.

# Working with static analysis tools

The CMake build sets CMAKE_EXPORT_COMPILE_COMMANDS=ON to generates the command
build file `compile_commands.json` in the build folder. This file can be used
to run tools like clang-tidy.

## clang-tidy

To have CMake run clang-tidy (if present) define the clang-tidy command
in CMAKE_C_CLANG_TIDY and/or CMAKE_CXX_CLANG_TIDY. 

To add clang-tidy to all C compilations add the following
to CMakeLists.txt (check version of gcc being used) for the Arm 
target:

```
set(CMAKE_C_CLANG_TIDY 
    clang-tidy;-extra-arg=-I$ENV{HOME}/gcc-arm-none-eabi-10-2020-q4-major/arm-none-eabi/include)
```

Just use `clang-tidy` for CMAKE_C_CLANG_TIDY on the host.

Also, pass this on the `cmake` command line using the build script:

```
./build.sh -DCMAKE_C_CLANG_TIDY="clang-tidy;-extra-arg=-I${HOME}/gcc-arm-none-eabi-10-2020-q4-major/arm-none-eabi/include"
```

To run clang-tidy use the following command for a host build:

```
$ run-clang-tidy -p build/debug
```

An Arm target build requires clang-tidy to be given the location of the 
Arm GCC header files. The following commands will find the location of the 
header include directory at runtime (without hard coding the GCC compiler 
install location):

```
$ arm_gcc=$(which arm-none-eabi-gcc)
$ rm_inc=${arm_gcc%/*/*}/arm-none-eabi/include
$ run-clang-tidy -p build/debug -extra-arg=-I$arm_inc
```

