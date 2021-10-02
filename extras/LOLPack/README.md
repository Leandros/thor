# LOLPack

The LOLPack format is the format used for storing all binary data used by the
game at runtime. It can be read using the built-in API and created using the
lolpack.exe utility.

## Format

A LOLPack archive consists of multiple files, some of which are only required
at build time, some are required at runtime.

#### ChangeList (.lst)

Changelist files are only required at build time, they contain all files
which are currently in the archives, together with the last date they were
changed.
This is only used to determine if a file has been updated in the development
process and requires repacking.
A binary template for changelists is included.


#### C Header File (.h)

The generated header file for inclusion into C and C++ sources contains the
hashes by which files can be refered to at compile time by the application
using LOLPack archives.
This removes the need of:

- including file name strings in the exe
- hashing of file names at runtime or via another preprocessor


#### Data Files (.dat)

The data files contain the raw data of each file, optionally compressed,
encrypted or otherwise stored.
A binary template for data files is included.


#### Index Files (.idx)

Each data file has an index file with the same name, it contains information
about each file, how it was stored, where it was stored and other information
(for example locale). Using this information one can retrieve files by their
name or content hashes.
A binary template for data files is included.


### Meta Index (.xdi)

The meta file contains an index of each file, linking the hash of the name to
the .idx files.
A binary template for meta indices is included.



## Creating Archives

Input Data:

    struct input {
        u64 hash_name;
        u64 hash_content;
        u64 file_size;
        u8 name[];
    };


