# TODO


## General

- INI parser
    - read configuration for initial window parameters
- rename Config.(PLATFORM).mk -> (PLATFORM).mk

- Thread Safety
    - Document thread safety of each function. Is it "thread-safe", or just
      "re-entrant"? Document!

- Port to Windows. Sigh.
- Add 'NORETURN' to `dbg_assert` functions


## Build System

- Make 'make configure' take arguments, like a configure script, write them to
  a file, and read the file in the build process. Example of arguments:
    - build directory
    - extra build flags
    - et cetera

## Scripting

- integrate scripting language
    - ECL
    - chibi-scheme
    - terra
    - **own?**


## Infrastructure

- Jenkins
    - Seperate between 'master' and 'dev' branch

- Code analysis
    - gcov
    - ASan
    - UBSan
    - MSan
    - clangs static analyzer
    - tis-interpreter

## Memory

- Allocator with lifetime over multiple frames


## Container

- Array
    - Thin wrapper for statically allocated arrays, providing bounds checking
    if desired.
    - Purely implemented via header + inline functions

- Vector
    - Improved memory allocation strategy

- Hashmap
    - Improved memory allocation strategy

- String
    - Improved memory allocation strategy

- Deque / Queue / Stack
    - Add peek functions

- Memory allocation strategy proposals:
    - give it a fixed block of memory to work with?
    - give it function pointers to an allocator? makes the struct bigger.

- Macros to access data structures with type specified, to avoid going through
the many casts and dereferences to get from `void *` to the value of the correct
type. Won't give you typesafety, though.


## Allocator

- Linear / Stack Allocator

- Pool Allocator


## System

- Assert
    - Log all asserts to the main log file, even in release

