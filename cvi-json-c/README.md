\mainpage

`cvi_json-c`
========

1. [Overview and Build Status](#overview)
2. [Building on Unix](#buildunix)
    * [Prerequisites](#installprereq)
    * [Build commands](#buildcmds)
3. [CMake options](#CMake)
4. [Testing](#testing)
5. [Building with `vcpkg`](#buildvcpkg)
6. [Linking to libcvi_json-c](#linking)
7. [Using cvi_json-c](#using)

JSON-C - A JSON implementation in C <a name="overview"></a>
-----------------------------------

Build Status
* [AppVeyor Build](https://ci.appveyor.com/project/hawicz/cvi_json-c) ![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/github/cvi_json-c/cvi_json-c?branch=master&svg=true)
* [Travis Build](https://travis-ci.org/cvi_json-c/cvi_json-c) ![Travis Build Status](https://travis-ci.org/cvi_json-c/cvi_json-c.svg?branch=master)

Test Status
* [Coveralls](https://coveralls.io/github/cvi_json-c/cvi_json-c?branch=master) [![Coverage Status](https://coveralls.io/repos/github/cvi_json-c/cvi_json-c/badge.svg?branch=master)](https://coveralls.io/github/cvi_json-c/cvi_json-c?branch=master)

JSON-C implements a reference counting object model that allows you to easily
construct JSON objects in C, output them as JSON formatted strings and parse
JSON formatted strings back into the C representation of JSON objects.
It aims to conform to [RFC 7159](https://tools.ietf.org/html/rfc7159).

Building on Unix with `git`, `gcc` and `cmake` <a name="buildunix"></a>
--------------------------------------------------

Home page for cvi_json-c: https://github.com/cvi_json-c/cvi_json-c/wiki

### Prerequisites: <a name="installprereq"></a>

 - `gcc`, `clang`, or another C compiler

 - `cmake>=2.8`, `>=3.16` recommended

To generate docs you'll also need:
 - `doxygen>=1.8.13`

If you are on a relatively modern system, you'll likely be able to install
the prerequisites using your OS's packaging system.

### Install using apt (e.g. Ubuntu 16.04.2 LTS)
```sh
sudo apt install git
sudo apt install cmake
sudo apt install doxygen  # optional
sudo apt install valgrind # optional
```

### Build instructions:  <a name="buildcmds"></a>

`cvi_json-c` GitHub repo: https://github.com/cvi_json-c/cvi_json-c

```sh
$ git clone https://github.com/cvi_json-c/cvi_json-c.git
$ mkdir cvi_json-c-build
$ cd cvi_json-c-build
$ cmake ../cvi_json-c   # See CMake section below for custom arguments
```

Note: it's also possible to put your build directory inside the cvi_json-c
source directory, or even not use a separate build directory at all, but
certain things might not work quite right (notably, `make distcheck`)

Then:

```sh
$ make
$ make test
$ make USE_VALGRIND=0 test   # optionally skip using valgrind
$ make install
```


### Generating documentation with Doxygen:

The library documentation can be generated directly from the source code using Doxygen tool:

```sh
# in build directory
make doc
google-chrome doc/html/index.html
```


CMake Options <a name="CMake"></a>
--------------------

The cvi_json-c library is built with [CMake](https://cmake.org/cmake-tutorial/),
which can take a few options.

Variable                     | Type   | Description
-----------------------------|--------|--------------
CMAKE_INSTALL_PREFIX         | String | The install location.
CMAKE_BUILD_TYPE             | String | Defaults to "debug".
BUILD_SHARED_LIBS            | Bool   | The default build generates a dynamic (dll/so) library.  Set this to OFF to create a static library only.
BUILD_STATIC_LIBS            | Bool   | The default build generates a static (lib/a) library.  Set this to OFF to create a shared library only.
DISABLE_STATIC_FPIC          | Bool   | The default builds position independent code.  Set this to OFF to create a shared library only.
DISABLE_BSYMBOLIC            | Bool   | Disable use of -Bsymbolic-functions.
DISABLE_THREAD_LOCAL_STORAGE | Bool   | Disable use of Thread-Local Storage (HAVE___THREAD).
DISABLE_WERROR               | Bool   | Disable use of -Werror.
ENABLE_RDRAND                | Bool   | Enable RDRAND Hardware RNG Hash Seed.
ENABLE_THREADING             | Bool   | Enable partial threading support.
OVERRIDE_GET_RANDOM_SEED     | String | A block of code to use instead of the default implementation of cvi_json_c_get_random_seed(), e.g. on embedded platforms where not even the fallback to time() works.  Must be a single line.

Pass these options as `-D` on CMake's command-line.

```sh
# build a static library only
cmake -DBUILD_SHARED_LIBS=OFF ..
```

### Building with partial threading support

Although cvi_json-c does not support fully multi-threaded access to
object trees, it has some code to help make its use in threaded programs
a bit safer.  Currently, this is limited to using atomic operations for
cvi_json_object_get() and cvi_json_object_put().

Since this may have a performance impact, of at least 3x slower
according to https://stackoverflow.com/a/11609063, it is disabled by
default.  You may turn it on by adjusting your cmake command with:
   -DENABLE_THREADING=ON

Separately, the default hash function used for object field keys,
lh_char_hash, uses a compare-and-swap operation to ensure the random
seed is only generated once.  Because this is a one-time operation, it
is always compiled in when the compare-and-swap operation is available.


### cmake-configure wrapper script

For those familiar with the old autoconf/autogen.sh/configure method,
there is a `cmake-configure` wrapper script to ease the transition to cmake.

```sh
mkdir build
cd build
../cmake-configure --prefix=/some/install/path
make
```

cmake-configure can take a few options.

| options | Description|
| ---- | ---- |
| prefix=PREFIX |  install architecture-independent files in PREFIX |
| enable-threading |  Enable code to support partly multi-threaded use |
| enable-rdrand | Enable RDRAND Hardware RNG Hash Seed generation on supported x86/x64 platforms. |
| enable-shared  |  build shared libraries [default=yes] |
| enable-static  |  build static libraries [default=yes] |
| disable-Bsymbolic |  Avoid linking with -Bsymbolic-function |
| disable-werror |  Avoid treating compiler warnings as fatal errors |


Testing:  <a name="testing"></a>
----------

By default, if valgrind is available running tests uses it.
That can slow the tests down considerably, so to disable it use:
```sh
export USE_VALGRIND=0
```

To run tests a separate build directory is recommended:
```sh
mkdir build-test
cd build-test
# VALGRIND=1 causes -DVALGRIND=1 to be passed when compiling code
# which uses slightly slower, but valgrind-safe code.
VALGRIND=1 cmake ..
make

make test
# By default, if valgrind is available running tests uses it.
make USE_VALGRIND=0 test   # optionally skip using valgrind
```

If a test fails, check `Testing/Temporary/LastTest.log`,
`tests/testSubDir/${testname}/${testname}.vg.out`, and other similar files.
If there is insufficient output try:
```sh
VERBOSE=1 make test
```
or
```sh
JSONC_TEST_TRACE=1 make test
```
and check the log files again.


Building on Unix and Windows with `vcpkg` <a name="buildvcpkg"></a>
--------------------------------------------------

You can download and install JSON-C using the [vcpkg](https://github.com/Microsoft/vcpkg/) dependency manager:

    git clone https://github.com/Microsoft/vcpkg.git
    cd vcpkg
    ./bootstrap-vcpkg.sh
    ./vcpkg integrate install
    vcpkg install cvi_json-c

The JSON-C port in vcpkg is kept up to date by Microsoft team members and community contributors. If the version is out of date, please [create an issue or pull request](https://github.com/Microsoft/vcpkg) on the vcpkg repository.


Linking to `libcvi_json-c` <a name="linking">
----------------------

If your system has `pkgconfig`,
then you can just add this to your `makefile`:

```make
CFLAGS += $(shell pkg-config --cflags cvi_json-c)
LDFLAGS += $(shell pkg-config --libs cvi_json-c)
```

Without `pkgconfig`, you would do something like this:

```make
JSON_C_DIR=/path/to/cvi_json_c/install
CFLAGS += -I$(JSON_C_DIR)/include/cvi_json-c
LDFLAGS+= -L$(JSON_C_DIR)/lib -lcvi_json-c
```


Using cvi_json-c <a name="using">
------------

To use cvi_json-c you can either include cvi_json.h, or preferrably, one of the
following more specific header files:

* cvi_json_object.h  - Core types and methods.
* cvi_json_tokener.h - Methods for parsing and serializing cvi_json-c object trees.
* cvi_json_pointer.h - JSON Pointer (RFC 6901) implementation for retrieving
                   objects from a cvi_json-c object tree.
* cvi_json_object_iterator.h - Methods for iterating over single cvi_json_object instances.  (See also `cvi_json_object_object_foreach()` in cvi_json_object.h)
* cvi_json_visit.h   - Methods for walking a tree of cvi_json-c objects.
* cvi_json_util.h    - Miscellaneous utility functions.

For a full list of headers see [files.html](http://cvi_json-c.github.io/cvi_json-c/cvi_json-c-current-release/doc/html/files.html)

The primary type in cvi_json-c is cvi_json_object.  It describes a reference counted
tree of cvi_json objects which are created by either parsing text with a
cvi_json_tokener (i.e. `cvi_json_tokener_parse_ex()`), or by creating
(with `cvi_json_object_new_object()`, `cvi_json_object_new_int()`, etc...) and adding
(with `cvi_json_object_object_add()`, `cvi_json_object_array_add()`, etc...) them 
individually.
Typically, every object in the tree will have one reference, from its parent.
When you are done with the tree of objects, you call cvi_json_object_put() on just
the root object to free it, which recurses down through any child objects
calling cvi_json_object_put() on each one of those in turn.

You can get a reference to a single child 
(`cvi_json_object_object_get()` or `cvi_json_object_array_get_idx()`)
and use that object as long as its parent is valid.  
If you need a child object to live longer than its parent, you can
increment the child's refcount (`cvi_json_object_get()`) to allow it to survive
the parent being freed or it being removed from its parent
(`cvi_json_object_object_del()` or `cvi_json_object_array_del_idx()`)

When parsing text, the cvi_json_tokener object is independent from the cvi_json_object
that it returns.  It can be allocated (`cvi_json_tokener_new()`)
used one or multiple times (`cvi_json_tokener_parse_ex()`, and
freed (`cvi_json_tokener_free()`) while the cvi_json_object objects live on.

A cvi_json_object tree can be serialized back into a string with 
`cvi_json_object_to_cvi_json_string_ext()`.  The string that is returned 
is only valid until the next "to_cvi_json_string" call on that same object.
Also, it is freed when the cvi_json_object is freed.

