# ATCommander

Portable C++ library for sending AT commands and parsing their responses

Designed for resource contrained (embedded) environments, but should also be useful elsewhere

Since this uses C++ iostream-like approach, this library defaults to a blocking
behavior.  See notes below about this topic.  

Dependencies
------------

This library depends on https://github.com/malachib/util.embedded library.  Specifically,
it utilizes a lightweight C++ iostream implementation suitable for embedded environments.

Because of this, this library is compatible with:

* mbed OS
* Arduino

In theory this library should be able to adapt to standard C++ iostream as well, but this
is not tested or planned.

Notes
-----

Though billed as a blocking library, each command is decomposable into a request
and response.  This affords us the possibility to buffer the output, and come back later to
process the input.  I wouldn't call this full async, but it's better than a straight up blocking
library.
