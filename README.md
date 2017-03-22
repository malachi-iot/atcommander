# ATCommander

Portable C++ library for sending AT commands and parsing their responses

Designed for resource contrained (embedded) environments, but should also be useful elsewhere

Since this uses C++ iostream-like approach, this is a *blocking* library.  

Dependencies
------------

This library depends on https://github.com/malachib/util.embedded library.  Specifically,
it utilizes a lightweight C++ iostream implementation suitable for embedded environments.

In theory this library should be able to adapt to standard C++ iostream as well, but this
is not tested or planned.
